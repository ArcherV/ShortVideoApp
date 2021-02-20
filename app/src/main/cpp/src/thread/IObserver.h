//
// Created by admin on 2021-02-20.
//

#ifndef SHORTVIDEOAPP_IOBSERVER_H
#define SHORTVIDEOAPP_IOBSERVER_H

#include <mutex>
#include <vector>
#include "XThread.h"
#include "../utils/XData.h"

class IObserver : public XThread {
public:
    //观察者接收数据函数
    virtual void Update(XData data) = 0;

    //主体函数 添加观察者(线程安全)
    void AddObs(IObserver *obs);

    //通知所有观察者(线程安全)
    void Notify(XData data);

protected:
    std::vector<IObserver *> obss;
    std::mutex mux;
};


#endif //SHORTVIDEOAPP_IOBSERVER_H
