//
// Created by ArcherV on 2021-02-23.
//

#ifndef SHORTVIDEOAPP_IAPPBUIDLER_H
#define SHORTVIDEOAPP_IAPPBUIDLER_H


#include "IDemux.h"
#include "IEncode.h"
#include "IApp.h"
#include "IMux.h"
#include "IReadPixels.h"

class IAppBuidler {
public:
    virtual IApp *BuilderApp();

protected:
//    virtual IDemux *CreateDemux() = 0;
    virtual IMux *CreateMux() = 0;
    virtual void CreateEnv() = 0;
    virtual IReadPixels *CreateReadPixels() = 0;
    virtual IAudioRecord *CreateAudioRecord() = 0;
//    virtual IDecode *CreateDecode() = 0;
    virtual IEncode *CreateEncode(bool isAudio) = 0;
//    virtual IResample *CreateResample() = 0;
//    virtual IVideoView *CreateVideoView() = 0;
//    virtual IAudioPlay *CreateAudioPlay() = 0;
    virtual IApp *CreateApp() = 0;
};


#endif //SHORTVIDEOAPP_IAPPBUIDLER_H
