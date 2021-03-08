//
// Created by ArcherV on 2021-02-23.
//

#include "FFMux.h"
#include "Xlog.h"
#include "XUtils.h"

extern "C" {
#include <libavformat/avformat.h>
}

bool FFMux::Open(const char *url) {
    Close();
    std::lock_guard<std::mutex> lck(mux);
    int re = avformat_alloc_output_context2(&ic, nullptr, nullptr, url);
    if (re != 0) {
        char buf[1024] = {0};
        av_strerror(re, buf, sizeof(buf));
        XLOGE("FFMux::Open", "FFMux open %s failed! %s", url, buf);
        return false;
    }
    fmt = ic->oformat;
    if (!(fmt->flags & AVFMT_NOFILE)) {
        int ret = avio_open(&ic->pb, url, AVIO_FLAG_WRITE);
        if (ret < 0) {
            XLOGE("FFMux::Open", "Could not open '%s': %s", url, av_err2str(ret));
            return false;
        }
    }
    XLOGE("FFMux::Open", "FFMux open %s success!", url);
    return true;
}

bool FFMux::AddStream(bool isAudio, XParameter para) {
    AVStream *stream = avformat_new_stream(ic, nullptr);
    if (!stream) {
        XLOGE("FFMux::Open", "Stream open %s failed!", (isAudio ? "audio" : "video"));
        return false;
    }
    stream->id = stream->index = (isAudio ? AUDIO_STREAM : VIDEO_STREAM);
    avcodec_parameters_copy(stream->codecpar, para.para);
    nb_streams++;
    if (isAudio)
        audioStream = stream;
    else
        videoStream = stream;
    if (nb_streams == 2) {
        AVDictionary* opt = nullptr;
        av_dict_set_int(&opt, "video_track_timescale", 25, 0);
        int ret = avformat_write_header(ic, &opt);
        if (ret < 0) {
            XLOGE("FFMux::Open", "Error occurred when opening output file: %s", av_err2str(ret));
            return false;
        }
    }
    XLOGE("FFMux::Open", "Add stream %s success!", (isAudio ? "audio" : "video"));
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

void FFMux::Write(XData pkt) {
    if(pkt.size <= 0 || !pkt.data)
        return;
    std::lock_guard<std::mutex> lck(mux);
    XLOGE("FFMux::Write", "Packet type %s pts %ld size %d", (pkt.isAudio ? "audio" : "video"), ((AVPacket *)pkt.data)->pts, ((AVPacket *)pkt.data)->size);
    ((AVPacket *)pkt.data)->stream_index = (pkt.isAudio ? AUDIO_STREAM : VIDEO_STREAM);
    av_interleaved_write_frame(ic, (AVPacket *)pkt.data);
    av_packet_unref((AVPacket *)pkt.data);
}