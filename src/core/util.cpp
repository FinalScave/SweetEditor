#include <filesystem>
#include <vector>
#include <utf8/utf8.h>
#include <codecvt>
#ifdef _WIN32
#include <windows.h>
#else
#include <iconv.h>
#endif
#include <fstream>
#include "util.h"

#ifdef _WIN32
std::string windowsGBKToUTF8(const std::string& gbk_str) {
  if (gbk_str.empty()) {
    return "";
  }
  // GBK转Unicode
  int unicode_len = MultiByteToWideChar(CP_ACP, 0,
    gbk_str.c_str(), -1, NULL, 0);
  wchar_t* unicode_str = new wchar_t[unicode_len];
  MultiByteToWideChar(CP_ACP, 0,
    gbk_str.c_str(), -1, unicode_str, unicode_len);
  // Unicode转UTF-8
  int utf8_len = WideCharToMultiByte(CP_UTF8, 0,
    unicode_str, -1, NULL, 0, NULL, NULL);
  char* utf8_str = new char[utf8_len];
  WideCharToMultiByte(CP_UTF8, 0,
    unicode_str, -1, utf8_str, utf8_len, NULL, NULL);
  std::string result(utf8_str);
  delete[] unicode_str;
  delete[] utf8_str;
  return result;
}

std::string windowsUTF8ToGBK(const std::string& utf8_str) {
  if (utf8_str.empty()) {
    return "";
  }
  // UTF-8 转 Unicode (UTF-16)
  int wlen = MultiByteToWideChar(CP_UTF8, 0, utf8_str.c_str(), -1, nullptr, 0);
  if (wlen == 0) return "";
  std::vector<wchar_t> wbuf(wlen);
  MultiByteToWideChar(CP_UTF8, 0, utf8_str.c_str(), -1, wbuf.data(), wlen);
  // Unicode (UTF-16) 转 GBK
  int gbk_len = WideCharToMultiByte(CP_ACP, 0, wbuf.data(), -1, nullptr, 0, nullptr, nullptr);
  if (gbk_len == 0) return "";
  std::vector<char> gbk_buf(gbk_len);
  WideCharToMultiByte(CP_ACP, 0, wbuf.data(), -1, gbk_buf.data(), gbk_len, nullptr, nullptr);
  return std::string(gbk_buf.data());
}
#elif !defined(ANDROID)
std::string convertEncoding(const std::string& input,
                           const std::string& in_encoding,
                           const std::string& out_encoding) {
  iconv_t cd = iconv_open(out_encoding.c_str(), in_encoding.c_str());
  if (cd == (iconv_t)-1) {
    throw std::runtime_error("iconv_open failed");
  }

  size_t in_bytes = input.size();
  size_t out_bytes = in_bytes * 4; // 预留足够空间
  std::string output(out_bytes, '\0');

  char* in_ptr = const_cast<char*>(input.data());
  char* out_ptr = const_cast<char*>(output.data());

  if (iconv(cd, &in_ptr, &in_bytes, &out_ptr, &out_bytes) == (size_t)-1) {
    iconv_close(cd);
    throw std::runtime_error("iconv conversion failed");
  }

  iconv_close(cd);
  output.resize(output.size() - out_bytes); // 调整到实际大小
  return output;
}
#endif

namespace NS_SWEETEDITOR {
  // ===================================== Utf8Util ============================================
  size_t Utf8Util::countChars(const String& str) {
    return utf8::distance(str.begin(), str.end());
  }
  
  size_t Utf8Util::charPosToBytePos(const String& str, size_t char_pos) {
    if (char_pos == 0) return 0;

    auto it = str.begin();
    for (size_t i = 0; i < char_pos && it != str.end(); ++i) {
      utf8::next(it, str.end());
    }
    return it - str.begin();
  }
  
  size_t Utf8Util::bytePosToCharPos(const String& str, size_t byte_pos) {
    if (byte_pos == 0) return 0;

    size_t char_count = 0;
    auto it = str.begin();
    while (it != str.end() && (it - str.begin()) < static_cast<ptrdiff_t>(byte_pos)) {
      utf8::next(it, str.end());
      char_count++;
    }
    return char_count;
  }
  
