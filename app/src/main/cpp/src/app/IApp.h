//
// Created by ArcherV on 2021-02-24.
//

#ifndef SHORTVIDEOAPP_IAPP_H
#define SHORTVIDEOAPP_IAPP_H


#include "../thread/XThread.h"
#include "../encode/IEncode.h"

class IApp : public XThread {
public:
    static IApp *Get();
    virtual bool Open(const char *path);
    virtual bool Start() override ;
    virtual void InitView(void *win, const char *, const char *, const char *);
    virtual void Close();

    IEncode *vencode = 0;
protected:
    void Main() override;
    IApp() {}
    std::mutex mux;
};


#endif //SHORTVIDEOAPP_IAPP_H
