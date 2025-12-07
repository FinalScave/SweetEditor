//
// Created by Scave on 2025/12/7.
//

#ifndef SWEETEDITOR_LOGGING_H
#define SWEETEDITOR_LOGGING_H

#ifdef ENABLE_LOG
  #define LOG_TAG "SWEETEDITOR"
  #if defined(ANDROID)
    #include "android_log.h"
  #elif defined(OHOS)
    #include "ohos_log.h"
  #else
    #include <iostream>
    #include "utility.h"
    #define LOGD(...) std::cout << "[" << LOG_TAG << "] " << __FILE_NAME__ << ":" << __FUNCTION__ << StrUtil::formatString(__VA_ARGS__) << std::endl;
    #define LOGI(...) std::cout << StrUtil::formatString(__VA_ARGS__) << std::endl;
    #define LOGW(...) std::cout << StrUtil::formatString(__VA_ARGS__) << std::endl;
    #define LOGE(...) std::cerr << StrUtil::formatString(__VA_ARGS__) << std::endl;
  #endif
#else
  #define LOGD(...)
  #define LOGI(...)
  #define LOGW(...)
  #define LOGE(...)
#endif

#endif //SWEETEDITOR_LOGGING_H