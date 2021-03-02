//
// Created by ArcherV on 2021-02-24.
//

#include "IApp.h"
#include "Xlog.h"
#include "XTexture.h"

IApp *IApp::Get() {
    static IApp app;
    return &app;
}

bool IApp::Open(const char *path) {
    Close();
    //解码 如果解封装之后是原始数据解码可能不需要
    if(!vencode || !vencode->Open()){
        XLOGE("Open", "vdecode->Open failed!");
        return false;
    }
    if(!muxer || !muxer->Open(path, vencode->GetPara())){
        XLOGE("Open", "mux->Open %s failed!", path);
        return false;
    }
    return true;
}

bool IApp::Start() {
    std::lock_guard<std::mutex> lck(mux);
    if (muxer)
        muxer->Start();
    if (vencode)
        vencode->Start();
    XThread::Start();
    return true;
}

void IApp::InitView(void *win, const char *vertexShader, const char *fragmentOESShader, const char *fragmentShader) {
    XTexture::Get()->Init(win, vertexShader, fragmentOESShader, fragmentShader);
}

void IApp::Close() {
    std::lock_guard<std::mutex> lck(mux);
    XThread::Stop();
    if (muxer) {
        muxer->Stop();
        muxer->Clear();
        muxer->Close();
    }
    if (vencode)
        vencode->Stop();
    if(vencode)
        vencode->Clear();
    if(vencode)
        vencode->Close();
}

void IApp::Main() {
    while (!isExit) {
        std::lock_guard<std::mutex> lck(mux);
        // TODO
    }
}
