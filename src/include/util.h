#ifndef SWEETEDITOR_UTIL_H
#define SWEETEDITOR_UTIL_H

#include <cstdint>
#include "macro.h"

namespace NS_SWEETEDITOR {
  /// UTF8字符串处理工具
  class Utf8Util {
  public:
    Utf8Util() = delete;
    Utf8Util(const Utf8Util&) = delete;
    Utf8Util& operator=(const Utf8Util&) = delete;

    /// 计算UTF-8字符串中的字符数
    /// @param str UTF8文本
    static size_t countChars(const String& str);

    /// 将字符位置转换为字节位置
    /// @param str UTF8文本
    /// @param char_pos 字符位置
    static size_t charPosToBytePos(const String& str, size_t char_pos);

    /// 将字节位置转换为字符位置
    /// @param str UTF8文本
    /// @param byte_pos 字节位置
    static size_t bytePosToCharPos(const String& str, size_t byte_pos);

    /// 获取UTF-8子字符串（按字符计数）
    /// @param str UTF8文本
    /// @param start_char 字符起始位置
    /// @param char_count 字符数量
    /// @return 截取的子串
    static String utf8Substr(const String& str, size_t start_char, size_t char_count);

    /// 检查UTF-8字符串是否有效
    /// @param str UTF8文本
    static bool isValidUTF8(const String& str);
  };

  /// 字符串处理工具
  class StrUtil {
  public:
    StrUtil() = delete;
    StrUtil(const StrUtil&) = delete;
    StrUtil& operator=(const StrUtil&) = delete;

    /// String转换为WString
    /// \param s String内容
    /// \return 转换后的宽文本
    static std::wstring toWString(const std::string& s);

    /// WString转换为String
    /// \param ws WString内容
    /// \return 转换后的UTF8文本
    static std::string toString(const std::wstring& ws);

    /// 将宽文本转换为C风格字符串（内存安全）
    /// @param ws 宽文本
    static UPtr<const char[]> toCString(const std::wstring& ws);

    /// 接受va_list作为参数进行格式化
    /// @param format 格式
    /// @param args 参数
    /// @return 格式化之后的文本
    static String vFormatString(const char* format, va_list args);

    /// 格式化字符串
    /// \param format 格式
    /// \param ... 替换参数
    /// \return 格式化之后的文本
    static String formatString(const char* format, ...);

    /// 去首尾空
    /// \param str 要去除首尾空的文本
    static String trim(const String& str);

    /// 替换文本中指定的第一个文本为另一个文本
    /// @param str 原始文本
    /// @param from 要替换的文本
    /// @param to 替换后文本
    /// @return 替换成功返回true
    static bool replaceFirst(String& str, const String& from, const String& to);

    /// 替换文本中指定文本为另一个文本（全部替换）
    /// @param source 原始文本
    /// @param from 要替换的文本
    /// @param to 替换后文本
    /// @return 替换成功返回true
    static String replaceAll(const String& source, const String& from, const String& to);

    /// 判断文本开头是否为指定文本
    /// @param str 文本
    /// @param prefix 前缀
    static bool startsWith(const String& str, const String& prefix);

    /// 判断文本是否包含指定内容
    /// @param str 文本
    /// @param partial 包含的内容
    static bool contains(const String& str, const String& partial);

    /// GBK编码文本转UTF8编码
    /// @param str GBK文本
    /// @return UTF8文本
    static String convertGBKToUTF8(const String& str);

    /// UTF8编码文本转GBK编码
    /// @param str UTF8文本
    /// @return GBK文本
    static String convertUTF8ToGBK(const String& str);
  };

  /// 文件操作工具类
  class FileUtil {
  public:
    FileUtil() = delete;
    FileUtil(const FileUtil&) = delete;
    FileUtil& operator=(const FileUtil&) = delete;

    /// 获取指定路径文件的名称
    /// @param path 文件路径
    /// @return 文件名称（包含后缀名）
    static String getPathName(const String& path);

    /// 获取指定路径文件的扩展名
    /// @param path 文件路径
    /// @return 文件扩展名
    static String getExtension(const String& path);

    /// 获取指定路径的所处目录
    /// @param path 文件路径
    /// @return 文件目录路径
    static String getParentPath(const String& path);

    /// 判断指定路径文件是否存在
    /// @param path 文件路径
    /// @return 如果文件存在返回true
    static bool exists(const String& path);

    /// 创建文件夹(会递归创建父目录)
    /// @param path 文件夹路径
    /// @return 创建成功返回true
    static bool mkdirs(const String& path);

    /// 创建文件夹(不递归创建父目录)
    /// @param path 文件夹路径
    /// @return 创建成功返回true
    static bool mkdir(const String& path);

    /// 判断指定路径是否为文件，不是文件夹
    /// @param path 文件路径
    /// @return 如果文件存在并且不是文件夹返回true
    static bool isFile(const String& path);

    /// 判断指定路径是否为文件夹，不是文件
    /// @param path 文件路径
    /// @return 如果文件夹存在并且不是文件返回true
    static bool isDirectory(const String& path);

    /// 读取指定文件的内容
    /// @param path 文件路径
    static String readString(const String& path);

    /// 向指定文件路径写入内容
    /// @param path 文件路径
    /// @param text 文本内容
    /// @return 写入成功返回true
    static bool writeString(const String& path, const String& text);
  };
}

#endif //SWEETEDITOR_UTIL_H