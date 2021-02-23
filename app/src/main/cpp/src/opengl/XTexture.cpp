//
// Created by ArcherV on 2021-02-22.
//

#include <mutex>
#include "XTexture.h"
#include "XEGL.h"
#include "../utils/Xlog.h"
#include "XShader.h"
#include "../utils/XUtils.h"

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
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 720, 1280, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
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

    virtual void Draw(unsigned char *data[], int width, int height, TextureType type) override
    {
        std::lock_guard<std::mutex> lck(mux);

        sh.GetTexture(texture2D, width, height, data[0]);        //Y

        sh.Draw(type);
        XEGL::Get()->Draw();
    }

    void DrawOES() override {
        std::lock_guard<std::mutex> lck(mux);

        sh.GetTexture(textureOES);        //Y

        sh.Draw(TYPE_OES);

        glReadPixels(0, 0, WIDTH, HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, pixs_buffer);
        XData data;
        data.Alloc(WIDTH * HEIGHT * 4, pixs_buffer);
        data.width = WIDTH;
        data.height = HEIGHT;
        data.pts = ++num;
        Notify(data);
        XLOGE("DrawOES", "生产 %d", num);

        XEGL::Get()->Draw();
    }

private:
    XShader sh;
    int num;
    std::mutex mux;
};

XTexture *XTexture::Get()
{
    static CXTexture texture;
    return &texture;
}
