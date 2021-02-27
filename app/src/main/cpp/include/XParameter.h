//
// Created by ArcherV on 2021-02-23.
//

#ifndef SHORTVIDEOAPP_XPARAMETER_H
#define SHORTVIDEOAPP_XPARAMETER_H

struct AVCodecParameters;
struct AVRational;

class XParameter {
public:
    AVCodecParameters *para = 0;

    int channels = 2;
    int sample_rate = 44100;

    AVRational *time_base;
};


#endif //SHORTVIDEOAPP_XPARAMETER_H
