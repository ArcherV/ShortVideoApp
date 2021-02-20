//
// Created by admin on 2021-02-21.
//

#include "XRenderer.h"
#include "XEGL.h"
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

void XRenderer::SetRender(void *win) {
    view = win;
}

XRenderer *XRenderer::Get() {
    static XRenderer xRenderer;
    return &xRenderer;
}

void XRenderer::Update(XData data) {
    std::lock_guard<std::mutex> lck(mux);
    render = true;
    condition.notify_all();
}

void XRenderer::Main() {
    while (!isExit) {
        if (IsPause()) {
            XSleep(2);
            continue;
        }
        if (!initEGL) {
            initEGL = true;
            XEGL::Get()->Init(view);
            //材质初始化
            glGenTextures(1, &texture);

            //设置纹理属性
            glBindTexture(GL_TEXTURE_2D, texture);
            //缩小的过滤器
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            //设置纹理的格式和大小
            glTexImage2D(GL_TEXTURE_2D,
                         0,             //细节基本 0默认
                         GL_LUMINANCE,  //gpu内部格式 亮度 灰度图
                         width,         //l拉升到全屏
                         height,        //边框
                         0,             //数据的像素格式 亮度 灰度图 要与上面一致
                         GL_LUMINANCE,  //像素的数据类型
                         GL_UNSIGNED_BYTE,  //纹理的数据
                         NULL);
        }
        std::unique_lock<std::mutex> lck(mux);
        condition.wait(lck, [this] {return render;});
        render = false;

        // TODO: 绘制过程

        //

        XEGL::Get()->Draw();
    }
}
