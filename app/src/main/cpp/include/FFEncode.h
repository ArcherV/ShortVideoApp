//
// Created by ArcherV on 2021-02-23.
//

#ifndef SHORTVIDEOAPP_FFENCODE_H
#define SHORTVIDEOAPP_FFENCODE_H


#include "IEncode.h"

class FFEncode : public IEncode {
public:
    bool Open() override;

    void Close() override;

    void Clear() override;

    bool SendFrame(XData para) override;

    XData RecvPacket() override;

private:
    int pts = -1;
    AVCodecContext *codec;
    AVFrame *frame;
    AVPacket *packet;
    SwsContext *swsContext;
    SwrContext *swrContext;
    std::mutex mux;
};


#endif //SHORTVIDEOAPP_FFENCODE_H
