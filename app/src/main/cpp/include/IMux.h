//
// Created by ArcherV on 2021-02-23.
//

#ifndef SHORTVIDEOAPP_IMUX_H
#define SHORTVIDEOAPP_IMUX_H

#include <list>
#include "IObserver.h"
#include "XParameter.h"

struct AVPacket;

class IMux : public IObserver {
public:
    virtual void Update(XData data) override;

    //打开文件， 或者流媒体 rtmp http rtsp
    virtual bool Open(const char *url) = 0;

    virtual void Close() = 0;

    virtual void Clear();

    //读取视频参数
    virtual XParameter GetVPara() = 0;

    //获取音频参数
    virtual XParameter GetAPara() = 0;

    virtual void Write(XData) = 0;

protected:
    virtual void Main() override;

    int maxList = 100;
    std::list<XData> packets;
    std::mutex packetsMutex;
    std::condition_variable condition;
};


#endif //SHORTVIDEOAPP_IMUX_H