  String Utf8Util::utf8Substr(const String& str, size_t start_char, size_t char_count) {
    auto start_it = str.begin();
    auto end_it = str.begin();

    // 移动到起始字符
    for (size_t i = 0; i < start_char && start_it != str.end(); ++i) {
      utf8::next(start_it, str.end());
    }

    // 移动到结束字符
    end_it = start_it;
    for (size_t i = 0; i < char_count && end_it != str.end(); ++i) {
      utf8::next(end_it, str.end());
    }

    return {start_it, end_it};
  }
  
  bool Utf8Util::isValidUTF8(const String& str) {
    return utf8::is_valid(str.begin(), str.end());
  }

  // ======================================== StrUtil =================================================
  std::wstring StrUtil::toWString(const std::string& s) {
#ifdef _WIN32
    if (s.empty()) {
      return {};
    }
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &s[0],
      static_cast<int>(s.size()),nullptr, 0);
    std::wstring wstr(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, &s[0],
      static_cast<int>(s.size()), &wstr[0], size_needed);
    return wstr;
#else
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    return converter.from_bytes(s);
#endif
  }

  std::string StrUtil::toString(const std::wstring& ws) {
#ifdef _WIN32
    if (ws.empty()) {
      return {};
    }
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &ws[0],
      static_cast<int>(ws.size()), nullptr, 0, nullptr, nullptr);
    std::string str(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, &ws[0],
      static_cast<int>(ws.size()), str.data(), size_needed, nullptr, nullptr);
    return str;
#else
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    return converter.to_bytes(ws);
#endif
  }

  UPtr<const char[]> StrUtil::toCString(const std::wstring& ws) {
#ifdef _WIN32
    const int size_needed = WideCharToMultiByte(CP_UTF8, 0, ws.c_str(),
      static_cast<int>(ws.size()), nullptr, 0, nullptr, nullptr);
    UPtr<char[]> ptr = makeUPtr<char[]>(size_needed + 1);
    WideCharToMultiByte(CP_UTF8, 0, ws.c_str(), static_cast<int>(ws.size()),
      ptr.get(), size_needed, nullptr,nullptr);
    ptr[size_needed] = '\0';
    return ptr;
#else
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    std::string utf8_str = converter.to_bytes(ws.c_str());
    UPtr<char[]> ptr = MAKE_UPTR<char[]>(utf8_str.size() + 1);
    std::copy(utf8_str.begin(), utf8_str.end(), ptr.get());
    ptr[utf8_str.size()] = '\0';
    return ptr;
#endif
  }

  String StrUtil::vFormatString(const char* format, va_list args) {
    va_list args_copy;
    va_copy(args_copy, args); // 复制 va_list（因为 args 可能被多次使用）
    int size = std::vsnprintf(nullptr, 0, format, args_copy);
    va_end(args_copy);

    if (size < 0) return "";
    String result(size + 1, '\0');
    std::vsnprintf(result.data(), size + 1, format, args);
    result.resize(size); // 移除末尾的 '\0'
    return result;
  }

  String StrUtil::formatString(const char* format, ...) {
    va_list args;
    va_start(args, format);
    String result = vFormatString(format, args);
    va_end(args);
    return result;
  }

  String StrUtil::trim(const String& str) {
    size_t first = str.find_first_not_of(" \t");
    if (first == String::npos) {
      return "";
    }
    size_t last = str.find_last_not_of(" \t");
    return str.substr(first, (last - first + 1));
  }

  bool StrUtil::replaceFirst(String& str, const String& from, const String& to) {
    size_t start_pos = str.find(from);
    if (start_pos == String::npos) {
      return false;
    }
    str.replace(start_pos, from.length(), to);
    return true;
  }

  String StrUtil::replaceAll(const String& source, const String& from, const String& to) {
    if (from.empty()) {
      return source;
    }
    std::vector<size_t> positions;
    size_t pos = 0;

    // 收集所有匹配位置
    while ((pos = source.find(from, pos)) != String::npos) {
      positions.push_back(pos);
      pos += from.length();
    }

    if (positions.empty()) return source;

    // 计算结果字符串长度
    size_t result_length = source.length() +
                          positions.size() * (to.length() - from.length());

    String result;
    result.reserve(result_length);

    size_t last_pos = 0;
    for (size_t p : positions) {
      result.append(source, last_pos, p - last_pos);
      result.append(to);
      last_pos = p + from.length();
    }
    result.append(source, last_pos, source.length() - last_pos);

    return result;
  }

  bool StrUtil::startsWith(const String& str, const String& prefix) {
    return str.find(prefix) == 0;
  }

  bool StrUtil::contains(const String& str, const String& partial) {
    return str.find(partial) != String::npos;
  }

  String StrUtil::convertGBKToUTF8(const String& str) {
#ifdef _WIN32
    return windowsGBKToUTF8(str);
#elif !defined(ANDROID)
    return convertEncoding(str, "GBK", "UTF-8");
#else
    return str;
#endif
  }

  String StrUtil::convertUTF8ToGBK(const String& str) {
#ifdef _WIN32
    return windowsUTF8ToGBK(str);
#elif !defined(ANDROID)
    return convertEncoding(str, "UTF-8", "GBK");
#else
    return str;
#endif
  }

  // ======================================== FileUtil =================================================
