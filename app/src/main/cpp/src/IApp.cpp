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
    if (!audioRecord || !audioRecord->StartRecord()) {
        XLOGE("Open", "audioRecord->Open failed!");
        return false;
    }
    if(!aencode || !aencode->Open()){
        XLOGE("Open", "aencode->Open failed!");
        return false;
    }
    if(!readPixels || !readPixels->Open()){
        XLOGE("Open", "readPixels->Open failed!");
        return false;
    }
    if(!vencode || !vencode->Open()){
        XLOGE("Open", "vdecode->Open failed!");
        return false;
    }
    if(!muxer || !muxer->Open(path)){
        XLOGE("Open", "mux->Open %s failed!", path);
        return false;
    }
    muxer->AddStream(false, vencode->GetPara());
    muxer->AddStream(true, aencode->GetPara());
    return true;
}

bool IApp::Start() {
    std::lock_guard<std::mutex> lck(mux);
    if (audioRecord)
        audioRecord->Start();
    if (readPixels)
        readPixels->Start();
    if (muxer)
        muxer->Start();
    if (aencode)
        aencode->Start();
    if (vencode)
        vencode->Start();
    XThread::Start();
    return true;
}

void IApp::InitView(void *win) {
    XTexture::Get()->Init(win);
    readPixels->setSharedContext(XTexture::Get()->getSharedContext());
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
    if (readPixels)
        readPixels->Stop();
    if (aencode)
        aencode->Stop();
    if(vencode)
        vencode->Clear();
    if(readPixels)
        readPixels->Clear();
    if (aencode)
        aencode->Clear();
    if(vencode)
        vencode->Close();
    if(readPixels)
        readPixels->Close();
    if (aencode)
        aencode->Close();
    if (audioRecord) {
        audioRecord->Stop();
        audioRecord->Clear();
        audioRecord->Close();
    }
}

void IApp::Main() {
    while (!isExit) {
        std::lock_guard<std::mutex> lck(mux);
        // TODO
    }
}
