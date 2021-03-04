//
// Created by ArcherV on 2021-03-04.
//

#ifndef SHORTVIDEOAPP_IAUDIORECORD_H
#define SHORTVIDEOAPP_IAUDIORECORD_H


#include "IObserver.h"

class IAudioRecord : public IObserver {
public:
    //获取缓冲数据，如果没有则阻塞
    virtual bool StartRecord() = 0;
    virtual bool StopRecord() = 0;
    virtual XData Read() = 0;
    virtual void Clear() = 0;
    virtual void Close() = 0;
    virtual void Main() override;

    int pts = 0;
protected:
    bool recording = false;
    std::mutex mux;
    std::condition_variable condition;
};



#endif //SHORTVIDEOAPP_IAUDIORECORD_H
