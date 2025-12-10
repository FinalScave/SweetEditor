//
// Created by Scave on 2025/12/7.
//
#include <cmath>
#include <simdutf/simdutf.h>
#include <utf8/utf8.h>
#include "layout.h"
#include "logging.h"

namespace NS_SWEETEDITOR {
  TextLayout::TextLayout(const Ptr<TextMeasurer>& measurer, const Ptr<DecorationManager>& decoration_manager)
    : m_measurer_(measurer), m_decoration_manager_(decoration_manager) {
    resetMeasurer();
  }

  void TextLayout::loadDocument(const Ptr<Document>& document) {
    m_document_ = document;
  }

  void TextLayout::setViewport(const Viewport& viewport) {
    m_viewport_ = viewport;
  }

  void TextLayout::setViewState(const ViewState& view_state) {
    m_view_state_ = view_state;
  }

  void TextLayout::setWrapMode(WrapMode mode) {
    m_wrap_mode_ = mode;
  }

  void TextLayout::layoutLine(size_t index, LogicalLine& logical_line) {
    if (!logical_line.is_layout_dirty) {
      return;
    }
    // 清空文本ID
    for (const VisualLine& line : logical_line.visual_lines) {
      for (const VisualRun& run : line.runs) {
        removeTextId(run.text_id);
      }
    }
    logical_line.visual_lines.clear();
    m_document_->updateDirtyLine(index, logical_line);
    if (index > 0) {
      const LogicalLine& prev_line = m_document_->getLogicalLines()[index - 1];
      logical_line.start_y = prev_line.start_y + prev_line.height;
    } else {
      logical_line.start_y = 0;
    }
    const U16String& line_text = logical_line.cached_text;
    VisualLine visual_line = {index};
    visual_line.line_number_position = {m_params_.line_number_margin, logical_line.start_y};
    if (m_wrap_mode_ == WrapMode::NONE) {
      logical_line.height = m_params_.font_height * m_params_.line_spacing_mult + m_params_.line_spacing_add;
      // 将span、inlay-hints、phantom-text组合起来，便于后续断行
      VisualRun text_run = {VisualRunType::TEXT, 0, logical_line.cached_text.length()};
      text_run.y = logical_line.start_y;
      text_run.text_id = createTextId(line_text);
      visual_line.runs.push_back(text_run);
    } else if (m_wrap_mode_ == WrapMode::CHAR_BREAK) {
    } else {
    }
    logical_line.visual_lines.push_back(std::move(visual_line));
    logical_line.is_layout_dirty = false;
  }

  void TextLayout::composeRenderModel(EditorRenderModel& model) {
    if (!m_viewport_.valid() || m_document_ == nullptr) {
      return;
    }
    Vector<LogicalLine>& logical_lines = m_document_->getLogicalLines();
    if (logical_lines.empty()) {
      return;
    }
    // 计算行号宽度
    m_params_.line_number_width = computeLineNumberWidth();
    // 计算第一行和最后一行可见的
    VisibleLineInfo visile_line_info = computeVisibleLineInfo();
    // 构建视觉行（仅扫描可见列）
    for (size_t i = visile_line_info.first_line; i <= visile_line_info.last_line; ++i) {
      LogicalLine& logical_line = logical_lines[i];
      // 对逻辑行重组的VisualLine进行视口裁剪
      for (VisualLine& visual_line : logical_line.visual_lines) {
        cropVisualLineRuns(visual_line);
        model.lines.push_back(visual_line);
      }
    }
    model.split_x = m_params_.line_number_margin * 2 + m_params_.line_number_width;
  }

  const U16String& TextLayout::getTextById(int64_t text_id) {
    return m_text_mapping_[text_id];
  }

  void TextLayout::resetMeasurer() {
    FontMetrics metrics = m_measurer_->getFontMetrics();
    m_params_.font_height = metrics.descent - metrics.ascent;
    static const U16String test_chars = CHAR16("iIl1!.,;:W0@");
    static const U16String test_number = CHAR16("9");
    static const U16String test_space = CHAR16(" ");
#ifdef _MSC_VER
    static const size_t test_chars_len = 12;
#else
    static const size_t test_chars_len = test_chars.size();
#endif
    float widths[test_chars_len];
    float sum = 0;
    // 测量每个字符的宽度
    for (int i = 0; i < test_chars_len; i++) {
      widths[i] = m_measurer_->measureWidth(test_chars.substr(i, 1), false);
      sum += widths[i];
    }
    // 计算平均宽度和标准差
    float average = sum / test_chars_len;
    float variance = 0;
    for (float w : widths) {
      variance += pow(w - average, 2);
    }
    float std_dev = sqrt(variance / test_chars_len);
    // 如果标准差非常小，则认为所有字符宽度一致
    float tolerance = 0.5f;
    m_is_monospace_ = std_dev < tolerance;
    LOGD("m_is_monospace_: %s", m_is_monospace_ ? "true" : "false");
    m_number_width_ = m_measurer_->measureWidth(test_number, 0);
    m_space_width_ = m_measurer_->measureWidth(test_space, 0);
  }

