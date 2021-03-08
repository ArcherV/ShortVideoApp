//
// Created by ArcherV on 2021-02-24.
//

#ifndef SHORTVIDEOAPP_IAPP_H
#define SHORTVIDEOAPP_IAPP_H


#include "XThread.h"
#include "IEncode.h"
#include "IMux.h"
#include "IAudioRecord.h"

class IApp : public XThread {
public:
    static IApp *Get();
    virtual bool Open(const char *path);
    virtual bool Start() override ;
    virtual void InitView(void *win, const char *, const char *, const char *);
    virtual void Close();

    IAudioRecord *audioRecord = nullptr;
    IEncode *aencode = nullptr;
    IEncode *vencode = nullptr;
    IMux *muxer = nullptr;
protected:
    void Main() override;
    IApp() {}
    std::mutex mux;
};


#endif //SHORTVIDEOAPP_IAPP_H
