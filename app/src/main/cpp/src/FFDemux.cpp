//
// Created by ArcherV on 2021-02-23.
//

#include "FFDemux.h"
#include "Xlog.h"

extern "C" {
#include <libavformat/avformat.h>
}

FFDemux::FFDemux() {
    static bool isFirst = true;
    if (isFirst) {
        isFirst = false;
        //注册所有封装器
        av_register_all();
        //注册所有的解码器
        avcodec_register_all();

        XLOGE("FFDemux", "Register ffmpeg!");
    }
}

bool FFDemux::Open(const char *url) {
    XLOGE("Open", "Open file %s begin", url);
}

void FFDemux::Close() {
    std::lock_guard<std::mutex> lck(mux);
    if (ic)
        avformat_close_input(&ic);
}

XParameter FFDemux::GetVPara() {
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

XParameter FFDemux::GetAPara() {
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

XData FFDemux::Read() {
    return XData();
}

void FFDemux::Write(XData) {

}
