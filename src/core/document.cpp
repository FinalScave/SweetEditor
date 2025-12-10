//
// Created by Scave on 2025/12/2.
//
#include <stdexcept>
#include <algorithm>
#include <simdutf/simdutf.h>
#include "document.h"
#include "utility.h"

namespace NS_SWEETEDITOR {
  // ================================================== Document ===================================================
  Document::Document(U8String&& original_string): m_original_buffer_(makeUPtr<U8StringBuffer>(std::move(original_string))) {
    rebuildBufferSegments();
  }

  Document::Document(const U8String& original_string): m_original_buffer_(makeUPtr<U8StringBuffer>(original_string)) {
    rebuildBufferSegments();
  }

  Document::Document(const U16String& original_string) {
    U8String utf8_text;
    StrUtil::convertUTF16ToUTF8(original_string, utf8_text);
    m_original_buffer_ = makeUPtr<U8StringBuffer>(std::move(utf8_text));
    rebuildBufferSegments();
  }

  Document::Document(UPtr<Buffer>&& original_buffer): m_original_buffer_(std::move(original_buffer)) {
    rebuildBufferSegments();
  }

  Document::~Document() = default;

  U8String Document::getU8Text() {
    return getU8Text(0, m_total_bytes_);
  }

  U16String Document::getU16Text() {
    U8String utf8_text = getU8Text(0, m_total_bytes_);
    U16String result;
    StrUtil::convertUTF8ToUTF16(utf8_text, result);
    return result;
  }

  size_t Document::getLineCount() const {
    return m_logical_lines_.size();
  }

  U16String Document::getLineU16Text(size_t line) const {
    if (line >= m_logical_lines_.size()) {
      throw std::out_of_range("Document::getLineU16Text line index out of range");
    }
    if (!m_logical_lines_[line].is_char_dirty) {
      return m_logical_lines_[line].cached_text;
    }
    const size_t byte_length = getByteLengthOfLine(line);
    U8String utf8_text = getU8Text(m_logical_lines_[line].start_byte, byte_length);
    U16String result;
    StrUtil::convertUTF8ToUTF16(utf8_text, result);
    return result;
  }

  uint32_t Document::getLineColumns(size_t line) {
    if (line >= m_logical_lines_.size()) {
      throw std::out_of_range("Document::getLineColumns line index out of range");
    }
    updateDirtyLine(line, m_logical_lines_[line]);
    return m_logical_lines_[line].cached_text.size();
  }

  TextPosition Document::getPositionFromCharIndex(size_t char_index) const {
    if (m_logical_lines_.empty()) {
      return TextPosition{0, 0};
    }
    if (char_index == 0) {
      return TextPosition{0, 0};
    }
    const size_t target_line = getLineFromCharIndex(char_index);
    const LogicalLine& logical_line = m_logical_lines_[target_line];
    size_t column = char_index - logical_line.start_char;
    return TextPosition{target_line, column};
  }

  size_t Document::getCharIndexFromPosition(const TextPosition& position) {
    size_t line = position.line;
    size_t column = position.column;

    if (line >= m_logical_lines_.size()) {
      line = m_logical_lines_.size() - 1;
    }

    const LogicalLine& line_snapshot = m_logical_lines_[line];
    uint32_t line_chars = getLineColumns(line);
    if (column > line_chars) {
      column = line_chars;
    }
    return line_snapshot.start_char + column;
  }

  void Document::insertU8Text(const TextPosition& position, const U8String& text) {
    if (text.empty()) {
      return;
    }
    const size_t byte_offset = getByteOffsetFromPosition(position);
    insertU8Text(byte_offset, text);
  }

  void Document::deleteU8Text(const TextRange& range) {
    size_t start_byte = getByteOffsetFromPosition(range.start);
    size_t byte_length = getByteOffsetFromPosition(range.end) - start_byte;
    deleteU8Text(start_byte, byte_length);
  }

  void Document::replaceU8Text(const TextRange& range, const U8String& text) {
    deleteU8Text(range);
    insertU8Text(range.start, text);
  }

  void Document::rebuildBufferSegments() {
    m_edit_buffer_ = makeUPtr<U8StringBuffer>();
    m_buffer_segments_.clear();
    m_buffer_segments_.push_back({SegmentType::ORIGINAL, 0, m_original_buffer_->size()});
    m_total_bytes_ = m_original_buffer_->size();
    rebuildLogicalLines();
  }

