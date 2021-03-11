#include <jni.h>
#include <string>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <android/asset_manager_jni.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include "XEGL.h"
#include "XUtils.h"
#include "Xlog.h"
#include "XTexture.h"
#include "IEncode.h"
#include "FFEncode.h"
#include "IAppProxy.h"

AAssetManager *g_pAssetManager = nullptr;

extern "C"
JNIEXPORT void JNICALL
Java_com_example_shortvideoapp_MainActivity_registerAssetManager(
        JNIEnv *env,
        jobject,
        jobject asset_manager) {
    g_pAssetManager = AAssetManager_fromJava(env, asset_manager);
    vertexShader = readAssetFile("vertex.glsl", g_pAssetManager);
    fragmentOESShader = readAssetFile("fragmentOES.glsl", g_pAssetManager);
    fragmentShader = readAssetFile("fragment2D.glsl", g_pAssetManager);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_shortvideoapp_MainActivity_InitView(JNIEnv *env, jobject, jobject surface) {
    ANativeWindow *win = ANativeWindow_fromSurface(env, surface);
    XTexture::Get()->Init(win);
    XTexture::Get()->DrawOES();
}
