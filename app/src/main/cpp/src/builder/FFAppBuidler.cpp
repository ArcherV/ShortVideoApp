//
// Created by ArcherV on 2021-02-23.
//

#include "FFAppBuidler.h"
#include "../encode/FFEncode.h"
#include "../utils/Xlog.h"

IEncode *FFAppBuidler::CreateEncode() {
    IEncode *ffencode = new FFEncode();
    return ffencode;
}

IApp *FFAppBuidler::CreateApp() {
    return IApp::Get();
}
