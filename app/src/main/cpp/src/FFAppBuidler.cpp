//
// Created by ArcherV on 2021-02-23.
//

#include "SLAudioRecord.h"
#include "FFAppBuidler.h"
#include "FFEncode.h"
#include "Xlog.h"
#include "FFMux.h"

IEncode *FFAppBuidler::CreateEncode(bool isAudio) {
    IEncode *ffencode = new FFEncode(isAudio);
    return ffencode;
}

IMux *FFAppBuidler::CreateMux() {
    IMux *ffmux = new FFMux();
    return ffmux;
}

IAudioRecord *FFAppBuidler::CreateAudioRecord() {
    IAudioRecord *audioRecord = new SLAudioRecord();
    return audioRecord;
}

IApp *FFAppBuidler::CreateApp() {
    return IApp::Get();
}

void FFAppBuidler::CreateEnv() {
    // 太太太重要了，需要注册这俩家伙
    //注册所有封装器
    av_register_all();
    //注册所有的解码器
    avcodec_register_all();
}
