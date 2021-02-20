//
// Created by admin on 2021-02-21.
//

#ifndef SHORTVIDEOAPP_XRENDERER_H
#define SHORTVIDEOAPP_XRENDERER_H

#include "../thread/IObserver.h"

class XRenderer : public IObserver {
public:
    void SetRender(void *win);
    void Update(XData data) override;
    void Main() override;
    static XRenderer *Get();

private:
    XRenderer(){}

    bool initEGL = false;
    bool render = false;
    void *view = 0;
    unsigned int texture;
    std::mutex mux;
    std::condition_variable condition;

    int width = 720;
    int height = 1280;
};


#endif //SHORTVIDEOAPP_XRENDERER_H
