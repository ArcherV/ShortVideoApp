//
// Created by ArcherV on 2021-02-23.
//

#include "FFEncode.h"
#include "XParameter.h"
#include "Xlog.h"
#include "XUtils.h"

bool FFEncode::Open() {
    Close();
    //1 查找解码器
    AVCodec *cd = avcodec_find_encoder(isAudio ? ACODEC_ID : VCODEC_ID);
    if (!cd) {
        XLOGE("Open", "avcodec_find_encoder failed");
        return false;
    }
    XLOGE("Open", "avcodec_find_encoder success!");

    //2 创建解码上下文 并复制参数
    codec = avcodec_alloc_context3(cd);
    if (isAudio) {
        codec->sample_fmt = SAMPLE_FORMAT;
        codec->bit_rate = AUDIO_BIT_RATE;
        codec->sample_rate = SAMPLE_RATE;
        codec->codec_type = AVMEDIA_TYPE_AUDIO;
        codec->profile = FF_PROFILE_UNKNOWN;
        if (cd->supported_samplerates) {
            codec->sample_rate = cd->supported_samplerates[0];
            for (int i = 0; cd->supported_samplerates[i]; i++) {
                if (cd->supported_samplerates[i] == SAMPLE_RATE)
                    codec->sample_rate = SAMPLE_RATE;
            }
        }
        codec->channel_layout = CHANNEL_LAYOUT;
        codec->channels       = av_get_channel_layout_nb_channels(codec->channel_layout);
        if (cd->channel_layouts) {
            codec->channel_layout = cd->channel_layouts[0];
            for (int i = 0; cd->channel_layouts[i]; i++) {
                if (cd->channel_layouts[i] == CHANNEL_LAYOUT)
                    codec->channel_layout = CHANNEL_LAYOUT;
            }
        }
        codec->channels = av_get_channel_layout_nb_channels(codec->channel_layout);
        codec->time_base = (AVRational){1, SAMPLE_RATE};
    }
    else {
        codec->codec_type = AVMEDIA_TYPE_VIDEO;
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
    }
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
    if (isAudio) {
        frame->format = codec->sample_fmt;
        frame->channel_layout = codec->channel_layout;
        frame->sample_rate = codec->sample_rate;
        if (codec->codec->capabilities & AV_CODEC_CAP_VARIABLE_FRAME_SIZE)
            frame->nb_samples = 10000;
        else
            frame->nb_samples = codec->frame_size;
        re = av_frame_get_buffer(frame, 0);
        if (re < 0) {
            XLOGE("FFEncode::Open", "alloc_audio_frame", "Error allocating an audio buffer");
            return false;
        }
    }
    else
    {
        frame->format = codec->pix_fmt;
        frame->width = codec->width;
        frame->height = codec->height;

        re = av_frame_get_buffer(frame, 32);
        if (re < 0) {
            XLOGE("Open", "Could not allocate the video frame data");
            return false;
        }
    }
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
//    XLOGE("FFEncode::Clear", "codec is null ? %s", (codec == nullptr ? "Yes" : "No"));
    if (codec)
        avcodec_flush_buffers(codec);
}

bool FFEncode::SendFrame(XData fr) {
    if (fr.size <= 0 || !fr.data)
        return false;
    std::lock_guard<std::mutex> lck(mux);
    if (isAudio)
        memcpy(frame->data[0], fr.data, (size_t)fr.size);
    else
        rgbToFrame(WIDTH, HEIGHT, fr.data, frame, RGBA_YUV420P);
    frame->pts = fr.pts;
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
//    XLOGE("FFEncode::RecvPacket", "Packet pts %ld dts %ld size %d", packet->pts, packet->dts, packet->size);
    // 不能写成AVPacket tmp = {0};这种会有很多错误
    AVPacket *tmp = av_packet_alloc();
    av_packet_ref(tmp, packet);
    XData data;
    data.isAudio = isAudio;
    data.pts = packet->pts;
    data.data = (u_char *)tmp;
    data.size = packet->size;
    return data;
}

XParameter FFEncode::GetPara() {
    std::lock_guard<std::mutex> lck(mux);
    if (!codec) {
        XLOGE("GetPara", "GetPara failed! ic is NULL!");
        return XParameter();
    }
    XParameter para;
    para.para = avcodec_parameters_alloc();
    avcodec_parameters_from_context(para.para, codec);
    para.time_base = &codec->time_base;
    return para;
}
