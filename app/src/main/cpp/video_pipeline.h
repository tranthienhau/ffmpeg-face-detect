#pragma once

#include <atomic>
#include <mutex>
#include <android/native_window.h>
#include "rtsp_decoder.h"
#include "frame_converter.h"
#include "face_detector.h"

class VideoPipeline {
public:
    VideoPipeline();
    ~VideoPipeline();

    bool start(const std::string& rtsp_url, ANativeWindow* window);
    void stop();
    int face_count() const { return face_count_.load(); }

private:
    RtspDecoder decoder_;
    FrameConverter converter_;
    FaceDetector detector_;
    ANativeWindow* window_ = nullptr;
    std::atomic<int> face_count_{0};
    std::mutex render_mutex_;

    void on_frame(AVFrame* frame, int width, int height);
    void render_to_surface(const cv::Mat& frame);
};
