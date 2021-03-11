//
// Created by ArcherV on 2021-02-22.
//

#ifndef SHORTVIDEOAPP_XTEXTURE_H
#define SHORTVIDEOAPP_XTEXTURE_H

#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>
#include <EGL/egl.h>
#include "XShader.h"
#include "IObserver.h"

class XTexture : public IObserver {
public:
    static XTexture *Get();
    virtual bool Init(void *win) = 0;
    virtual GLuint CreateTexture() = 0;
    virtual EGLContext getSharedContext() = 0;
    virtual void Draw(unsigned char *data[], int width, int heigth, TextureType type) = 0;
    virtual void DrawOES() = 0;
    virtual void Drop() = 0;

    virtual ~XTexture(){};

protected:
    XTexture(){};

    unsigned int textureOES = 0;
    unsigned int texture2D = 0;
    unsigned int framebuffer = 0;
//    uint16_t *pixs_buffer = nullptr;
//    uint8_t *pixs_buffer = nullptr;
    uint32_t *pixs_buffer = nullptr;
};


#endif //SHORTVIDEOAPP_XTEXTURE_H
