#include "rtsp_decoder.h"
#include <android/log.h>
#include <thread>

#define TAG "RtspDecoder"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)

RtspDecoder::RtspDecoder() {
    avformat_network_init();
}

RtspDecoder::~RtspDecoder() {
    stop();
    if (codec_ctx_) avcodec_free_context(&codec_ctx_);
    if (fmt_ctx_) avformat_close_input(&fmt_ctx_);
}

bool RtspDecoder::open(const std::string& rtsp_url) {
    AVDictionary* opts = nullptr;
    av_dict_set(&opts, "rtsp_transport", "tcp", 0);
    av_dict_set(&opts, "stimeout", "5000000", 0);

    int ret = avformat_open_input(&fmt_ctx_, rtsp_url.c_str(), nullptr, &opts);
    av_dict_free(&opts);
    if (ret < 0) {
        char errbuf[256] = {0};
        av_strerror(ret, errbuf, sizeof(errbuf));
        LOGE("Failed to open RTSP stream: %s (ret=%d: %s)", rtsp_url.c_str(), ret, errbuf);
        return false;
    }

    if (avformat_find_stream_info(fmt_ctx_, nullptr) < 0) {
        LOGE("Failed to find stream info");
        return false;
    }

    if (!find_video_stream() || !open_codec()) return false;

    LOGI("Opened RTSP stream: %dx%d", codec_ctx_->width, codec_ctx_->height);
    return true;
}

bool RtspDecoder::find_video_stream() {
    for (unsigned i = 0; i < fmt_ctx_->nb_streams; i++) {
        if (fmt_ctx_->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            video_stream_idx_ = i;
            return true;
        }
    }
    LOGE("No video stream found");
    return false;
}

bool RtspDecoder::open_codec() {
    auto* params = fmt_ctx_->streams[video_stream_idx_]->codecpar;
    const AVCodec* codec = avcodec_find_decoder(params->codec_id);
    if (!codec) {
        LOGE("Unsupported codec: %d", params->codec_id);
        return false;
    }

    codec_ctx_ = avcodec_alloc_context3(codec);
    avcodec_parameters_to_context(codec_ctx_, params);

    if (avcodec_open2(codec_ctx_, codec, nullptr) < 0) {
        LOGE("Failed to open codec");
        return false;
    }
    return true;
}

void RtspDecoder::start(FrameCallback on_frame) {
    running_ = true;
    std::thread([this, on_frame]() { decode_loop(on_frame); }).detach();
}

void RtspDecoder::stop() {
    running_ = false;
}

void RtspDecoder::decode_loop(FrameCallback on_frame) {
    AVPacket* pkt = av_packet_alloc();
    AVFrame* frame = av_frame_alloc();

    while (running_) {
        if (av_read_frame(fmt_ctx_, pkt) < 0) {
            LOGE("Read frame failed, attempting reconnect...");
            break;
        }

        if (pkt->stream_index == video_stream_idx_) {
            if (avcodec_send_packet(codec_ctx_, pkt) == 0) {
                while (avcodec_receive_frame(codec_ctx_, frame) == 0) {
                    on_frame(frame, codec_ctx_->width, codec_ctx_->height);
                }
            }
        }
        av_packet_unref(pkt);
    }

    av_frame_free(&frame);
    av_packet_free(&pkt);
    running_ = false;
    LOGI("Decode loop ended");
}
