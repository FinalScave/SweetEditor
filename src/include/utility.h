//
// Created by Scave on 2025/12/6.
//

#ifndef SWEETEDITOR_UTILITY_H
#define SWEETEDITOR_UTILITY_H

#include <cstdint>
#include "macro.h"

namespace NS_SWEETEDITOR {
  class TimeUtil {
  public:
    TimeUtil() = delete;
    TimeUtil(const TimeUtil&) = delete;
    TimeUtil& operator=(const TimeUtil&) = delete;

    /// 获取当前毫秒时间戳
    /// @return 毫秒时间戳
    static int64_t milliTime();

    /// 获取当前微秒时间戳
    /// @return 微秒时间戳
    static int64_t microTime();

    /// 获取当前纳秒时间戳
    /// @return 纳秒时间戳
    static int64_t nanoTime();
  };

  class StrUtil {
  public:
    /// 格式化字符串
    /// @param format 格式
    /// @param ... 替换参数
    /// @return 格式化之后的文本
    static U8String formatString(const char* format, ...);
    /// 接受va_list作为参数进行格式化
    /// @param format 格式
    /// @param args 参数
    /// @return 格式化之后的文本
    static U8String vFormatString(const char* format, va_list args);

    /// 将UTF8文本转换为UTF16
    /// @param utf8_str UTF8文本
    /// @param result 结果UTF16
    static void convertUTF8ToUTF16(const U8String& utf8_str, U16String& result);

    /// 将UTF8文本转换为UTF16字符串
    /// @param utf8_str UTF8文本
    /// @param result 结果UTF16
    static void convertUTF8ToUTF16(const U8String& utf8_str, U16Char** result);
  };
}

#endif //SWEETEDITOR_UTILITY_H