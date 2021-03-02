//
// Created by ArcherV on 2021-02-24.
//

#ifndef SHORTVIDEOAPP_IAPPPROXY_H
#define SHORTVIDEOAPP_IAPPPROXY_H

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include "IApp.h"

class IAppProxy : public IApp {
public:
    static IAppProxy*Get() {
        static IAppProxy px;
        return &px;
    }

    void Init();
    GLuint CreateTextureOES();
    void CreateTexture();
    void Render();
    virtual bool Open(const char *path);
    virtual bool Start();
    virtual void InitView(void *win, const char *, const char *, const char *);
    virtual void Close();
    virtual bool IsPause();

protected:
    IAppProxy(){}
    IApp *app = 0;
    std::mutex mux;
};


#endif //SHORTVIDEOAPP_IAPPPROXY_H
