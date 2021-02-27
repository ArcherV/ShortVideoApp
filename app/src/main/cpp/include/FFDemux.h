//
// Created by ArcherV on 2021-02-23.
//

#ifndef SHORTVIDEOAPP_FFDEMUX_H
#define SHORTVIDEOAPP_FFDEMUX_H

#include "IDemux.h"
struct AVFormatContext;
struct AVOutputFormat;

class FFDemux : public IDemux {
public:
    FFDemux();

    virtual bool Open(const char *url);

    virtual void Close();

    //获取视频参数
    virtual XParameter GetVPara();

    //获取音频参数
    virtual XParameter GetAPara();

    //读取一帧数据，数据由调用者清理
    virtual XData Read();

    virtual void Write(XData);

private:
    AVFormatContext *ic = 0;
    AVOutputFormat *fmt = 0;
    std::mutex mux;
    int audioStream = 1;
    int videoStream = 0;
};


#endif //SHORTVIDEOAPP_FFDEMUX_H
