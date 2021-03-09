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
        egl.Close();
        sh.Close();
        delete this;
    }

    virtual bool Init(void *win)
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

    virtual void Draw(unsigned char *data[], int width, int height, TextureType type) override
    {
        std::lock_guard<std::mutex> lck(mux);

        sh.GetTexture(texture2D, width, height, data[0]);        //Y

        sh.Draw(type);
        egl.Draw();
    }

    void DrawOES() override {
        std::lock_guard<std::mutex> lck(mux);

        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_EXTERNAL_OES, textureOES);        //Y
        glViewport(0, 0, WIDTH, HEIGHT);
        sh.Draw(TYPE_OES);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glBindTexture(GL_TEXTURE_2D, texture2D);
        sh.Draw(TYPE_2D);
        egl.Draw();

        XData data;
        data.texId = texture2D;
        data.pts = ++num;
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
