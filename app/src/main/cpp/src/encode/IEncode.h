//
// Created by ArcherV on 2021-02-23.
//

#ifndef SHORTVIDEOAPP_IENCODE_H
#define SHORTVIDEOAPP_IENCODE_H

#include "../thread/IObserver.h"
#include "../utils/XParameter.h"
#include <list>
extern "C" {
#include <libavutil/avassert.h>
#include <libavutil/channel_layout.h>
#include <libavutil/opt.h>
#include <libavutil/mathematics.h>
#include <libavutil/timestamp.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
};

class IEncode : public IObserver{
public:
    virtual bool Open(XParameter para) = 0;
    virtual void Close() = 0;
    virtual void Clear() = 0;

    virtual bool SendFrame(XData para) = 0;

    //从线程中获取解码结果 再次调用会复用上次空间 线程不安全
    virtual  XData RecvPacket() = 0;

    //由主体notify的数据 阻塞
    virtual  void Update(XData frame);

    bool isAudio = false;
protected:
    virtual void Main();

    int maxList = 100;
    std::list<XData> frames;
    std::mutex framesMutex;
    std::condition_variable condition;
};


#endif //SHORTVIDEOAPP_IENCODE_H
