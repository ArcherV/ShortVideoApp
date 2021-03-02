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
    IEncode *vencode = CreateEncode();
    XTexture::Get()->AddObs(vencode);
    vencode->AddObs(muxer);
    app->vencode = vencode;
    app->muxer = muxer;
//    XLOGE("BuilderApp", "xxxxxxx");
    return app;
}
