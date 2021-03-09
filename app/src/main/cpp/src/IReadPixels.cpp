//
// Created by ArcherV on 2021-03-09.
//

#include <Xlog.h>
#include "IReadPixels.h"

void IReadPixels::Update(XData data) {
    std::lock_guard<std::mutex> lck(mux);
    texId = data.texId;
    pts = data.pts;
    flag = true;
    condition.notify_one();
}

void IReadPixels::Main() {
    while (!isExit) {
        if (IsPause()) {
            XSleep(2);
            continue;
        }
        if (!init) {
            CreateEnv();
            init = true;
        }
        std::unique_lock<std::mutex> lck(mux);
        condition.wait(lck, [this] {
            return flag;
        });
        XData data = ReadPixels(texId);
        data.pts = pts;
        flag = false;
        Notify(data);
    }
}

void IReadPixels::Close() {
    std::unique_lock<std::mutex> lck(mux);
    flag = false;
    init = false;
//    sharedContext = nullptr;
    texId = 0;
}
