//
// Created by ArcherV on 2021-02-23.
//

#include "IAppBuidler.h"
#include "../opengl/XTexture.h"
#include "../app/IApp.h"
#include "../utils/Xlog.h"

IApp *IAppBuidler::BuilderApp() {
    IApp *app = CreateApp();
    IEncode *vencode = CreateEncode();
    XTexture::Get()->AddObs(vencode);
    app->vencode = vencode;
//    XLOGE("BuilderApp", "xxxxxxx");
    return app;
}
