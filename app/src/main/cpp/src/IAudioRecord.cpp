//
// Created by ArcherV on 2021-03-04.
//

#include "IAudioRecord.h"

void IAudioRecord::Main() {
    while (!isExit) {
        if(IsPause()) {
            XSleep(2);
            continue;
        }
        std::unique_lock<std::mutex> lck(mux);
        condition.wait(lck, [this] {
            return recording;
        });
        XData data = Read();
        Notify(data);
    }
}
