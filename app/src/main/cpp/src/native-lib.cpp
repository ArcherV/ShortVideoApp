#include <jni.h>
#include <string>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <android/asset_manager_jni.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include "opengl/XEGL.h"
#include "utils/XUtils.h"
#include "utils/Xlog.h"
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
Java_com_example_shortvideoapp_MainActivity_InitView(JNIEnv *env, jobject, jobject surface) {
    ANativeWindow *win = ANativeWindow_fromSurface(env, surface);
    const char *vertexShader = readAssetFile("vertex.glsl", g_pAssetManager);
    const char *fragmentOESShader = readAssetFile("fragmentOES.glsl", g_pAssetManager);
    const char *fragmentShader = readAssetFile("fragment2D.glsl", g_pAssetManager);
    XTexture::Get()->Init(win, vertexShader, fragmentOESShader, fragmentShader);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_example_shortvideoapp_MainActivity_CreateTextureOES(JNIEnv *, jobject) {
    XLOGE("CreateTexture", "提前Create了");
    return (jint)XTexture::Get()->CreateTextureOES();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_shortvideoapp_MainActivity_render(JNIEnv *, jobject) {
    XTexture::Get()->DrawOES();
}