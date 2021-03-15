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

    virtual void Drop() override
    {
        std::lock_guard<std::mutex> lck(mux);
        egl.Close();
        sh.Close();
        delete this;
    }

    virtual bool Init(void *win) override
    {
        std::lock_guard<std::mutex> lck(mux);
        if(!win){
            XLOGE("Init", "XTexture Init failed win is NULL");
            return false;
        }

        if(!egl.Init(win, nullptr)){
            return false;
        }
        sh.Init();
        num = 0;
        return true;
    }

    virtual GLuint CreateTexture() override {
        glGenFramebuffers(pool_size, framebufferpool);
        glGenTextures(pool_size, texture2Dpool);
        for (int i = 0; i < pool_size; i++) {
            glBindFramebuffer(GL_FRAMEBUFFER, framebufferpool[i]);
            glBindTexture(GL_TEXTURE_2D, texture2Dpool[i]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, WIDTH, HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebufferpool[i], 0);
            glBindTexture(GL_TEXTURE_2D, 0);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }
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

        glBindFramebuffer(GL_FRAMEBUFFER, framebufferpool[textureIndex]);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_EXTERNAL_OES, textureOES);        //Y
        glViewport(0, 0, WIDTH, HEIGHT);
        sh.Draw(TYPE_OES, false);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glBindTexture(GL_TEXTURE_2D, texture2Dpool[textureIndex]);
        glViewport(0, 0, 1200, int((float)1200 / WIDTH * HEIGHT));
        sh.Draw(TYPE_2D, true);
        egl.Draw();

        XData data;
        data.texId = texture2Dpool[textureIndex];
        data.pts = ++num;

        textureIndex = (textureIndex + 1) % pool_size;

        Notify(data);
//        XLOGE("DrawOES", "生产 %d", num);
    }

    EGLContext getSharedContext() override {
        return egl.getSharedContext();
    }

private:
    XShader sh;
    XEGL egl;
    int num;
    std::mutex mux;
};

XTexture *XTexture::Get()
{
    static CXTexture texture;
    return &texture;
}
