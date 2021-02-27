//
// Created by ArcherV on 2021-02-23.
//

#include "IDemux.h"

void IDemux::Main()
{
    while (!isExit) {
        if(IsPause()) {
            XSleep(2);
            continue;
        }

        XData d = Read();
        if (d.size > 0) {
            Notify(d);
        } else {
            XSleep(2);
        }
    }
}