//
// Created by ArcherV on 2021-02-24.
//

#include "IAppProxy.h"
#include "../builder/FFAppBuidler.h"
#include "../opengl/XTexture.h"

bool IAppProxy::Open(const char *path) {
    std::lock_guard<std::mutex> lck(mux);
    return app && app->Open(path);
}

bool IAppProxy::Start() {
    std::lock_guard<std::mutex> lck(mux);
    return app && app->Start();
}

void IAppProxy::InitView(void *win, const char *vertexShader, const char *fragmentOESShader, const char *fragmentShader) {
    std::lock_guard<std::mutex> lck(mux);
    app->InitView(win, vertexShader, fragmentOESShader, fragmentShader);
}

void IAppProxy::Close() {
    std::lock_guard<std::mutex> lck(mux);
    app->Close();
}

bool IAppProxy::IsPause() {
    std::lock_guard<std::mutex> lck(mux);
    app->IsPause();
}

void IAppProxy::Init() {
    std::lock_guard<std::mutex> lck(mux);
    app = FFAppBuidler::Get()->BuilderApp();
}

GLuint IAppProxy::CreateTextureOES() {
    return XTexture::Get()->CreateTextureOES();
}

void IAppProxy::Render() {
    XTexture::Get()->DrawOES();
}
