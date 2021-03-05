//
// Created by ArcherV on 2021-03-04.
//

#include "Xlog.h"
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
        recording = false;
        XData data = Read();
//        XLOGE("IAudioRecord::Main", "Size %04d Byte %d", data.size, data.data[0]);
        Notify(data);
    }
}