  void Document::rebuildLogicalLines() {
    m_logical_lines_.clear();
    m_logical_lines_.push_back({0, 0, {}, true});
    const char* data = m_original_buffer_->data();
    const size_t size = m_original_buffer_->size();
    size_t i = 0;
    for (i = 0; i < size; ++i) {
      if (data[i] == '\n') {
        m_logical_lines_.push_back({i + 1, 0, {}, true});
      }
    }
  }

  U8String Document::getU8Text(size_t start_byte, size_t byte_length) const {
    if (start_byte >= m_total_bytes_) {
      return "";
    }
    if (start_byte + byte_length > m_total_bytes_) {
      byte_length = m_total_bytes_ - start_byte;
    }
    U8String result;
    result.reserve(byte_length);
    size_t current_byte = 0;
    size_t req_end_byte = start_byte + byte_length;
    for (const BufferSegment& segment : m_buffer_segments_) {
      size_t seg_start = current_byte;
      size_t seg_end = current_byte + segment.byte_length;

      if (seg_end <= start_byte) {
        current_byte += segment.byte_length;
        continue;
      }

      if (seg_start >= req_end_byte) {
        break;
      }

      size_t copy_start = std::max(seg_start, start_byte);
      size_t copy_end = std::min(seg_end, req_end_byte);
      size_t copy_len = copy_end - copy_start;
      size_t seg_offset = copy_start - seg_start;
      const char* src_data = getSegmentData(segment);
      result.append(src_data + seg_offset, copy_len);
      current_byte += segment.byte_length;
    }
    return result;
  }

  void Document::insertU8Text(size_t start_byte, const U8String& text) {
    if (text.empty()) {
      return;
    }
    if (start_byte > m_total_bytes_) {
      start_byte = m_total_bytes_;
    }
    size_t edit_buffer_start = m_edit_buffer_->currentEnd();
    m_edit_buffer_->append(text);
    BufferSegment new_seg = {SegmentType::EDITED, edit_buffer_start, text.size()};
    bool is_inserted = false;
    size_t current_byte = 0;
    for (auto it = m_buffer_segments_.begin(); it != m_buffer_segments_.end(); ++it) {
      // 检查插入点是否在当前 Segment 内部（包含头部，不包含尾部，除非是最后一个）
      if (current_byte + it->byte_length > start_byte) {
        size_t offset_in_seg = start_byte - current_byte;
        // 插入点在 Segment 头部 (offset == 0)
        if (offset_in_seg == 0) {
          m_buffer_segments_.insert(it, new_seg);
          is_inserted = true;
          break;
        }
        // 插入点在 Segment 中间，进行拆分 (左 + 新 + 右)
        BufferSegment right = *it;
        right.start_byte += offset_in_seg;
        right.byte_length -= offset_in_seg;
        // 改当前 Segment 为左半部分
        it->byte_length = offset_in_seg;
        // 插入新 Segment 和 右半部分
        it = m_buffer_segments_.insert(it + 1, new_seg);
        m_buffer_segments_.insert(it + 1, right);
        is_inserted = true;
        break;
      }
      current_byte += it->byte_length;
    }
    // 中间没有插入则追加到末尾
    if (!is_inserted) {
      m_buffer_segments_.push_back(new_seg);
    }
    m_total_bytes_ += text.size();
    updateLogicalLinesByInsertText(start_byte, text);
  }

  void Document::deleteU8Text(size_t start_byte, size_t byte_length) {
    if (byte_length == 0) {
      return;
    }
    if (start_byte + byte_length > m_total_bytes_) {
      byte_length = m_total_bytes_ - start_byte;
    }
    size_t delete_end = start_byte + byte_length;
    size_t current_byte = 0;
    auto it = m_buffer_segments_.begin();
    while (it != m_buffer_segments_.end()) {
      size_t seg_len_original = it->byte_length;
      size_t seg_start = current_byte;
      size_t seg_end = current_byte + seg_len_original;
      // 计算交集
      size_t intersect_start = std::max(seg_start, start_byte);
      size_t intersect_end = std::min(seg_end, delete_end);
      if (intersect_start < intersect_end) { //有重叠
        size_t delete_len_in_seg = intersect_end - intersect_start;
        if (intersect_start == seg_start && intersect_end == seg_end) {
          // 全删
          it = m_buffer_segments_.erase(it);
        } else if (intersect_start == seg_start) {
          // 删头部
          it->start_byte += delete_len_in_seg;
          it->byte_length -= delete_len_in_seg;
          ++it;
        } else if (intersect_end == seg_end) {
          // 删尾部
          it->byte_length -= delete_len_in_seg;
          ++it;
        }
        else {
          // 删中间
          size_t left_len = intersect_start - seg_start;
          BufferSegment right = *it;
          // 右半部分的起始偏移 = 原始起始 + 左半部分长 + 被删掉的长度
          right.start_byte += (left_len + delete_len_in_seg);
          right.byte_length -= (left_len + delete_len_in_seg);
          it->byte_length = left_len;
          // 插入右半部分
          it = m_buffer_segments_.insert(it + 1, right);
          ++it;
        }
      } else {
        // 无重叠
        ++it;
      }
      current_byte += seg_len_original;
      if (current_byte >= delete_end) {
        break;
      }
    }
    m_total_bytes_ -= byte_length;
    updateLogicalLinesByDeleteText(start_byte, byte_length);
  }

