#include "face_detector.h"
#include <android/log.h>

#define TAG "FaceDetector"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)

bool FaceDetector::init(const std::string& cascade_path) {
    if (!cascade_.load(cascade_path)) {
        LOGE("Failed to load cascade: %s", cascade_path.c_str());
        return false;
    }
    LOGI("Cascade loaded successfully");
    return true;
}

std::vector<FaceRect> FaceDetector::detect(const cv::Mat& rgb_frame) {
    cv::cvtColor(rgb_frame, gray_, cv::COLOR_RGB2GRAY);
    cv::equalizeHist(gray_, gray_);

    std::vector<cv::Rect> cv_faces;
    cascade_.detectMultiScale(
        gray_, cv_faces,
        1.1,                  // scale factor
        3,                    // min neighbors
        0,                    // flags
        cv::Size(80, 80)      // min face size
    );

    std::vector<FaceRect> faces;
    faces.reserve(cv_faces.size());
    for (const auto& r : cv_faces) {
        faces.push_back({r.x, r.y, r.width, r.height});
    }
    return faces;
}

void FaceDetector::draw_faces(cv::Mat& frame, const std::vector<FaceRect>& faces) {
    for (const auto& f : faces) {
        cv::rectangle(frame,
            cv::Point(f.x, f.y),
            cv::Point(f.x + f.width, f.y + f.height),
            cv::Scalar(0, 255, 0), 3);
    }
}
