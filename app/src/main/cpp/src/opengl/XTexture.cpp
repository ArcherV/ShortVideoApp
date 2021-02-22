//
// Created by ArcherV on 2021-02-22.
//

#include <mutex>
#include "XTexture.h"
#include "XEGL.h"
#include "../utils/Xlog.h"
#include "XShader.h"

class CXTexture:public XTexture
{
public:
    XShader sh;
    std::mutex mux;

    virtual void Drop()
    {
        std::lock_guard<std::mutex> lck(mux);
        XEGL::Get()->Close();
        sh.Close();
        delete this;
    }

    virtual bool Init(void *win, const char *vertex, const char *fragment)
    {
        std::lock_guard<std::mutex> lck(mux);
        if(!win){
            XLOGE("Init", "XTexture Init failed win is NULL");
            return false;
        }

        if(!XEGL::Get()->Init(win)){
            return false;
        }
        sh.Init(vertex, fragment);
        return true;
    }

    virtual GLuint CreateTexture() override {
        if (texture == 0) {
            glGenFramebuffers(1, &framebuffer);
            glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 720, 1280, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebuffer, 0);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
        return texture;
    }

    virtual void Draw(unsigned char *data[], int width, int height, TextureType type) override
    {
        std::lock_guard<std::mutex> lck(mux);

        sh.GetTexture(texture, width, height, data[0]);        //Y

        sh.Draw(type);
        XEGL::Get()->Draw();
    }

};

XTexture *XTexture::Get()
{
    static CXTexture texture;
    return &texture;
}