  size_t Document::countChars(size_t start_byte, size_t byte_length) const {
    U8String text = getU8Text(start_byte, byte_length);
    return simdutf::count_utf8(text.data(), text.length());
  }

  Vector<LogicalLine>& Document::getLogicalLines() {
    return m_logical_lines_;
  }

  void Document::updateDirtyLine(size_t line, LogicalLine& logical_line) {
    if (logical_line.is_char_dirty) {
      const size_t byte_length = getByteLengthOfLine(line);
      U8String u8_text = getU8Text(logical_line.start_byte, byte_length);
      StrUtil::convertUTF8ToUTF16(u8_text, logical_line.cached_text);
      if (line > 0) {
        LogicalLine& prev_line = m_logical_lines_[line - 1];
        if (prev_line.is_char_dirty) {
          U8String prev_texts = getU8Text(0, prev_line.start_byte);
          const size_t prev_chars_count = simdutf::count_utf8(prev_texts.data(), prev_texts.length());
          prev_line.start_char = prev_chars_count;
        }
        const size_t prev_byte_length = getByteLengthOfLine(line - 1);
        U8String prev_line_text = getU8Text(prev_line.start_byte, prev_byte_length);
        logical_line.start_char = prev_line.start_char + simdutf::count_utf8(prev_line_text.data(), prev_line_text.length());
      } else {
        logical_line.start_char = 0;
      }
      logical_line.is_char_dirty = false;
    }
  }

  void Document::updateLogicalLinesByInsertText(size_t start_byte, const U8String& text) {
    const size_t line = getLineFromByteOffset(start_byte);
    Vector<LogicalLine> new_lines;
    for (size_t i = 0; i < text.size(); ++i) {
      if (text[i] == '\n') {
        LogicalLine logical_line;
        logical_line.start_byte = start_byte + i + 1;
        logical_line.is_char_dirty = true;
        new_lines.push_back(logical_line);
      }
    }
    if (!new_lines.empty()) {
      // 插入新行数据
      m_logical_lines_.insert(m_logical_lines_.begin() + line + 1, new_lines.begin(), new_lines.end());
    }
    // 更新受影响的后续行字节偏移，m_logical_lines[line + 1] 开始的所有项都要 start_byte 都要 + shift_amount
    size_t shift_amount = text.size();
    size_t start_shift_line = line + 1 + new_lines.size();
    for (size_t i = start_shift_line; i < m_logical_lines_.size(); ++i) {
      m_logical_lines_[i].start_byte += shift_amount;
      m_logical_lines_[i].is_char_dirty = true;
      m_logical_lines_[i].height = -1;
    }
  }

  void Document::updateLogicalLinesByDeleteText(size_t start_byte, size_t byte_length) {
    size_t end_byte = start_byte + byte_length;
    // 第一个line.start_byte > start_byte 的行
    size_t low = 0;
    size_t high = m_logical_lines_.size();
    while (low < high) {
      size_t mid = low + (high - low) / 2;
      if (m_logical_lines_[mid].start_byte <= start_byte) {
        low = mid + 1;
      } else {
        high = mid;
      }
    }
    size_t line_to_remove = low;
    // 第一个line.start_byte > end_byte 的行
    low = line_to_remove;
    high = m_logical_lines_.size();
    while (low < high) {
      size_t mid = low + (high - low) / 2;
      if (m_logical_lines_[mid].start_byte <= end_byte) {
        low = mid + 1;
      } else {
        high = mid;
      }
    }
    size_t line_to_keep = low;
    if (line_to_remove < line_to_keep) {
      m_logical_lines_.erase(m_logical_lines_.begin() + line_to_remove, m_logical_lines_.begin() + line_to_keep);
    }
    // 位移后续行，被删除区间后面的所有行，偏移量都要减去 length
    for (size_t i = line_to_remove; i < m_logical_lines_.size(); ++i) {
      m_logical_lines_[i].start_byte -= byte_length;
      m_logical_lines_[i].is_char_dirty = true;
      m_logical_lines_[i].height = -1;
    }
  }