#ifdef _WIN32
  constexpr static char kPathSeparator = '\\';
#else
  constexpr static char kPathSeparator = '/';
#endif

  namespace fs = std::filesystem;
  String FileUtil::getPathName(const String& path) {
    const size_t index = path.find_last_of(kPathSeparator);
    if (index == String::npos) {
      return path;
    }
    return path.substr(index + 1);
  }

  String FileUtil::getExtension(const String& path) {
    try {
#ifdef _WIN32
      fs::path fs_path = fs::u8path(path);
#else
      fs::path fs_path(path);
#endif
      return fs_path.extension().string();
    } catch (const fs::filesystem_error& ex) {
      return path;
    }
  }

  String FileUtil::getParentPath(const String& path) {
    try {
#ifdef _WIN32
      fs::path fs_path = fs::u8path(path);
      return fs_path.parent_path().u8string();
#else
      fs::path fs_path(path);
      return fs_path.parent_path().string();
#endif
    } catch (const fs::filesystem_error& ex) {
      return path;
    }
  }

  bool FileUtil::exists(const String& path) {
    try {
#ifdef _WIN32
      fs::path fs_path = fs::u8path(path);
#else
      fs::path fs_path(path);
#endif
      return fs::exists(fs_path);
    } catch (const fs::filesystem_error& ex) {
      return false;
    }
  }

  bool FileUtil::mkdirs(const String& path) {
    try {
#ifdef _WIN32
      fs::path fs_path = fs::u8path(path);
#else
      fs::path fs_path(path);
#endif
      return fs::create_directories(fs_path);
    } catch (const fs::filesystem_error& ex) {
      return false;
    }
  }

  bool FileUtil::mkdir(const String& path) {
    try {
#ifdef _WIN32
      fs::path fs_path = fs::u8path(path);
#else
      fs::path fs_path(path);
#endif
      return fs::create_directory(fs_path);
    } catch (const fs::filesystem_error& ex) {
      return false;
    }
  }

  bool FileUtil::isFile(const String& path) {
    try {
#ifdef _WIN32
      fs::path fs_path = fs::u8path(path);
#else
      fs::path fs_path(path);
#endif
      return fs::exists(fs_path) && fs::is_regular_file(fs_path);
    } catch (const fs::filesystem_error& ex) {
      return false;
    }
  }

  bool FileUtil::isDirectory(const String& path) {
    try {
#ifdef _WIN32
      fs::path fs_path = fs::u8path(path);
#else
      fs::path fs_path(path);
#endif
      return fs::exists(fs_path) && fs::is_directory(fs_path);
    } catch (const fs::filesystem_error& ex) {
      return false;
    }
  }

  String FileUtil::readString(const String& path) {
#ifdef _WIN32
    fs::path fs_path = fs::u8path(path);
    std::ifstream in(fs_path, std::ios::binary);
#else
    std::ifstream in(path, std::ios::binary);
#endif
    if (!in) {
      return "";
    }

    in.seekg(0, std::ios::end);
    const std::streamsize size = in.tellg();
    in.seekg(0, std::ios::beg);

    String content(size, '\0');
    in.read(&content[0], size);
    in.close();
    return content;
  }

  bool FileUtil::writeString(const String& path, const String& text) {
#ifdef _WIN32
    std::filesystem::path fs_path = std::filesystem::u8path(path);
    std::ofstream out(fs_path, std::ios::binary);
#else
    std::ofstream out(path.data(), std::ios::binary);
#endif
    if (!out) {
      return false;
    }
    out.write(text.data(), text.size());
    const bool res = out.good();
    out.close();
    return res;
  }
}
