//
// Created by ArcherV on 2021-03-04.
//

#include "Xlog.h"
#include "SLAudioRecord.h"

void openSLCallBack(SLAndroidSimpleBufferQueueItf, void *t) {
    ((SLAudioRecord *) t)->queueCallBack();
}

void SLAudioRecord::queueCallBack() {
    std::lock_guard<std::mutex> lck(mux);
    recording = true;
    condition.notify_one();
}

bool SLAudioRecord::StartRecord() {
    XLOGE("SLAudioRecord::StartRecord", "Start recording");
    num = 0;
    buffer = new u_char[BUFFER_SIZE * sizeof(short)];
    openSLInit();
    recorderInit();
    return true;
}

XData SLAudioRecord::Read() {
    (*androidBufferQueueItf)->Enqueue(androidBufferQueueItf, buffer,
                                      BUFFER_SIZE * sizeof(short));

    XData data;
    data.Alloc(BUFFER_SIZE * sizeof(short), buffer);
    data.isAudio = true;
    data.pts = ++num;
    return data;
}

void SLAudioRecord::Close() {
    openSLDestroy();
}

bool SLAudioRecord::StopRecord() {
    std::lock_guard<std::mutex> lck(mux);
    recording = false;
}

void SLAudioRecord::openSLInit() {
    engineInterface = nullptr;
    engineObject = nullptr;
    recorderInterface = nullptr;
    recorderObject = nullptr;

    slCreateEngine(&engineObject, 0, nullptr, 0, nullptr, nullptr);

    (*engineObject)->Realize(engineObject, SL_BOOLEAN_FALSE);

    (*engineObject)->GetInterface(engineObject, SL_IID_ENGINE, &engineInterface);
}

void SLAudioRecord::openSLDestroy() {
    // TODO：待确定是否正确Stop的方式
    if (recorderObject) {
        (*recorderObject)->Destroy(recorderObject);
        recorderObject = nullptr;
        recorderInterface = nullptr;
    }

    androidBufferQueueItf = nullptr;

    if (engineObject) {
        (*engineObject)->Destroy(engineObject);
        engineObject = nullptr;
        engineInterface = nullptr;
    }
}

void SLAudioRecord::recorderInit() {
    //录音源source
    ioDevice.locatorType = SL_DATALOCATOR_IODEVICE;       //源头是io设备
    ioDevice.deviceType = SL_IODEVICE_AUDIOINPUT;         //音频输入
    ioDevice.deviceID = SL_DEFAULTDEVICEID_AUDIOINPUT;
    ioDevice.device = nullptr;           //device ID生效的前提必须device为NULL

    source.pLocator = &ioDevice;
    source.pFormat = nullptr;        //已经设置了locator忽略此参数

    //输出
    queue.locatorType = SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE;  //输出到缓冲队列
    queue.numBuffers = 2;     //2个缓冲队列

    pcmFormat.formatType = SL_DATAFORMAT_PCM;     //录音数据格式
    pcmFormat.numChannels = CHANNELS;
    pcmFormat.samplesPerSec = SL_SAMPLINGRATE_44_1;
    pcmFormat.bitsPerSample = SL_PCMSAMPLEFORMAT_FIXED_16;        //每个采样点bit
    pcmFormat.containerSize = SL_PCMSAMPLEFORMAT_FIXED_16;
    pcmFormat.channelMask = SL_SPEAKER_FRONT_CENTER;             //根据声道数确定掩码
    pcmFormat.endianness = SL_BYTEORDER_LITTLEENDIAN;             //字节小端模式

    sink.pLocator = &queue;
    sink.pFormat = &pcmFormat;

    ///Recorder/////
    SLInterfaceID id[] = {SL_IID_ANDROIDSIMPLEBUFFERQUEUE};
    SLboolean required[] = {SL_BOOLEAN_TRUE};
    int re;

    (*engineInterface)->CreateAudioRecorder(engineInterface, &recorderObject, &source, &sink, 1, id,
                                            required);
    (*recorderObject)->Realize(recorderObject, SL_BOOLEAN_FALSE);

    //Register
    (*recorderObject)->GetInterface(recorderObject, SL_IID_ANDROIDSIMPLEBUFFERQUEUE,
                                    &androidBufferQueueItf);
    (*androidBufferQueueItf)->RegisterCallback(androidBufferQueueItf, openSLCallBack, this);

    //start recorder
    (*recorderObject)->GetInterface(recorderObject, SL_IID_RECORD, &recorderInterface);
    re = (*recorderInterface)->SetRecordState(recorderInterface, SL_RECORDSTATE_RECORDING);

    if (re != SL_RESULT_SUCCESS) {
        XLOGE("SLAudioRecord::recorderInit", "SetRecordState failed!");
    }
    //6 在设置完录制状态后一定需要先Enqueue一次，这样的话才会开始采集回调
    re = (*androidBufferQueueItf)->Enqueue(androidBufferQueueItf, buffer,
                                           BUFFER_SIZE * sizeof(short));
    if (re != SL_RESULT_SUCCESS) {
        XLOGE("SLAudioRecord::recorderInit", "Enqueue failed!");
    }
}

void SLAudioRecord::Clear() {
    std::lock_guard<std::mutex> lck(mux);
    delete buffer;
    buffer = nullptr;
}
