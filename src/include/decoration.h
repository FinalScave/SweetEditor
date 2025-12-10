//
// Created by Scave on 2025/12/7.
//

#ifndef SWEETEDITOR_DECORATION_H
#define SWEETEDITOR_DECORATION_H

#include <cstdint>
#include "macro.h"

namespace NS_SWEETEDITOR {
  /// 编辑器样式定义
  struct Style {
    /// 样式ID
    uint32_t style_id {0};
    /// 颜色值
    int32_t color {0};
    /// 是否加粗
    bool is_bold {false};
    /// 是否斜体
    bool is_italic {false};
  };

  /// 编辑器样式注册表
  class StyleRegistry {
  public:
    /// 注册一个高亮样式
    /// @param style 高亮样式信息
    void registerStyle(Style&& style);

    /// 根据样式ID取样式信息
    /// @param style_id 样式ID
    /// @return 对应的样式信息
    Style& getStyle(uint32_t style_id);
  private:
    HashMap<uint32_t, Style> style_map_;
  };

  /// 高亮Span定义
  struct StyleSpan {
    /// 在行中的起始列
    uint32_t column {0};
    /// Span的字符长度
    uint32_t length {0};
    /// 样式ID
    uint32_t style_id {0};
  };

  /// 镶嵌内容类型枚举
  enum struct InlayType {
    /// 镶嵌文本
    TEXT = 0,
    /// 镶嵌图标
    ICON = 1,
  };

  /// 镶嵌内容
  struct InlayHint {
    /// 镶嵌格式
    InlayType type {InlayType::TEXT};
    /// 在行中的起始列
    uint32_t column {0};
    /// 镶嵌的文本内容
    U8String text;
    /// 镶嵌的图标ID
    int32_t icon_id {0};
  };

  /// 幽灵文本
  struct PhantomText {
    /// 在行中的起始列
    uint32_t column {0};
    /// 镶嵌的文本内容
    U8String text;
  };

  /// 所有嵌入文本和样式的操作接口
  class DecorationManager {
  public:
    DecorationManager();

    Ptr<StyleRegistry> getStyleRegistry();

    //StyleSpan& getStyleSpan(size_t line, uint32_t column, uint32_t length);
  private:
    Ptr<StyleRegistry> m_style_reg_;
    Vector<Vector<StyleSpan>> m_spans_;
    Vector<Vector<InlayHint>> m_inlay_hints_;
    Vector<Vector<PhantomText>> m_phantom_texts_;
  };
}

#endif //SWEETEDITOR_DECORATION_H