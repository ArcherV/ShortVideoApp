//
// Created by ArcherV on 2021-02-22.
//

#ifndef SHORTVIDEOAPP_XTEXTURE_H
#define SHORTVIDEOAPP_XTEXTURE_H

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include "XShader.h"

class XTexture {
public:
    static XTexture *Get();
    virtual bool Init(void *win, const char *, const char *, const char *) = 0;
    virtual GLuint CreateTexture() = 0;
    virtual GLuint CreateTextureOES() = 0;
    virtual void Draw(unsigned char *data[], int width, int heigth, TextureType type) = 0;
    virtual void DrawOES() = 0;
    virtual void Drop() = 0;

    virtual ~XTexture(){};

protected:
    XTexture(){};

    unsigned int textureOES = 0;
    unsigned int texture2D = 0;
    unsigned int framebuffer = 0;
};


#endif //SHORTVIDEOAPP_XTEXTURE_H
