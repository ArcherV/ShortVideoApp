//
// Created by ArcherV on 2021-03-09.
//

#ifndef SHORTVIDEOAPP_XREADPIXELS_H
#define SHORTVIDEOAPP_XREADPIXELS_H


#include "XShader.h"
#include "XEGL.h"
#include "IReadPixels.h"
#include "XData.h"

class XReadPixels : public IReadPixels {
public:
    bool Open() override ;

    void CreateEnv() override;

    XData ReadPixels(GLuint texture2D) override;

    void Clear() override;

    void Close() override;

protected:
    GLuint texture = 0;
    GLuint framebuffer = 0;
    u_char *pixs_buffer = nullptr;
    XShader sh;
    XEGL egl;
};


#endif //SHORTVIDEOAPP_XREADPIXELS_H
