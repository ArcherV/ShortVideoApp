#include <jni.h>
#include <string>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include "opengl/XRenderer.h"

extern "C"
JNIEXPORT void JNICALL
Java_com_example_shortvideoapp_svaView_InitView(JNIEnv *env, jobject thiz, jobject surface) {
    ANativeWindow *win = ANativeWindow_fromSurface(env, surface);
    XRenderer::Get()->SetRender(win);
    XRenderer::Get()->Start();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_shortvideoapp_MainActivity_setColor(JNIEnv *env, jobject thiz, jint c) {
    XData data;
    data.type = c;
    XRenderer::Get()->Update(data);
}