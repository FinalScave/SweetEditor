//
// Created by Scave on 2025/12/7.
//

#ifndef SWEETEDITOR_LOGGING_H
#define SWEETEDITOR_LOGGING_H

#if defined(_MSC_VER) && !defined(__FILE_NAME__)
#define __FILE_NAME__ (strrchr("\\" __FILE__, '\\') + 1)
#endif

#ifdef ENABLE_LOG
  #define LOG_TAG "SWEETEDITOR"
#include "buffer.h"
#if defined(ANDROID)
    #include "android_log.h"
  #elif defined(OHOS)
    #include "ohos_log.h"
  #elif defined(_WIN32) || defined(_WIN64)
    #include <debugapi.h>
    #include "utility.h"
    #define LOG_FMT "[%s][%s] \"%s\"-%s:%d %s"
    #define LOG(level, ...) OutputDebugString(StrUtil::formatString(LOG_FMT, level, LOG_TAG, __FILE_NAME__, __FUNCTION__, __LINE__, StrUtil::formatString(__VA_ARGS__)).c_str())
    #define LOGD(...) LOG("D", __VA_ARGS__)
    #define LOGI(...) LOG("I", __VA_ARGS__)
    #define LOGW(...) LOG("W", __VA_ARGS__)
    #define LOGE(...) LOG("E", __VA_ARGS__)
  #else
    #include <iostream>
    #include "utility.h"
    #define LOG(level, ...) std::cout << "[" << level << "][" << LOG_TAG << "] \"" << __FILE_NAME__ << "\"-" << __FUNCTION__ << ":" << __LINE__ << " " << StrUtil::formatString(__VA_ARGS__) << std::endl;
    #define LOGD(...) LOG("D", __VA_ARGS__)
    #define LOGI(...) LOG("I", __VA_ARGS__)
    #define LOGW(...) LOG("W", __VA_ARGS__)
    #define LOGE(...) LOG("E", __VA_ARGS__)
  #endif
#else
  #define LOGD(...)
  #define LOGI(...)
  #define LOGW(...)
  #define LOGE(...)
#endif

#endif //SWEETEDITOR_LOGGING_H