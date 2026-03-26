#include "video_pipeline.h"
#include <android/native_window_jni.h>
#include <android/log.h>

#define TAG "VideoPipeline"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)

VideoPipeline::VideoPipeline() = default;

VideoPipeline::~VideoPipeline() {
    stop();
}

bool VideoPipeline::start(const std::string& rtsp_url, ANativeWindow* window) {
    window_ = window;
    ANativeWindow_acquire(window_);

    // Initialize face detector with OpenCV's bundled Haar cascade
    std::string cascade_path = "/data/local/tmp/haarcascade_frontalface_alt.xml";
    if (!detector_.init(cascade_path)) {
        LOGE("Face detector init failed, continuing without detection");
    }

    if (!decoder_.open(rtsp_url)) {
        LOGE("Failed to open RTSP stream");
        ANativeWindow_release(window_);
        window_ = nullptr;
        return false;
    }

    decoder_.start([this](AVFrame* frame, int w, int h) {
        on_frame(frame, w, h);
    });

    LOGI("Pipeline started");
    return true;
}

void VideoPipeline::stop() {
    decoder_.stop();
    std::lock_guard<std::mutex> lock(render_mutex_);
    if (window_) {
        ANativeWindow_release(window_);
        window_ = nullptr;
    }
}

void VideoPipeline::on_frame(AVFrame* frame, int width, int height) {
    // Convert YUV to RGB
    cv::Mat rgb = converter_.convert(frame, width, height);

    // Run face detection
    auto faces = detector_.detect(rgb);
    face_count_ = static_cast<int>(faces.size());

    // Draw bounding boxes
    detector_.draw_faces(rgb, faces);

    // Render to Android surface
    render_to_surface(rgb);
}

void VideoPipeline::render_to_surface(const cv::Mat& frame) {
    std::lock_guard<std::mutex> lock(render_mutex_);
    if (!window_) return;

    ANativeWindow_setBuffersGeometry(window_, frame.cols, frame.rows,
                                      AHARDWAREBUFFER_FORMAT_R8G8B8A8_UNORM);

    ANativeWindow_Buffer buffer;
    if (ANativeWindow_lock(window_, &buffer, nullptr) != 0) return;

    // Convert RGB to RGBA and copy to surface buffer
    cv::Mat rgba;
    cv::cvtColor(frame, rgba, cv::COLOR_RGB2RGBA);

    uint8_t* dst = static_cast<uint8_t*>(buffer.bits);
    int dst_stride = buffer.stride * 4;
    int src_stride = rgba.step[0];
    int copy_width = std::min(src_stride, dst_stride);

    for (int y = 0; y < std::min(frame.rows, buffer.height); y++) {
        memcpy(dst + y * dst_stride, rgba.data + y * src_stride, copy_width);
    }

    ANativeWindow_unlockAndPost(window_);
}
