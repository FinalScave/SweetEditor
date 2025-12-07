//
// Created by Scave on 2025/12/6.
//

#ifndef SWEETEDITOR_VISUAL_H
#define SWEETEDITOR_VISUAL_H

#include <cstdint>
#include "macro.h"

namespace NS_SWEETEDITOR {
  /// 高亮Span定义
  struct StyleSpan {
    /// 在行中的起始列
    uint32_t column {0};
    /// Span的字符长度
    uint32_t length {0};
    /// 样式ID
    uint32_t style_id {0};
  };

  /// Span的操作接口
  class SpanManager {
  public:
    StyleSpan& getStyleSpan(uint32_t column, uint32_t length);
  };

  /// 视觉渲染片段的枚举定义
  enum struct VisualRunType {
    /// 普通文本
    TEXT,
    /// 空格
    WHITESPACE,
    /// 换行符
    NEWLINE,
    /// 镶嵌内容（文本或图标）
    INLAY_HINT,
    /// 幽灵文本（用于Copilot代码提示）
    PHANTOM_TEXT
  };

  /// 视觉上渲染的每个文本片段结构定义
  struct VisualRun {
    /// 片段类型
    VisualRunType type {VisualRunType::TEXT};
    /// 绘制的起始横坐标
    float x {0};
    /// 绘制的起始纵坐标
    float y {0};
    /// 片段的文本内容（只有TEXT、INLAY_HINT、PHANTOM_TEXT会有）
    U16String text;
    /// 样式ID
    uint32_t style_id {0};
  };

  /// 视觉渲染行数据定义
  struct VisualLine {
    /// 逻辑行行号
    size_t logical_line {0};
    /// 视觉行包含的文本片段
    Vector<VisualRun> runs;
  };
}

#endif //SWEETEDITOR_VISUAL_H