//
// Created by ArcherV on 2021-03-04.
//

#ifndef SHORTVIDEOAPP_SLAUDIORECORD_H
#define SHORTVIDEOAPP_SLAUDIORECORD_H


#include "IAudioRecord.h"
extern "C" {
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
};

class SLAudioRecord : public IAudioRecord {
public:
    bool StartRecord() override;

    bool StopRecord() override;

    XData Read() override;

    void queueCallBack();

    void Clear() override;

    void Close() override;

private:
    void openSLInit();
    void openSLDestroy();
    void recorderInit();

    SLObjectItf engineObject;
    SLEngineItf engineInterface;

    SLObjectItf recorderObject;
    SLRecordItf recorderInterface;


    SLAndroidSimpleBufferQueueItf androidBufferQueueItf;

    SLDataLocator_AndroidSimpleBufferQueue queue;
    SLDataFormat_PCM pcmFormat;
    SLDataLocator_IODevice ioDevice;        //io设备描述

    SLDataSource  source;           //音频数据来源的信息
    SLDataSink  sink;               //音频数据输出信息

    u_char *buffer;

    const SLuint32 CHANNELS = 1;
    const int SAMPLE_RATE = 44100;
    const unsigned int PERIOD_TIME = 20;  //周期20ms
    const unsigned int FRAME_SIZE = SAMPLE_RATE * PERIOD_TIME / 1000;
    const unsigned int BUFFER_SIZE = FRAME_SIZE * CHANNELS;
};



#endif //SHORTVIDEOAPP_SLAUDIORECORD_H
