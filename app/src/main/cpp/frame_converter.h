#pragma once

#include <opencv2/core.hpp>

extern "C" {
#include <libavutil/frame.h>
#include <libswscale/swscale.h>
}

class FrameConverter {
public:
    FrameConverter();
    ~FrameConverter();

    cv::Mat convert(AVFrame* yuv_frame, int width, int height);

private:
    SwsContext* sws_ctx_ = nullptr;
    AVFrame* rgb_frame_ = nullptr;
};
