//
// Created by admin on 2021-02-20.
//

#include "IObserver.h"
#include "../utils/XData.h"
#include "../utils/Xlog.h"

void IObserver::AddObs(IObserver *obs) {
    if (!obs) {
        XLOGE("AddObs", "观察者为空！");
        return;
    }
    std::lock_guard<std::mutex> lck(mux);
    obss.push_back(obs);
}

void IObserver::Notify(XData data) {
    std::lock_guard<std::mutex> lck(mux);
    for (auto obs : obss)
        obs->Update(data);
}
