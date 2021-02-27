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
            XLOGE("Update", "Packet pts %ld", ((AVPacket *)pkt.data)->pts);
            XLOGE("Update", "pointer %ld", pkt.data);
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
//        XData packet = packets.front();
        XData packet = packets.front();
        XLOGE("Main", "Packet pts %ld", ((AVPacket *)packet.data)->pts);
//        XLOGE("Main", "Packet pointer %ld", packet.data);
//        XLOGE("Main", "Packet xxx %ld", (AVPacket *)packet.data);
        packets.pop_front();

//        Write(packet);
    }
}
