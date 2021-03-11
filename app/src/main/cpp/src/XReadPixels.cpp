//
// Created by ArcherV on 2021-03-09.
//

#include "Xlog.h"
#include "XUtils.h"
#include "XReadPixels.h"

void XReadPixels::CreateEnv() {
    egl.Init(nullptr, sharedContext);
    sh.Init(false);
    if (texture == 0) {
        glGenFramebuffers(1, &framebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glGenTextures(1, &texture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, WIDTH, HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebuffer, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    XLOGE("XReadPixels::CreateEnv", "texture %u", texture);
}

XData XReadPixels::ReadPixels(GLuint texture2D) {
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_EXTERNAL_OES, texture2D);
    glViewport(0, 0, WIDTH, HEIGHT);
    sh.Draw(TYPE_OES);
    glFinish();
    glReadPixels(0, 0, WIDTH, HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, pixs_buffer);
    glBindTexture(GL_TEXTURE_EXTERNAL_OES, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    XData data;
    data.Alloc(WIDTH * HEIGHT * 4, pixs_buffer);
    return data;
}

void XReadPixels::Clear() {
    delete[] pixs_buffer;
    pixs_buffer = nullptr;
}

void XReadPixels::Close() {
    IReadPixels::Close();
    texture = 0;
    framebuffer = 0;
    sh.Close();
}

bool XReadPixels::Open() {
    pixs_buffer = new u_char[WIDTH * HEIGHT * 4];
    return true;
}
