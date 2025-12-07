//
// Created by Scave on 2025/12/6.
//
#include <chrono>
#include <cstdarg>
#include "utility.h"

namespace NS_SWEETEDITOR {
  // ======================================== TimeUtil =================================================
  int64_t TimeUtil::milliTime() {
    auto now = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
  }

  int64_t TimeUtil::microTime() {
    auto now = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()).count();
  }

  int64_t TimeUtil::nanoTime() {
    auto now = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count();
  }

  // ======================================== StrUtil =================================================
  U8String StrUtil::vFormatString(const char* format, va_list args) {
    va_list args_copy;
    va_copy(args_copy, args);
    int size = std::vsnprintf(nullptr, 0, format, args_copy);
    va_end(args_copy);

    if (size < 0) return "";
    U8String result(size + 1, '\0');
    std::vsnprintf(result.data(), size + 1, format, args);
    result.resize(size);
    return result;
  }

  U8String StrUtil::formatString(const char* format, ...) {
    va_list args;
    va_start(args, format);
    U8String result = vFormatString(format, args);
    va_end(args);
    return result;
  }
}