  size_t Document::getByteOffsetFromPosition(const TextPosition& position) const {
    if (position.line >= m_logical_lines_.size()) {
      throw std::out_of_range("Document::getPositionByteOffset line index out of range");
    }
    size_t line_start_byte = m_logical_lines_[position.line].start_byte;
    size_t line_end_byte = (position.line + 1 < m_logical_lines_.size())
                     ? m_logical_lines_[position.line + 1].start_byte
                     : m_total_bytes_;
    size_t current_byte = 0;
    size_t scanned_u16_count = 0;
    size_t result = line_start_byte;

    auto it = m_buffer_segments_.begin();
    while (it != m_buffer_segments_.end()) {
        size_t seg_end = current_byte + it->byte_length;
        // segment 在这一行的范围内 (有交集)
        if (seg_end > line_start_byte && current_byte < line_end_byte) {
            // seg 开始被扫描的位置
            size_t intersect_start = std::max(current_byte, line_start_byte);
            // seg 结束扫描的位置
            size_t intersect_end = std::min(seg_end, line_end_byte);
            const char* seg_data = getSegmentData(*it);
            // seg 内部的起始读取位置
            size_t local_start = intersect_start - current_byte;
            size_t local_end = intersect_end - intersect_start;
            size_t i = 0;
            while (i < local_end && scanned_u16_count < position.column) {
                unsigned char c = static_cast<unsigned char>(seg_data[local_start + i]);

                size_t step = 1;
                size_t u16_count = 1;

                if (c < 0x80) { //ASCII
                  step = 1;
                  u16_count = 1;
                } else if ((c & 0xE0) == 0xC0) { //拉丁/希腊
                  step = 2;
                  u16_count = 1;
                } else if ((c & 0xF0) == 0xE0) { //常见中文
                  step = 3;
                  u16_count = 1;
                } else if ((c & 0xF8) == 0xF0) { // emoji
                  step = 4;
                  u16_count = 2;
                } else {  // invalid
                  step = 1;
                  u16_count = 1;
                }
                scanned_u16_count += u16_count;
                i += step;
            }
            result += i;
            if (scanned_u16_count >= position.column) {
                return result;
            }
        }

        current_byte += it->byte_length;
        ++it;

        // 已经超出这一行
        if (current_byte >= line_end_byte) {
          break;
        }
    }
    return result;
  }

  size_t Document::getLineFromByteOffset(size_t byte_offset) const {
    size_t low = 0;
    size_t high = m_logical_lines_.size();
    while (low < high) {
      size_t mid = low + (high - low) / 2;
      if (m_logical_lines_[mid].start_byte <= byte_offset) {
        low = mid + 1;
      } else {
        high = mid;
      }
    }
    return low - 1;
  }

  size_t Document::getLineFromCharIndex(size_t char_index) const {
    size_t left = 0;
    size_t right = m_logical_lines_.size() - 1;
    size_t target_line = 0;
    while (left <= right) {
      size_t mid = left + (right - left) / 2;
      const LogicalLine& logical_line = m_logical_lines_[mid];
      if (mid + 1 < m_logical_lines_.size() && m_logical_lines_[mid + 1].start_char <= char_index) {
        // 如果下一行的起始字符索引小于等于char_index，说明要找的行在当前行之后
        left = mid + 1;
      } else if (logical_line.start_char > char_index) {
        // 如果当前行的起始字符索引大于char_index，说明要找的行在当前行之前
        if (mid == 0) {
          break;
        }
        right = mid - 1;
      } else {
        target_line = mid;
        break;
      }
    }
    // 边界处理
    if (left < m_logical_lines_.size()) {
      target_line = left;
    }
    if (target_line >= m_logical_lines_.size()) {
      target_line = m_logical_lines_.size() - 1;
    }
    return target_line;
  }

  size_t Document::getByteLengthOfLine(size_t line) const {
    const size_t size = m_logical_lines_.size();
    if (line >= size) {
      throw std::out_of_range("Document::getLineByteLength line index out of range");
    }
    if (line == size - 1) {
      return m_total_bytes_ - m_logical_lines_[size - 1].start_byte;
    } else {
      return m_logical_lines_[line + 1].start_byte - m_logical_lines_[line].start_byte;
    }
  }

  inline const char* Document::getSegmentData(const BufferSegment& segment) const {
    return (segment.type == SegmentType::ORIGINAL)
             ? m_original_buffer_->data() + segment.start_byte
             : m_edit_buffer_->data() + segment.start_byte;
  }
}
