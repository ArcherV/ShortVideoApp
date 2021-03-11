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
        sh.Init(true);
        num = 0;
        pixs_buffer = new uint32_t[HDR_WIDTH * HDR_HEIGHT * HDR_CHANNEL];
        FILE *in = fopen("/sdcard/new_img_2_10_10_10.bin", "rb");
        fread(pixs_buffer, HDR_WIDTH * HDR_HEIGHT * sizeof(uint32_t), 1, in);
        XLOGE("CXTexture::Init", "first short is %d", pixs_buffer[0]);
        CreateTexture();
        return true;
    }

    virtual GLuint CreateTexture() override {
        if (texture2D == 0) {
            glGenTextures(1, &texture2D);
            glBindTexture(GL_TEXTURE_2D, texture2D);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB10_A2, HDR_WIDTH, HDR_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_INT_2_10_10_10_REV, pixs_buffer);
            XLOGE("CreateTexture", "%d", glGetError());
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
        return texture2D;
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

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture2D);
        sh.Draw(TYPE_2D);
//        glClearColor(1, 0, 1, 1);
//        glClear(GL_COLOR_BUFFER_BIT);
        egl.Draw();
        glBindTexture(GL_TEXTURE_2D, 0);
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
