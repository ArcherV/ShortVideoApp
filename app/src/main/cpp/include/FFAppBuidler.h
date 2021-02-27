//
// Created by ArcherV on 2021-02-23.
//

#ifndef SHORTVIDEOAPP_FFAPPBUIDLER_H
#define SHORTVIDEOAPP_FFAPPBUIDLER_H


#include "IAppBuidler.h"
#include "IApp.h"

class FFAppBuidler : public IAppBuidler {
public:
    static FFAppBuidler *Get() {
        static FFAppBuidler ff;
        return &ff;
    }

protected:
    FFAppBuidler() {};
    virtual IMux *CreateMux();
    virtual IEncode *CreateEncode();
//    virtual IDemux *CreateDemux();
//    virtual IDecode *CreateDecode();
//    virtual IResample *CreateResample();
//    virtual IVideoView *CreateVideoView();
//    virtual IAudioPlay *CreateAudioPlay();
    virtual IApp *CreateApp() override ;
};


#endif //SHORTVIDEOAPP_FFAPPBUIDLER_H
