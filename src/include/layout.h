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

  /// 字体度量信息
  struct FontMetrics {
    float ascent;
    float descent;
  };

  /// 可见行区域信息
  struct VisibleLineInfo {
    /// 可见的第一行行号索引
    size_t first_line {0};
    /// 可见的最后一行行号索引
    size_t last_line {0};
    /// 可见的第一行纵坐标(只有部分可见情况下该坐标为负值)
    float first_line_y {0};
  };

  /// 文本宽度测量接口，由各平台实现
  class TextMeasurer {
  public:
    virtual ~TextMeasurer() = default;

    /// 测量指定文本内容的宽度
    /// @param text 文本内容
    /// @param style_id 文本样式
    /// @return 测量后的宽度
    virtual float measureWidth(const U16String& text, uint32_t style_id) = 0;

    /// 获取字体度量信息
    /// @return 字体度量信息
    virtual FontMetrics getFontMetrics() = 0;
  };

  /// 文本布局引擎
  class TextLayout {
  public:
    TextLayout(const Ptr<TextMeasurer>& measurer, const Ptr<DecorationManager>& decoration_manager);

    void loadDocument(const Ptr<Document>& document);

    void setViewport(const Viewport& viewport);

    void setViewState(const ViewState& view_state);

    void setWrapMode(WrapMode mode);

    void layoutLine(size_t index, LogicalLine& logical_line);

    void composeRenderModel(EditorRenderModel& model);

    const U16String& getTextById(int64_t text_id);

    void resetMeasurer();

    EditorParams& getEditorParams();
  private:
    Ptr<TextMeasurer> m_measurer_;
    Ptr<Document> m_document_;
    Ptr<DecorationManager> m_decoration_manager_;
    Viewport m_viewport_;
    ViewState m_view_state_;
    WrapMode m_wrap_mode_ {WrapMode::NONE};
    EditorParams m_params_;
    bool m_is_monospace_ {true};
    float m_number_width_;
    float m_space_width_;
    // text_id 到相应文本的映射
    HashMap<int64_t, U16String> m_text_mapping_;
    int64_t m_text_id_counter_ {0};
    // 每个字符的测量宽度缓存
    HashMap<U16String, float> m_text_widths_;

    float measureWidth(const U16String& text, bool is_bold);
    int64_t createTextId(const U16String& text);
    void removeTextId(int64_t id);
    VisibleLineInfo computeVisibleLineInfo();
    void cropVisualLineRuns(VisualLine& visual_line);
    float computeLineNumberWidth() const;
  };
}

#endif //SWEETEDITOR_LAYOUT_H