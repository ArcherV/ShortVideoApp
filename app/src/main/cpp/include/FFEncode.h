//
// Created by ArcherV on 2021-02-23.
//

#ifndef SHORTVIDEOAPP_FFENCODE_H
#define SHORTVIDEOAPP_FFENCODE_H


#include "IEncode.h"

class FFEncode : public IEncode {
public:
    FFEncode(bool isAudio) {
        this->isAudio = isAudio;
    }

    bool Open() override;

    void Close() override;

    void Clear() override;

    bool SendFrame(XData para) override;

    XParameter GetPara() override;

    XData RecvPacket() override;

private:
    int pts = -1;
    AVCodecContext *codec = nullptr;
    AVFrame *frame = nullptr;
    AVPacket *packet = nullptr;
    SwsContext *swsContext;
    SwrContext *swrContext;
    std::mutex mux;
};


#endif //SHORTVIDEOAPP_FFENCODE_H
