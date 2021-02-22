//
// Created by ArcherV on 2021-02-22.
//

#ifndef SHORTVIDEOAPP_XSHADER_H
#define SHORTVIDEOAPP_XSHADER_H

#include <mutex>

enum TextureType {
    TYPE_OES = 0,
    TYPE_2D = 1
};

class XShader {
public:
    virtual void Close();
    virtual bool Init(const char *, const char *);

    //获取材质并映射到内存
    virtual void GetTexture(int textureId, int width, int height, unsigned char *buf);
    virtual void Draw(TextureType type);

protected:
    unsigned int vsh = 0;
    unsigned int fsh = 0;
    unsigned int program = 0;
    std::mutex mux;
};


#endif //SHORTVIDEOAPP_XSHADER_H
