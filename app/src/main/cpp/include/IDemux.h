//
// Created by ArcherV on 2021-02-23.
//

#ifndef SHORTVIDEOAPP_IDEMUX_H
#define SHORTVIDEOAPP_IDEMUX_H


#include "IObserver.h"
#include "XParameter.h"

class IDemux : public IObserver {
public:
    //打开文件， 或者流媒体 rtmp http rtsp
    virtual bool Open(const char *url) = 0;

    virtual void Close() = 0;

    //读取视频参数
    virtual XParameter GetVPara() = 0;

    //获取音频参数
    virtual XParameter GetAPara() = 0;

    //读取一帧数据，数据由调用者清理
    virtual XData Read() = 0;

    virtual void Write(XData) = 0;

    //总时长(毫秒)
    int totalMs = 0;

protected:
    virtual void Main();
};


#endif //SHORTVIDEOAPP_IDEMUX_H
