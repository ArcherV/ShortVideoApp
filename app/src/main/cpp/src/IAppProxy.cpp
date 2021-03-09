//
// Created by ArcherV on 2021-02-24.
//

#include "IAppProxy.h"
#include "FFAppBuidler.h"
#include "XTexture.h"

bool IAppProxy::Open(const char *path) {
    std::lock_guard<std::mutex> lck(mux);
    return app && app->Open(path);
}

bool IAppProxy::Start() {
    std::lock_guard<std::mutex> lck(mux);
    return app && app->Start();
}

void IAppProxy::InitView(void *win) {
    std::lock_guard<std::mutex> lck(mux);
    app->InitView(win);
}

void IAppProxy::Close() {
    std::lock_guard<std::mutex> lck(mux);
    app->Close();
}

bool IAppProxy::IsPause() {
    std::lock_guard<std::mutex> lck(mux);
    return app->IsPause();
}

void IAppProxy::Init() {
    std::lock_guard<std::mutex> lck(mux);
    app = FFAppBuidler::Get()->BuilderApp();
}

GLuint IAppProxy::CreateTexture() {
    return XTexture::Get()->CreateTexture();
}

void IAppProxy::Render() {
    XTexture::Get()->DrawOES();
}
