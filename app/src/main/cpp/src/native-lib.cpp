#include <jni.h>
#include <string>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <android/asset_manager_jni.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include "opengl/XEGL.h"
#include "utils/XUtils.h"
#include "opengl/XTexture.h"

AAssetManager *g_pAssetManager = nullptr;

extern "C"
JNIEXPORT void JNICALL
Java_com_example_shortvideoapp_MainActivity_registerAssetManager(
        JNIEnv *env,
        jobject,
        jobject asset_manager) {
    g_pAssetManager = AAssetManager_fromJava(env, asset_manager);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_shortvideoapp_svaView_InitView(JNIEnv *env, jobject, jobject surface) {
    ANativeWindow *win = ANativeWindow_fromSurface(env, surface);
    const char *vertexShader = readAssetFile("vertex.glsl", g_pAssetManager);
    const char *fragmentShader = readAssetFile("fragmentOES.glsl", g_pAssetManager);
    XTexture::Get()->Init(win, vertexShader, fragmentShader);
    glClearColor(1, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    XEGL::Get()->Draw();
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_example_shortvideoapp_MainActivity_CreateTexture(JNIEnv *, jobject) {
    return (jint)XTexture::Get()->CreateTexture();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_shortvideoapp_MainActivity_render(JNIEnv *, jobject) {

}