//
// Created by ArcherV on 2021-02-23.
//

#ifndef SHORTVIDEOAPP_FFMUX_H
#define SHORTVIDEOAPP_FFMUX_H

#include "IMux.h"

struct AVFormatContext;
struct AVOutputFormat;
struct AVStream;

class FFMux : public IMux {
public:
    virtual bool Open(const char *url, XParameter para);

    virtual void Close();

    //获取视频参数
    virtual XParameter GetVPara();

    //获取音频参数
    virtual XParameter GetAPara();

    virtual void Write(XData);

private:
    AVFormatContext *ic = 0;
    AVOutputFormat *fmt = 0;
    AVStream *stream = 0;
    std::mutex mux;
    std::condition_variable condition;
    int audioStream = 1;
    int videoStream = 0;
};


#endif //SHORTVIDEOAPP_FFMUX_H
