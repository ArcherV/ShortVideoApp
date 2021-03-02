//
// Created by ArcherV on 2021-02-23.
//
extern "C" {
#include <libavcodec/avcodec.h>
}
#include "IMux.h"
#include "Xlog.h"

void IMux::Update(XData pkt) {
    if (!isRuning)
        return;
    while (!isExit) {
        std::unique_lock<std::mutex> lck(packetsMutex);
        if (packets.size() < maxList) {
            packets.push_back(pkt);
            condition.notify_one();
            break;
        }
        XSleep(1);
    }
}

void IMux::Clear()
{
    while (!packets.empty()) {
//        packets.front().Drop();
        packets.pop_front();
    }
}

void IMux::Main()
{
    while (!isExit) {
        if(IsPause()) {
            XSleep(2);
            continue;
        }
        std::unique_lock<std::mutex> lck(packetsMutex);
        condition.wait(lck, [this] {
            return !packets.empty();
        });
        XData packet = packets.front();
        packets.pop_front();

        Write(packet);
    }
}
