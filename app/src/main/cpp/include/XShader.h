//
// Created by ArcherV on 2021-02-22.
//

#ifndef SHORTVIDEOAPP_XSHADER_H
#define SHORTVIDEOAPP_XSHADER_H

#include <mutex>
#include <GLES2/gl2.h>

enum TextureType {
    TYPE_OES = 0,
    TYPE_2D = 1
};

class XShader {
public:
    virtual void Close();
    virtual bool Init(bool screen);

    //获取材质并映射到内存
    virtual void GetTexture(int textureId, int width, int height, unsigned char *buf);
    virtual void GetTexture(unsigned int texId);
    virtual void Draw(TextureType type);
protected:
    virtual GLuint CreateProgram(TextureType, bool screen, const char *, const char *);

    unsigned int vsh = 0;
    unsigned int fsh = 0;
    unsigned int program = 0;
    unsigned int program2D = 0;
    unsigned int programOES = 0;
    std::mutex mux;
};


#endif //SHORTVIDEOAPP_XSHADER_H
