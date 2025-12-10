//
// Created by Scave on 2025/12/6.
//
#include <chrono>
#include <cstdarg>
#include <simdutf/simdutf.h>
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

  void StrUtil::convertUTF8ToUTF16(const U8String& utf8_str, U16String& result) {
    size_t utf16_len = simdutf::utf16_length_from_utf8(utf8_str.c_str(), utf8_str.length());
    result.reserve(utf16_len);
    simdutf::convert_utf8_to_utf16(utf8_str.c_str(), utf8_str.length(), CHAR16_PTR(result.data()));
  }

  void StrUtil::convertUTF8ToUTF16(const U8String& utf8_str, U16Char** result) {
    size_t utf16_len = simdutf::utf16_length_from_utf8(utf8_str.c_str(), utf8_str.length());
    U16Char* u16_chars = new U16Char[utf16_len];
    simdutf::convert_utf8_to_utf16(utf8_str.c_str(), utf8_str.length(), CHAR16_PTR(u16_chars));
    result = &u16_chars;
  }
}
