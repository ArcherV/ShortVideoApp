//
// Created by ArcherV on 2021-02-23.
//

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

IApp *FFAppBuidler::CreateApp() {
    return IApp::Get();
}
