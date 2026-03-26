#pragma once

#include <string>
#include <functional>
#include <atomic>

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
}

class RtspDecoder {
public:
    using FrameCallback = std::function<void(AVFrame* frame, int width, int height)>;

    RtspDecoder();
    ~RtspDecoder();

    bool open(const std::string& rtsp_url);
    void start(FrameCallback on_frame);
    void stop();
    bool is_running() const { return running_.load(); }

private:
    AVFormatContext* fmt_ctx_ = nullptr;
    AVCodecContext* codec_ctx_ = nullptr;
    int video_stream_idx_ = -1;
    std::atomic<bool> running_{false};

    bool find_video_stream();
    bool open_codec();
    void decode_loop(FrameCallback on_frame);
};
