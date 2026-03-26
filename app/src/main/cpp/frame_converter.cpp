#include "frame_converter.h"

extern "C" {
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
}

FrameConverter::FrameConverter() = default;

FrameConverter::~FrameConverter() {
    if (sws_ctx_) sws_freeContext(sws_ctx_);
    if (rgb_frame_) {
        av_freep(&rgb_frame_->data[0]);
        av_frame_free(&rgb_frame_);
    }
}

cv::Mat FrameConverter::convert(AVFrame* yuv_frame, int width, int height) {
    if (!sws_ctx_) {
        sws_ctx_ = sws_getContext(
            width, height, (AVPixelFormat)yuv_frame->format,
            width, height, AV_PIX_FMT_RGB24,
            SWS_BILINEAR, nullptr, nullptr, nullptr
        );
        rgb_frame_ = av_frame_alloc();
        av_image_alloc(rgb_frame_->data, rgb_frame_->linesize,
                       width, height, AV_PIX_FMT_RGB24, 1);
    }

    sws_scale(sws_ctx_,
        yuv_frame->data, yuv_frame->linesize, 0, height,
        rgb_frame_->data, rgb_frame_->linesize
    );

    return cv::Mat(height, width, CV_8UC3, rgb_frame_->data[0],
                   rgb_frame_->linesize[0]).clone();
}
