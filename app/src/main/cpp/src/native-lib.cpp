#include <jni.h>
#include <string>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <android/asset_manager_jni.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
extern "C" {
#include <libavcodec/jni.h>
}
#include "opengl/XEGL.h"
#include "utils/XUtils.h"
#include "utils/Xlog.h"
#include "opengl/XTexture.h"
#include "encode/IEncode.h"
#include "encode/FFEncode.h"
#include "app/IAppProxy.h"

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
JNIEXPORT
jint JNI_OnLoad(JavaVM *, void *) {
    IAppProxy::Get()->Init();
    return JNI_VERSION_1_6;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_shortvideoapp_MainActivity_InitView(JNIEnv *env, jobject, jobject surface) {
    ANativeWindow *win = ANativeWindow_fromSurface(env, surface);
    const char *vertexShader = readAssetFile("vertex.glsl", g_pAssetManager);
    const char *fragmentOESShader = readAssetFile("fragmentOES.glsl", g_pAssetManager);
    const char *fragmentShader = readAssetFile("fragment2D.glsl", g_pAssetManager);
    IAppProxy::Get()->InitView(win, vertexShader, fragmentOESShader, fragmentShader);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_example_shortvideoapp_MainActivity_CreateTextureOES(JNIEnv *, jobject) {
    XLOGE("CreateTexture", "提前Create了");
    return (jint)IAppProxy::Get()->CreateTextureOES();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_shortvideoapp_MainActivity_render(JNIEnv *, jobject) {
    IAppProxy::Get()->Render();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_shortvideoapp_MainActivity_startRecord(JNIEnv *, jobject) {
    IAppProxy::Get()->Open("/sdcard/out.mp4");
    IAppProxy::Get()->Start();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_shortvideoapp_MainActivity_stopRecord(JNIEnv *env, jobject thiz) {
    IAppProxy::Get()->Close();
}