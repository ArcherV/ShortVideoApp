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

bool FFMux::Open(const char *url, XParameter para) {
    Close();
    std::lock_guard<std::mutex> lck(mux);
    int re = avformat_alloc_output_context2(&ic, nullptr, nullptr, url);
    if (re != 0) {
        char buf[1024] = {0};
        av_strerror(re, buf, sizeof(buf));
        XLOGE("FFMux::Open", "FFMux open %s failed! %s", url, buf);
        return false;
    }
    XLOGE("FFMux::Open", "FFMux open %s success!", url);
    fmt = ic->oformat;
    stream = avformat_new_stream(ic, nullptr);
    if (!stream) {
        XLOGE("FFMux::Open", "Stream open %s failed!", url);
        return false;
    }
    stream->id = stream->index = 0;
    avcodec_parameters_copy(stream->codecpar, para.para);

    if (!(fmt->flags & AVFMT_NOFILE)) {
        int ret = avio_open(&ic->pb, url, AVIO_FLAG_WRITE);
        if (ret < 0) {
            XLOGE("FFMux::Open", "Could not open '%s': %s", url, av_err2str(ret));
            return false;
        }
    }
    AVDictionary* opt = nullptr;
    av_dict_set_int(&opt, "video_track_timescale", 25, 0);
    int ret = avformat_write_header(ic, &opt);
    if (ret < 0) {
        XLOGE("FFMux::Open", "Error occurred when opening output file: %s", av_err2str(ret));
        return false;
    }
    return true;
}

void FFMux::Close() {
    std::lock_guard<std::mutex> lck(mux);
    if (ic) {
        av_write_trailer(ic);
        if (!(fmt->flags & AVFMT_NOFILE))
            /* Close the output file. */
            avio_closep(&ic->pb);
        avformat_free_context(ic);
    }
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
    ((AVPacket *)pkt.data)->stream_index = stream->index;
    av_interleaved_write_frame(ic, (AVPacket *)pkt.data);
    av_packet_unref((AVPacket *)pkt.data);
}