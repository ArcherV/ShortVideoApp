//
// Created by ArcherV on 2021-02-23.
//

#include "FFEncode.h"
#include "../utils/XParameter.h"
#include "../utils/Xlog.h"
#include "../utils/XUtils.h"

bool FFEncode::Open() {
    // 太太太重要了，需要注册这俩家伙
    //注册所有封装器
    av_register_all();
    //注册所有的解码器
    avcodec_register_all();

    Close();
    //1 查找解码器
    AVCodec *cd = avcodec_find_encoder(AV_CODEC_ID_H264);;
    if (!cd) {
        XLOGE("Open", "avcodec_find_encoder failed");
        return false;
    }
    XLOGE("Open", "avcodec_find_encoder success!");

    std::lock_guard<std::mutex> lck(mux);
    //2 创建解码上下文 并复制参数
    codec = avcodec_alloc_context3(cd);

    codec->codec_id = AV_CODEC_ID_H264;
    codec->bit_rate = 5760000;
    codec->width = WIDTH;
    codec->height = HEIGHT;
    codec->time_base = (AVRational){1, 25};
    codec->framerate = (AVRational){25, 1};
    codec->gop_size = 10;
    codec->max_b_frames = 1;
    codec->pix_fmt = AV_PIX_FMT_YUV420P;
    codec->profile = FF_PROFILE_H264_BASELINE;
    // TODO：多线程FFMpeg
//    codec->thread_count = 8;

    //3 打开解码器
    int re = avcodec_open2(codec, cd, nullptr);
    if(re != 0){
        char buf[1024] = {0};
        av_strerror(re, buf, sizeof(buf) - 1);
        XLOGE("Open", "%s", buf);
        return false;
    }

    frame = av_frame_alloc();
    if (!frame) {
        XLOGE("Open", "Could not allocate video frame");
        return false;
    }

    frame->format = codec->pix_fmt;
    frame->width = codec->width;
    frame->height = codec->height;

    re = av_frame_get_buffer(frame, 32);
    if (re < 0) {
        XLOGE("Open", "Could not allocate the video frame data");
        return false;
    }

    isAudio = !(codec->codec_type == AVMEDIA_TYPE_VIDEO);

    XLOGE("Open", "avcodec_open2 success!");
    return true;
}

void FFEncode::Close() {
    IEncode::Clear();
    std::lock_guard<std::mutex> lck(mux);
    if (codec)
        avcodec_free_context(&codec);
    if (frame) {
        av_frame_free(&frame);
    }
    if (packet)
        av_packet_free(&packet);
    if (codec) {
        avcodec_close(codec);
        avcodec_free_context(&codec);
    }
}

void FFEncode::Clear() {
    IEncode::Clear();
    std::lock_guard<std::mutex> lck(mux);
    if (codec)
        avcodec_flush_buffers(codec);
}

bool FFEncode::SendFrame(XData fr) {
    if (fr.size <= 0 || !fr.data)
        return false;
    std::lock_guard<std::mutex> lck(mux);
    rgbToFrame(WIDTH, HEIGHT, fr.data, frame, RGBA_YUV420P);
    frame->pts = fr.pts;
    if (!codec)
        return false;
    int re = avcodec_send_frame(codec, frame);
    if (re != 0) {
        char buf[1024] = {0};
        av_strerror(re, buf, sizeof(buf) - 1);
        XLOGE("SendFrame", "%s", buf);
        return false;
    }
    return re == 0;
}

XData FFEncode::RecvPacket() {
    std::lock_guard<std::mutex> lck(mux);
    if (!codec)
        return XData();
    if (!packet)
        packet = av_packet_alloc();

    int re = avcodec_receive_packet(codec, packet);
    if (re != 0)
        return XData();
    XLOGE("av_frame_get_buffer", "Write packet %ld (size=%d)", packet->pts, packet->size);
    XData data;
    data.width = codec->width;
    data.height = codec->height;
    data.pts = packet->pts;
    av_packet_unref(packet);
    return data;
}
