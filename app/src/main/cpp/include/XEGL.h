//
// Created by admin on 2021-02-20.
//

#ifndef SHORTVIDEOAPP_XEGL_H
#define SHORTVIDEOAPP_XEGL_H


#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <mutex>

class XEGL {
public:
    virtual bool Init(void *win, EGLContext sharedContext);
    virtual void Draw();
    virtual void Close();
    virtual EGLContext getSharedContext();
protected:
private:
    EGLDisplay display = EGL_NO_DISPLAY;
    EGLSurface surface = EGL_NO_SURFACE;
    EGLContext context = EGL_NO_CONTEXT;
    std::mutex mux;
};


#endif //SHORTVIDEOAPP_XEGL_H
