//
// Created by admin on 2021-02-20.
//

#include "XEGL.h"
#include "Xlog.h"
#include <mutex>
#include "XUtils.h"

bool XEGL::Init(void *win, EGLContext sharedContext) {
    ANativeWindow *nwin = (ANativeWindow *)win;
    Close();
    std::lock_guard<std::mutex> lck(mux);

    display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

    if (display == EGL_NO_DISPLAY) {
        XLOGE("Init", "eglGetDisplay failed!");
        return false;
    }

    XLOGE("Init", "eglGetDisplay success!");

    //2初始化Display
    if(EGL_TRUE != eglInitialize(display, 0, 0)){
        XLOGE("Init", "eglInitialize failed!");
        return false;
    }
    XLOGE("Init", "eglInitialize success!");

    //3 获取配置并创建surface
    EGLint configSpec [] = {
            EGL_RED_SIZE,8,
            EGL_GREEN_SIZE, 8,
            EGL_BLUE_SIZE, 8,
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_NONE
    };
    EGLint configSpecPbuffer [] = {
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,// very important!
            EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,//EGL_WINDOW_BIT EGL_PBUFFER_BIT we will create a pixelbuffer surface
            EGL_RED_SIZE,8,
            EGL_GREEN_SIZE, 8,
            EGL_BLUE_SIZE, 8,
            EGL_NONE
    };
    EGLint surfaceAttr[] = {
            EGL_WIDTH, WIDTH,
            EGL_HEIGHT, HEIGHT,
            EGL_NONE
    };
    EGLint configSpecHDR [] = {
            EGL_RED_SIZE, 10,
            EGL_GREEN_SIZE, 10,
            EGL_BLUE_SIZE, 10,
            EGL_ALPHA_SIZE, 2,
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_NONE
    };
    EGLConfig config = 0;
    EGLint numConfig = 0;
    if(EGL_TRUE != eglChooseConfig(display, sharedContext == nullptr ? configSpecHDR : configSpecPbuffer, &config, 1, &numConfig)){
        XLOGE("Init", "eglChooseConfig failed!");
        return false;
    }
    EGLint attributes [] = {
            EGL_GL_COLORSPACE_KHR, EGL_GL_COLORSPACE_SCRGB_EXT,
            EGL_NONE
    };
    surface = sharedContext == nullptr ? eglCreateWindowSurface(display, config, nwin, attributes) :
            eglCreatePbufferSurface(display, config, surfaceAttr);

    //4创建并打开EGL上下文
    const EGLint ctxAttr[] = {EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE};
    context = eglCreateContext(display, config, sharedContext == nullptr ? EGL_NO_CONTEXT : sharedContext, ctxAttr);

    if(context == EGL_NO_CONTEXT){
        XLOGE("Init", "eglChooseContext failed!");
        return false;
    }
    XLOGE("Init", "eglChooseContext success!");

    if(EGL_TRUE != eglMakeCurrent(display, surface, surface, context)){
        XLOGE("Init", "eglMakeCurrent failed!");
        return false;
    }

    XLOGE("Init", "eglMakeCurrent success!");
    return true;
}

void XEGL::Draw() {
    std::lock_guard<std::mutex> lck(mux);
    if(display == EGL_NO_DISPLAY || surface == EGL_NO_SURFACE)
        return;
    eglSwapBuffers(display, surface);
}

void XEGL::Close() {
    std::lock_guard<std::mutex> lck(mux);
    if(display == EGL_NO_DISPLAY)
        return;

    eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);

    if(surface != EGL_NO_SURFACE)
        eglDestroySurface(display, surface);
    if(context != EGL_NO_CONTEXT)
        eglDestroyContext(display,context);

    eglTerminate(display);

    display = EGL_NO_DISPLAY;
    surface = EGL_NO_SURFACE;
    context = EGL_NO_CONTEXT;
}

EGLContext XEGL::getSharedContext() {
    return context;
}
