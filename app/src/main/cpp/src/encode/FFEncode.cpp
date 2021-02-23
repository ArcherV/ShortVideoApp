//
// Created by ArcherV on 2021-02-23.
//

#include "FFEncode.h"
#include "../utils/XParameter.h"
#include "../utils/Xlog.h"

bool FFEncode::Open(XParameter para) {
    Close();
    if (!para.para)
        return false;
    AVCodecParameters *p = para.para;
    //1 查找解码器
    AVCodec *cd = avcodec_find_encoder(p->codec_id);
    if (!cd) {
        XLOGE("Open", "avcodec_find_decoder %d failed", p->codec_id);
        return false;
    }
    XLOGE("Open", "avcodec_find_decoder success!");

    std::lock_guard<std::mutex> lck(mux);
    //2 创建解码上下文 并复制参数
    codec = avcodec_alloc_context3(cd);
    avcodec_parameters_to_context(codec, p);

    codec->thread_count = 8;

    //3 打开解码器
    int re = avcodec_open2(codec, 0, 0);
    if(re != 0){
        char buf[1024] = {0};
        av_strerror(re, buf, sizeof(buf) - 1);
        XLOGE("Open", "%s", buf);
        return false;
    }

    isAudio = !(codec->codec_type == AVMEDIA_TYPE_VIDEO);

    XLOGE("Open", "avcodec_open2 success!");
    return true;
}

void FFEncode::Close() {
    IEncode::Clear();
    std::lock_guard<std::mutex> lck(mux);
    if (frame)
        av_frame_free(&frame);
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

bool FFEncode::SendFrame(XData frame) {
    pts = frame.pts;
    return true;
}

XData FFEncode::RecvPacket() {
    if (pts == -1)
        return XData();
    XData data;
    data.pts = pts;
    pts = -1;
    return data;
}
