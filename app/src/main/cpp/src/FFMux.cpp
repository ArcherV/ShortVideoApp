//
// Created by ArcherV on 2021-02-23.
//

#include "FFMux.h"
#include "Xlog.h"
#include "XUtils.h"

extern "C" {
#include <libavformat/avformat.h>
}

FFMux::FFMux() {
    static bool isFirst = true;
    if (isFirst) {
        isFirst = false;
        //注册所有封装器
        av_register_all();
        //注册所有的解码器
        avcodec_register_all();

        XLOGE("FFMux", "Register ffmpeg!");
    }
}

bool FFMux::Open(const char *url) {
    Close();
    std::lock_guard<std::mutex> lck(mux);
    int re = avformat_alloc_output_context2(&ic, nullptr, nullptr, url);
    if (re != 0) {
        char buf[1024] = {0};
        av_strerror(re, buf, sizeof(buf));
        XLOGE("Open", "FFMux open %s failed!", url);
        return false;
    }
    XLOGE("Open", "FFMux open %s success!", url);
    fmt = ic->oformat;
//    XLOGE("Open", "Context is null ? %s", ic == nullptr ? "Yes" : "No");
    AVStream *stream = avformat_new_stream(ic, nullptr);
//    XLOGE("Open", "stream is null ? %s", stream == nullptr ? "Yes" : "No");
    if (!stream) {
        XLOGE("Open", "Stream open %s failed!", url);
        return false;
    }
    stream->id = 0;
    stream->codecpar->width = WIDTH;
    stream->codecpar->height = HEIGHT;
    stream->time_base = (AVRational){1, FRAMERATE};
    stream->index = 0;
    XLOGE("Open", "Stream is null %d", stream == nullptr);
    return true;
}

void FFMux::Close() {
    std::lock_guard<std::mutex> lck(mux);
    if (ic)
        avformat_close_input(&ic);
}

XParameter FFMux::GetVPara() {
    std::lock_guard<std::mutex> lck(mux);
    if (!ic) {
        XLOGE("GetVPara", "GetVPara failed! ic is NULL! ");
        return XParameter();
    }

    //获取了视频流索引
    int re = av_find_best_stream(ic, AVMEDIA_TYPE_VIDEO, -1, -1, 0, 0);
    if(re < 0) {
        XLOGE("GetVPara", "av_find_best_stream failed!");
        return XParameter();
    }
    videoStream = re;
    XParameter para;
    para.para = ic->streams[re]->codecpar;
    para.time_base = &(ic->streams[re]->time_base);
    return para;
}

XParameter FFMux::GetAPara() {
    std::lock_guard<std::mutex> lck(mux);
    if (!ic) {
        XLOGE("GetAPara", "GetVPara failed! ic is NULL! ");
        return XParameter();
    }

    //获取了音频流索引
    int re = av_find_best_stream(ic, AVMEDIA_TYPE_AUDIO, -1, -1, 0, 0);
    if (re < 0) {
        XLOGE("GetAPara", "av_find_best_stream failed!");
        return XParameter();
    }

    audioStream = re;
    XParameter para;
    para.para = ic->streams[re]->codecpar;
    para.time_base = &(ic->streams[re]->time_base);
    return para;
}

void FFMux::Write(XData pkt) {
    if(pkt.size <= 0 || !pkt.data)
        return;
    std::lock_guard<std::mutex> lck(mux);
//    ((AVPacket *)pkt.data)->stream_index = stream->index;
    XLOGE("Write", "Packet pts %ld", ((AVPacket *)pkt.data)->pts);

    av_interleaved_write_frame(ic, (AVPacket *)pkt.data);
    // 一旦使用av_packet_unref就会释放掉packet的内存空间
    av_packet_unref((AVPacket *)pkt.data);
}