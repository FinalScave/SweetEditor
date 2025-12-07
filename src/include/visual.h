//
// Created by Scave on 2025/12/6.
//

#ifndef SWEETEDITOR_VISUAL_H
#define SWEETEDITOR_VISUAL_H

#include <cstdint>
#include <nlohmann/json.hpp>
#include "foundation.h"

namespace NS_SWEETEDITOR {
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
    /// 片段的文本内容ID（只有TEXT、INLAY_HINT、PHANTOM_TEXT会有）
    int64_t text_id {-1};
    /// 样式ID
    uint32_t style_id {0};

    U8String dump() const;
  };

  /// 视觉渲染行数据定义
  struct VisualLine {
    /// 逻辑行行号
    size_t logical_line {0};
    /// 视觉行包含的文本片段
    Vector<VisualRun> runs;

    U8String dump() const;
  };

  /// 光标数据
  struct Cursor {
    /// 光标位置
    PointF position;
    /// 是否显示拖拽游标
    bool show_dragger {false};

    U8String dump() const;
  };

  /// 划线方向
  enum struct GuideLineDirection {
    /// 横线
    HORIZONTAL,
    /// 竖线
    VERTICAL,
  };

  /// 代码区块划线
  struct GuideLine {
    /// 划线方向
    GuideLineDirection direction {GuideLineDirection::VERTICAL};
    /// 起始点
    PointF start;
    /// 终止点
    PointF end;
  };

  /// 编辑器渲染模型
  struct EditorRenderModel {
    /// 当前行背景坐标
    PointF current_line;
    /// 视觉上要渲染的文字行
    Vector<VisualLine> lines;
    /// 光标
    Cursor cursor;
    /// 代码区块划线
    Vector<GuideLine> guide_lines;

    U8String dump() const;
    U8String toJson() const;
  };

  U8String dumpEnum(VisualRunType type);
  U8String dumpEnum(GuideLineDirection direction);

  NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(PointF, x, y)
  NLOHMANN_JSON_SERIALIZE_ENUM( VisualRunType, {
    {VisualRunType::TEXT, "TEXT"},
    {VisualRunType::WHITESPACE, "WHITESPACE"},
    {VisualRunType::NEWLINE, "NEWLINE"},
    {VisualRunType::INLAY_HINT, "INLAY_HINT"},
    {VisualRunType::PHANTOM_TEXT, "PHANTOM_TEXT"},
  })
  NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(VisualRun, type, x, y, text_id, style_id)
  NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(VisualLine, logical_line, runs)
  NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Cursor, position, show_dragger)
  NLOHMANN_JSON_SERIALIZE_ENUM( GuideLineDirection, {
    {GuideLineDirection::VERTICAL, "VERTICAL"},
    {GuideLineDirection::HORIZONTAL, "HORIZONTAL"},
  })
  NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(GuideLine, direction, start, end)
  NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(EditorRenderModel, current_line, lines, cursor, guide_lines)
}

#endif //SWEETEDITOR_VISUAL_H