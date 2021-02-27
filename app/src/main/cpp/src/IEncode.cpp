//
// Created by ArcherV on 2021-02-23.
//

#include "IEncode.h"
#include "Xlog.h"

void IEncode::Update(XData frame) {
    if (!isRuning)
        return;
    if (frame.isAudio != isAudio)
        return;
    while (!isExit) {
        std::unique_lock<std::mutex> lck(framesMutex);
        if (frames.size() < maxList) {
            frames.push_back(frame);
            condition.notify_one();
            break;
        }
        XSleep(1);
    }
}

void IEncode::Main() {
    while (!isExit) {
        if (IsPause()) {
            XSleep(2);
            continue;
        }
        std::unique_lock<std::mutex> lck(framesMutex);
        condition.wait(lck, [this] {
            return !frames.empty();
        });
        XData frame = frames.front();
        frames.pop_front();

        if (SendFrame(frame)) {
            while (!isExit) {
                XData packet = RecvPacket();
                if (!packet.data)
                    break;
                XLOGE("Main", "Packet pts %ld", ((AVPacket *)packet.data)->pts);
                XLOGE("Main", "pointer %ld", packet.data);
                Notify(packet);
            }
        }

        frame.Drop();
    }
}

void IEncode::Clear()
{
    while (!frames.empty())
    {
        frames.front().Drop();
        frames.pop_front();
    }
}
