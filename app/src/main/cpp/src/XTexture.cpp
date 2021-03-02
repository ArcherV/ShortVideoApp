//
// Created by ArcherV on 2021-02-22.
//

#include <mutex>
#include "XTexture.h"
#include "XEGL.h"
#include "Xlog.h"
#include "XShader.h"
#include "XUtils.h"

class CXTexture:public XTexture
{
public:
    void Update(XData data) override {

    }

    virtual void Drop()
    {
        std::lock_guard<std::mutex> lck(mux);
        XEGL::Get()->Close();
        sh.Close();
        delete this;
    }

    virtual bool Init(void *win, const char *vertex, const char *fragmentOES, const char *fragment)
    {
        std::lock_guard<std::mutex> lck(mux);
        if(!win){
            XLOGE("Init", "XTexture Init failed win is NULL");
            return false;
        }

        if(!XEGL::Get()->Init(win)){
            return false;
        }
        sh.Init(vertex, fragmentOES, fragment);
        matrixSetIdentityM(matrix1);
//        matrixSetIdentityM(matrix2);
        matrixSetRotateM(matrix2, 90, 0.0f, 1.0f, 0.0f);
        matrixRotateM(matrix2, 0, 0.0f, 0.0f, 1.0f);
        pixs_buffer = new u_char[WIDTH * HEIGHT * 4];
        num = 0;
        return true;
    }

    virtual GLuint CreateTexture() override {
        if (texture2D == 0) {
            glGenFramebuffers(1, &framebuffer);
            glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
            glGenTextures(1, &texture2D);
            glBindTexture(GL_TEXTURE_2D, texture2D);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, WIDTH, HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebuffer, 0);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
        return texture2D;
    }

    virtual GLuint CreateTextureOES() override {
        if (textureOES == 0) {
            glGenTextures(1, &textureOES);
            glBindTexture(GL_TEXTURE_EXTERNAL_OES, textureOES);
            glTexParameterf(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameterf(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glBindTexture(GL_TEXTURE_EXTERNAL_OES, 0);
        }
        return textureOES;
    }

    void DrawOES() override {
        std::lock_guard<std::mutex> lck(mux);
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        sh.GetTexture(textureOES);
        sh.Draw(TYPE_OES, nullptr);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glBindTexture(GL_TEXTURE_2D, texture2D);
        sh.Draw(TYPE_2D, matrix1);

        XEGL::Get()->Draw();

        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        sh.Draw(TYPE_2D, matrix2);
        glReadPixels(0, 0, WIDTH, HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, pixs_buffer);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        XData data;
        data.Alloc(WIDTH * HEIGHT * 4, pixs_buffer);
        data.width = WIDTH;
        data.height = HEIGHT;
        data.pts = ++num;
        Notify(data);
    }

private:
    XShader sh;
    float matrix1[16];
    float matrix2[16];
    int num;
    std::mutex mux;
};

XTexture *XTexture::Get()
{
    static CXTexture texture;
    return &texture;
}
