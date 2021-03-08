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
    virtual bool Open(const char *url);

    virtual bool AddStream(bool, XParameter);

    virtual void Close();

    virtual void Write(XData);

private:
    AVFormatContext *ic = nullptr;
    AVOutputFormat *fmt = nullptr;
    AVStream *audioStream = nullptr;
    AVStream *videoStream = nullptr;
    std::mutex mux;
//    std::condition_variable condition;
    const int AUDIO_STREAM = 1;
    const int VIDEO_STREAM = 0;
    int nb_streams = 0;
};


#endif //SHORTVIDEOAPP_FFMUX_H