  EditorParams& TextLayout::getEditorParams() {
    return m_params_;
  }

  float TextLayout::measureWidth(const U16String& text, bool is_bold) {
    const auto it = m_text_widths_.find(text);
    if (it == m_text_widths_.end()) {
      float width = m_measurer_->measureWidth(text, is_bold);
      m_text_widths_.insert_or_assign(text, width);
      return width;
    }
    return it->second;
  }

  int64_t TextLayout::createTextId(const U16String& text) {
    int64_t id = m_text_id_counter_++;
    m_text_mapping_.insert_or_assign(id, text);
    return id;
  }

  void TextLayout::removeTextId(int64_t id) {
    m_text_mapping_.erase(id);
  }

  VisibleLineInfo TextLayout::computeVisibleLineInfo() {
    Vector<LogicalLine>& logical_lines = m_document_->getLogicalLines();
    if (logical_lines.empty()) {
      return {};
    }
    size_t first_line = 0;
    size_t last_line = logical_lines.size() - 1;
    float first_y = 0;
    float current_y = 0;
    const size_t size = logical_lines.size();
    for (size_t i = 0; i < size; ++i) {
      LogicalLine& logical_line = logical_lines[i];
      layoutLine(i, logical_line);
      if (current_y + logical_line.height > m_view_state_.scroll_y) {
        first_line = i;
        first_y = current_y - m_view_state_.scroll_y;
        current_y += logical_line.height;
        break;
      }
    }
    for (size_t i = first_line + 1; i < size; ++i) {
      LogicalLine& logical_line = logical_lines[i];
      layoutLine(i, logical_line);
      if (current_y + logical_line.height > m_view_state_.scroll_y + m_viewport_.height) {
        last_line = i;
        break;
      }
      current_y += logical_line.height;
    }
    return {first_line, last_line, first_y};
  }

  void TextLayout::cropVisualLineRuns(VisualLine& visual_line) {
    float first_x = m_params_.line_number_margin * 2 + m_params_.line_number_width;
    float current_x = first_x;
    bool has_crop_start = false;
    auto run_it = visual_line.runs.begin();
    auto run_it_end = visual_line.runs.end();
    while (run_it != run_it_end) {
      VisualRun& run = *run_it;
      run.x = current_x - first_x;
      const U16String& run_text = getTextById(run.text_id);
      auto text_begin = run_text.begin();
      auto text_end = run_text.end();
      size_t start_u16_index = 0;
      size_t end_u16_index = run_text.length() - 1;
      size_t current_u16_index = 0;
      while (text_begin != text_end) {
        auto char_start = text_begin;
        utf8::next16(text_begin, text_end);
        U16String u16_char_text(char_start, text_begin);
        float char_width = measureWidth(u16_char_text, false);
        if (!has_crop_start && current_x + char_width > m_view_state_.scroll_x) {
          start_u16_index = current_u16_index;
          first_x = current_x;
          run.x = current_x;
          current_u16_index += u16_char_text.length();
          has_crop_start = true;
        } else if (current_x + char_width > m_view_state_.scroll_x + m_viewport_.width) {
          end_u16_index = current_u16_index;
          current_x += char_width;
          current_u16_index += u16_char_text.length();
          break;
        }
        current_x += char_width;
        current_u16_index += u16_char_text.length();
      }
      if (end_u16_index == 0) {
        run_it = visual_line.runs.erase(run_it);
        continue;
      } else if (start_u16_index > 0 || end_u16_index < run_text.length() - 1) {
        removeTextId(run.text_id);
        U16String visible_text = run_text.substr(start_u16_index, end_u16_index - start_u16_index);
        run.text_id = createTextId(visible_text);
      }
      ++run_it;
    }
  }

  float TextLayout::computeLineNumberWidth() const {
    size_t line_count = std::max(static_cast<size_t>(1), m_document_->getLogicalLines().size());
    uint32_t line_number_bits = static_cast<uint32_t>(std::log10(line_count) + 1 + 1e-10);
    if (m_is_monospace_) {
      return m_number_width_ * line_number_bits;
    } else {
      U16String test_text;
      test_text.reserve(line_number_bits);
      for (uint32_t i = 0; i < line_number_bits; ++i) {
        test_text.push_back(CHAR16('9'));
      }
      return m_measurer_->measureWidth(test_text, 0);
    }
  }
}
