//
// Created by ArcherV on 2021-02-23.
//

#include "SLAudioRecord.h"
#include "FFAppBuidler.h"
#include "FFEncode.h"
#include "Xlog.h"
#include "FFMux.h"

IEncode *FFAppBuidler::CreateEncode() {
    IEncode *ffencode = new FFEncode();
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
