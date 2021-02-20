//
// Created by admin on 2021-02-20.
//

#ifndef SHORTVIDEOAPP_XLOG_H
#define SHORTVIDEOAPP_XLOG_H

#include <android/log.h>

#define XLOGE(...) __android_log_print(ANDROID_LOG_ERROR, __VA_ARGS__)

#endif //SHORTVIDEOAPP_XLOG_H
