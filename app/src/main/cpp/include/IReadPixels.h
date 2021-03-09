//
// Created by ArcherV on 2021-03-09.
//

#ifndef SHORTVIDEOAPP_IREADPIXELS_H
#define SHORTVIDEOAPP_IREADPIXELS_H


#include <IObserver.h>
#include <EGL/egl.h>

class IReadPixels : public IObserver {
public:
    virtual bool Open() = 0;

    virtual void Update(XData data) override;

    virtual void Main() override ;

    virtual void CreateEnv() = 0;

    virtual XData ReadPixels(GLuint texture2D) = 0;

    virtual void Clear() = 0;

    virtual void Close();

    virtual void setSharedContext(EGLContext sharedContext) {
        this->sharedContext = sharedContext;
    }
private:
    std::mutex mux;
    std::condition_variable condition;
    int64_t pts;
    bool flag = false;
    bool init = false;
protected:
    EGLContext sharedContext;
    GLuint texId;
};


#endif //SHORTVIDEOAPP_IREADPIXELS_H
