//
// Created by Scave on 2025/12/2.
//

#ifndef SWEETEDITOR_DOCUMENT_H
#define SWEETEDITOR_DOCUMENT_H

#include <cstdint>
#include "foundation.h"
#include "buffer.h"

namespace NS_SWEETEDITOR {
	/// 文本片段的类型
  enum struct SegmentType {
    ORIGINAL,
    EDITED
	};

	/// Buffer的文本片段
  struct BufferSegment {
    /// 用于标识是否属于Document的原始文本
    SegmentType type {SegmentType::ORIGINAL};
    /// 在buffer中起始字节位置
    size_t start_byte {0};
    /// 字节长度
    size_t byte_length {0};
  };

  /// 逻辑行的数据快照(标记dirty后随时刷新)
  struct LogicalLine {
    /// 当前行在全文中的起始字节偏移，文本变动时即更新
    size_t start_byte {0};
    /// 当前行在全文中的起始字符偏移，dirty时更新
    size_t start_char {0};
    /// 当前行文本的缓存（不包括换行符），dirty时更新
    U16String cached_text;
    /// 当前行文本数据是否已经被标记为dirty，需要刷新
    bool is_char_dirty {false};
    /// 当前行的渲染高度
    float height {-1};
  };

  /// 编辑器的文本对象
  class Document {
  public:
    explicit Document(U8String&& original_string);
    explicit Document(const U8String& original_string);
    explicit Document(const U16String& original_string);
    explicit Document(UPtr<Buffer>&& original_buffer);

    virtual ~Document();

    /// 获取当前文档的全部文本内容（UTF8编码）
    virtual U8String getU8Text();

    /// 获取当前文档的全部文本内容（UTF16编码）
    virtual U16String getU16Text();

    /// 获取当前文档的总行数
    /// @return 总行数
    size_t getLineCount() const;

    /// 获取指定行的UTF8文本
    /// @param line 行号
    /// @return 指定行的文本内容
    U16String getLineU16Text(size_t line) const;

    /// 获取指定行的column数量（字符数）
    /// @param line 行号
    /// @return 指定行的字符总数
    uint32_t getLineColumns(size_t line);

    /// 获取字符索引对应的行列位置
    /// @param char_index 字符索引
    /// @return 行列位置
    TextPosition getPositionFromCharIndex(size_t char_index) const;

    /// 获取指定行列位置对应的字符索引
    /// @param position 行列位置
    /// @return 字符索引
    size_t getCharIndexFromPosition(const TextPosition& position);

    /// 在指定位置处插入UTF8文本
    /// @param position 插入文本的位置
    /// @param text 插入的内容
    void insertU8Text(const TextPosition& position, const U8String& text);

    /// 删除指定范围内的文本
    /// @param range 删除的范围
    void deleteU8Text(const TextRange& range);

    /// 替换指定范围内的文本
    /// @param range 范围
    /// @param text 替换后的文本
    void replaceU8Text(const TextRange& range, const U8String& text);

    /// 计算在文本中指定区域有多少字符
    /// @param start_byte 起始字节偏移
    /// @param byte_length 字节长度
    /// @return 字符数量
    size_t countChars(size_t start_byte, size_t byte_length) const;

    /// 获取所有逻辑行数据
    Vector<LogicalLine>& getLogicalLines();

    /// 更新被标记为dirty的行
    /// @param index 行号
    /// @param logical_line 逻辑行数据
    void updateDirtyLine(size_t index, LogicalLine& logical_line);
  protected:
		/// 原始内容的Buffer（只读）
    UPtr<Buffer> m_original_buffer_;
    /// 用于用户编辑的文本记录，只增加不删除
    UPtr<U8StringBuffer> m_edit_buffer_;
    /// 所有的文本片段
    Vector<BufferSegment> m_buffer_segments_;
    /// 逻辑行的数据
    Vector<LogicalLine> m_logical_lines_;
    /// 全文的字节长度
    size_t m_total_bytes_ {0};
  private:
    void rebuildBufferSegments();
    void rebuildLogicalLines();
    U8String getU8Text(size_t start_byte, size_t byte_length) const;
    void insertU8Text(size_t start_byte, const U8String& text);
    void deleteU8Text(size_t start_byte, size_t byte_length);
    void updateLogicalLinesByInsertText(size_t start_byte, const U8String& text);
    void updateLogicalLinesByDeleteText(size_t start_byte, size_t byte_length);
    size_t getByteOffsetFromPosition(const TextPosition& position) const;
    size_t getLineFromByteOffset(size_t byte_offset) const;
    size_t getLineFromCharIndex(size_t char_index) const;
    size_t getByteLengthOfLine(size_t line) const;
    const char* getSegmentData(const BufferSegment& segment) const;
  };
}

#endif //SWEETEDITOR_DOCUMENT_H