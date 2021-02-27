//
// Created by ArcherV on 2021-02-23.
//

#include "FFEncode.h"
#include "XParameter.h"
#include "Xlog.h"
#include "XUtils.h"

bool FFEncode::Open() {
    // 太太太重要了，需要注册这俩家伙
    //注册所有封装器
    av_register_all();
    //注册所有的解码器
    avcodec_register_all();

    Close();
    //1 查找解码器
    AVCodec *cd = avcodec_find_encoder(VCODEC_ID);;
    if (!cd) {
        XLOGE("Open", "avcodec_find_encoder failed");
        return false;
    }
    XLOGE("Open", "avcodec_find_encoder success!");

    std::lock_guard<std::mutex> lck(mux);
    //2 创建解码上下文 并复制参数
    codec = avcodec_alloc_context3(cd);

    codec->codec_id = VCODEC_ID;
    codec->bit_rate = BIT_RATE;
    codec->width = WIDTH;
    codec->height = HEIGHT;
    codec->time_base = (AVRational){1, FRAMERATE};
    codec->framerate = (AVRational){FRAMERATE, 1};
    codec->gop_size = GOP_SIZE;
    codec->max_b_frames = MAX_B_FRAMES;
    codec->pix_fmt = PIX_FMT;
    codec->profile = PROFILE;
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
    XLOGE("RecvPacket", "Packet pts %ld (size=%d)", packet->pts, packet->size);
//    XLOGE("RecvPacket", "data size is %lu pack size is %lu", sizeof(data.data), sizeof(packet));
    AVPacket tmp = {0};
    av_packet_ref(&tmp, packet);
//    XLOGE("RecvPacket", "%ld", tmp.pts);
    XData data;
    data.type = AVPACKET_TYPE;
    data.width = codec->width;
    data.height = codec->height;
    data.pts = packet->pts;
    data.data = (u_char *)&tmp;
    data.size = packet->size;
    XLOGE("RecvPacket", "Packet pts %ld", ((AVPacket *)data.data)->pts);
    XLOGE("RecvPacket", "pointer %ld", data.data);
    return data;
}
