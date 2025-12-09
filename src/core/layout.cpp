//
// Created by Scave on 2025/12/7.
//
#include <cmath>
#include <simdutf/simdutf.h>
#include <utf8/utf8.h>
#include "layout.h"
#include "logging.h"

namespace NS_SWEETEDITOR {
  TextLayout::TextLayout(const Ptr<TextMeasurer>& measurer): m_measurer_(measurer) {
    m_decoration_manager_ = makePtr<DecorationManager>();
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

  Vector<TextLine> TextLayout::layoutLine(size_t line) {
    U8String line_text = m_document_->getLineU8Text(line);
    if (m_wrap_mode_ == WrapMode::NONE) {
      return {{line_text}};
    } else if (m_wrap_mode_ == WrapMode::CHAR_BREAK) {
      return {{line_text}};
    } else {
      return {{line_text}};
    }
  }

  Vector<VisualLine> TextLayout::composeVisibleVisualLines() {
    if (!m_viewport_.valid()) {
      return {};
    }
    LOGD("composeVisibleVisualLines start");
    // 清空状态
    m_text_mapping_.clear();
    m_text_id_counter_ = 0;
    // 找第一行和最后一行可见的
    if (m_document_ == nullptr) {
      return {};
    }
    Vector<LogicalLine>& logical_lines = m_document_->getLogicalLines();
    if (logical_lines.empty()) {
      return {};
    }
    LOGD("composeVisibleVisualLines find visible line");
    size_t first_line = 0;
    size_t last_line = logical_lines.size() - 1;
    float first_y = 0;
    float current_y = 0;
    const size_t size = logical_lines.size();
    for (size_t i = 0; i < size; ++i) {
      LogicalLine& logical_line = logical_lines[i];
      if (logical_line.height < 0) {
        logical_line.height = m_line_height_;
      }
      if (current_y + logical_line.height > m_view_state_.scroll_y) {
        first_line = i;
        first_y = current_y - m_view_state_.scroll_y;
        current_y += logical_line.height;
        break;
      }
    }
    for (size_t i = first_line + 1; i < size; ++i) {
      LogicalLine& logical_line = logical_lines[i];
      if (logical_line.height < 0) {
        logical_line.height = m_line_height_;
      }
      if (current_y + logical_line.height > m_view_state_.scroll_y + m_viewport_.height) {
        last_line = i;
        break;
      }
      current_y += logical_line.height;
    }
    LOGD("composeVisibleVisualLines build visual");
    // 构建视觉行（仅扫描可见列）
    current_y = first_y;
    Vector<VisualLine> visual_lines;
    for (size_t i = first_line; i <= last_line; ++i) {
      LogicalLine& logical_line = logical_lines[i];
      m_document_->updateDirtyLine(i, logical_line);
      auto text_begin = logical_line.cached_text.begin();
      auto text_end = logical_line.cached_text.end();
      float current_x = 0;
      size_t start_byte = 0;
      size_t end_byte = logical_line.cached_text.length();
      float first_x = 0;
      size_t current_byte = 0;
      while (text_begin != text_end) {
        auto char_start = text_begin;
        utf8::next(text_begin, text_end);
        U8String char_text(char_start, text_begin);
        float char_width = measureWidth(char_text, false);
        if (current_x + char_width > m_view_state_.scroll_x) {
          start_byte = current_byte;
          first_x = current_x - m_view_state_.scroll_x;
          current_x += char_width;
          current_byte += char_text.length();
          break;
        }
        current_x += char_width;
        current_byte += char_text.length();
      }
      while (text_begin != text_end) {
        auto char_start = text_begin;
        utf8::next(text_begin, text_end);
        U8String char_text(char_start, text_begin);
        float char_width = measureWidth(char_text, false);
        if (current_x + char_width > m_view_state_.scroll_x + m_viewport_.width) {
          end_byte = current_byte;
          current_x += char_width;
          current_byte += char_text.length();
          break;
        }
        current_x += char_width;
        current_byte += char_text.length();
      }
      // 截取可见区域作为视觉行
      VisualLine visual_line;
      visual_line.logical_line = i;
      U8String visible_text = logical_line.cached_text.substr(start_byte, end_byte - start_byte);
      VisualRun run = {VisualRunType::TEXT, first_x, current_y, createTextId(visible_text), 0};
      visual_line.runs.push_back(run);
      visual_lines.push_back(visual_line);
      current_y += logical_line.height;
    }
    return visual_lines;
  }

  const U8String& TextLayout::getTextById(int64_t text_id) {
    return m_text_mapping_[text_id];
  }

  void TextLayout::resetMeasurer() {
    FontMetrics metrics = m_measurer_->getFontMetrics();
    m_line_height_ = metrics.descent - metrics.ascent;
#ifdef _WIN32
    static const U16String test_chars = L"iIl1!.,;:W0@";
#else
    static const U16String test_chars = u"iIl1!.,;:W0@";
#endif
#ifdef _MSC_VER
    static const size_t test_chars_len = 12;
#else
    static const size_t test_chars_len = test_chars.size();
#endif
    float widths[test_chars_len];
    float sum = 0;
    // 测量每个字符的宽度
    for (int i = 0; i < test_chars_len; i++) {
#ifdef _WIN32
      char16_t ch = static_cast<char16_t>(test_chars.at(i));
#else
      char16_t ch = test_chars.at(i);
#endif
      size_t u8_len = simdutf::utf8_length_from_utf16(&ch, 1);
      char* u8_text = new char[u8_len + 1];
      size_t count = simdutf::convert_utf16_to_utf8(&ch, 1, u8_text);
      u8_text[u8_len] = '\0';
      widths[i] = m_measurer_->measureWidth(u8_text, false);
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
  }

  float TextLayout::measureWidth(const U8String& text, bool is_bold) {
    const auto it = m_text_widths_.find(text);
    if (it == m_text_widths_.end()) {
      float width = m_measurer_->measureWidth(text, is_bold);
      m_text_widths_.insert_or_assign(text, width);
      return width;
    }
    return it->second;
  }

  int64_t TextLayout::createTextId(const U8String& text) {
    int64_t id = m_text_id_counter_++;
    m_text_mapping_.insert_or_assign(id, text);
    return id;
  }
}
