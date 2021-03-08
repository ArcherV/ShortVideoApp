//
// Created by ArcherV on 2021-02-23.
//

#include "IAppBuidler.h"
#include "XTexture.h"
#include "IApp.h"
#include "Xlog.h"

IApp *IAppBuidler::BuilderApp() {
    IApp *app = CreateApp();
    CreateEnv();
    IMux *muxer = CreateMux();
    IAudioRecord *audioRecord = CreateAudioRecord();
    IEncode *vencode = CreateEncode(false);
    IEncode *aencode = CreateEncode(true);
    XTexture::Get()->AddObs(vencode);
    audioRecord->AddObs(aencode);
    aencode->AddObs(muxer);
    vencode->AddObs(muxer);
    app->audioRecord = audioRecord;
    app->aencode = aencode;
    app->vencode = vencode;
    app->muxer = muxer;
//    XLOGE("BuilderApp", "xxxxxxx");
    return app;
}
