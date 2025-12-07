//
// Created by Scave on 2025/12/7.
//

#ifndef SWEETEDITOR_LAYOUT_H
#define SWEETEDITOR_LAYOUT_H

#include "document.h"
#include "decoration.h"
#include "visual.h"

namespace NS_SWEETEDITOR {
  /// 自动换行模式枚举
  enum struct WrapMode {
    /// 不换行
    NONE,
    /// 字符级换行
    CHAR_BREAK,
    /// 单词级换行
    WORD_BREAK,
  };

  /// 文本布局后的断行数据
  struct TextLine {
    /// 一行的文本
    U8String text;
  };

  struct VisibleRegion {
    size_t start_line {0};
    size_t end_line {0};
  };

  /// 文本宽度测量接口，由各平台实现
  class TextMeasurer {
  public:
    virtual ~TextMeasurer() = default;

    /// 测量指定文本内容的宽度
    /// @param text 文本内容
    /// @param is_bold 是否为加粗字体
    /// @return 测量后的宽度
    virtual float measureWidth(const U8String& text, bool is_bold) = 0;

    /// 获取字体高度
    /// @return 字体高度
    virtual float getFontHeight() = 0;
  };

  /// 文本布局引擎
  class TextLayout {
  public:
    explicit TextLayout(const Ptr<TextMeasurer>& measurer);

    void loadDocument(const Ptr<Document>& document);

    void setViewport(const Viewport& viewport);

    void setViewState(const ViewState& view_state);

    void setWrapMode(WrapMode mode);

    Vector<TextLine> layoutLine(size_t line);

    Vector<VisualLine> composeVisibleVisualLines();

    const U8String& getTextById(int64_t text_id);

    void testMonospace();
  private:
    Ptr<TextMeasurer> m_measurer_;
    Ptr<Document> m_document_;
    Ptr<DecorationManager> m_decoration_manager_;
    Viewport m_viewport_;
    ViewState m_view_state_;
    WrapMode m_wrap_mode_ {WrapMode::NONE};
    bool m_is_monospace_ {true};
    float m_line_height_ {20};
    // text_id 到相应文本的映射
    HashMap<int64_t, U8String> m_text_mapping_;
    int64_t m_text_id_counter_ {0};
    // 每个字符的测量宽度缓存
    HashMap<U8String, float> m_text_widths_;

    float measureWidth(const U8String& text, bool is_bold);
    int64_t createTextId(const U8String& text);
  };
}

#endif //SWEETEDITOR_LAYOUT_H