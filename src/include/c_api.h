//
// Created by Scave on 2025/12/8.
//

#ifndef SWEETEDITOR_C_API_H
#define SWEETEDITOR_C_API_H
#include <cstdint>

#if defined(WINDOWS) || defined(_WIN32) || defined(_WIN64)
  #ifdef SWEETEDITOR_EXPORT
    #define EDITOR_API __declspec(dllexport)
  #else
    #define EDITOR_API __declspec(dllimport)
  #endif
#else
  #define EDITOR_API __attribute__((visibility("default")))
#endif

extern "C" {

typedef float (__stdcall* MeasureTextWidth)(const U16Char* text, uint32_t style_id);
typedef void (__stdcall* GetFontMetrics)(float* arr, size_t length);

/// 创建Document类并返回其句柄
/// @param text UTF16文本内容
/// @return Document句柄
EDITOR_API intptr_t create_document_from_utf16(const U16Char* text);

/// 创建Document类并返回其句柄（从本地文件创建）
/// @param path 本地文件路径
/// @return Document句柄
EDITOR_API intptr_t create_document_from_file(const char* path);

/// 释放Document
/// @param document_handle Document句柄
EDITOR_API void free_document(intptr_t document_handle);

/// 获取Document的UTF8文本
/// @param document_handle Document句柄
/// @return UTF8文本内容
EDITOR_API const char* get_document_text(intptr_t document_handle);

/// 获取Document的总行数
/// @param document_handle Document句柄
/// @return Document的总行数
EDITOR_API size_t get_document_line_count(intptr_t document_handle);

/// 获取Document的总行数
/// @param document_handle Document句柄
/// @param line 行号
/// @return 指定行的UTF8文本内容
EDITOR_API const U16Char* get_document_line_text(intptr_t document_handle, size_t line);

/// 创建EditorCore类并返回其句柄
/// @param touch_slop 单击移动的阈值
/// @param double_tap_timeout 手势判定双击点击的时间差
/// @param measurer_func 文本宽度测量函数
/// @param metrics_func 文本度量信息函数
/// @return EditorCore句柄
EDITOR_API intptr_t create_editor(float touch_slop, int64_t double_tap_timeout, MeasureTextWidth measurer_func, GetFontMetrics metrics_func);

/// 释放EditorCore
/// @param editor_handle EditorCore句柄
EDITOR_API void free_editor(intptr_t editor_handle);

/// 设置Editor的Viewport
/// @param editor_handle EditorCore句柄
/// @param width Editor视图宽度
/// @param height Editor视图高度
EDITOR_API void set_editor_viewport(intptr_t editor_handle, int16_t width, int16_t height);

/// 加载Document
/// @param editor_handle EditorCore句柄
/// @param document_handle Document句柄
EDITOR_API void set_editor_document(intptr_t editor_handle, intptr_t document_handle);

/// 处理手势事件
/// @param editor_handle EditorCore句柄
/// @param type 事件类型
/// @param pointer_count 手指点数
/// @param points 每个点的数据
/// @return 处理结果（以JSON格式呈现）
EDITOR_API const U16Char* handle_editor_gesture_event(intptr_t editor_handle, uint8_t type, uint8_t pointer_count, float* points);

/// 重置editor文本测量结果（更新字体时调用）
/// @param editor_handle EditorCore句柄
EDITOR_API void reset_editor_text_measurer(intptr_t editor_handle);

/// 构建editor一帧的渲染模型
/// @param editor_handle EditorCore句柄
/// @return 渲染模型（以JSON格式呈现）
EDITOR_API const U16Char* build_editor_render_model(intptr_t editor_handle);

/// 获取指定ID的视觉文本
/// @param editor_handle EditorCore句柄
/// @param run_text_id 文本ID（渲染模型中的ID）
/// @return UTF8文本
EDITOR_API const U16Char* get_editor_visual_run_text(intptr_t editor_handle, int64_t run_text_id);

/// 释放C++侧的字符串内存
/// @param string_ptr 字符串指针
EDITOR_API void free_u16_string(intptr_t string_ptr);

#ifdef _WIN32
/// 设置Dll调用时的Crash日志输出，仅Windows可用
EDITOR_API void init_unhandled_exception_handler();
#endif

}

#endif //SWEETEDITOR_C_API_H
