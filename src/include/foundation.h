//
// Created by Scave on 2025/12/2.
//
#ifndef SWEETEDITOR_FOUNDATION_H
#define SWEETEDITOR_FOUNDATION_H

#include "macro.h"

namespace NS_SWEETEDITOR {
  /// 文本位置描述
  struct TextPosition {
    /// 文字所处行，起始为0
    size_t line {0};
    /// 文字所处列，起始为0
    size_t column {0};

    bool operator<(const TextPosition& other) const;
    bool operator==(const TextPosition& other) const;
#ifdef SWEETEDITOR_DEBUG
    void dump() const;
#endif
  };

  /// 文字的范围区间描述
  struct TextRange {
    TextPosition start;
    TextPosition end;

    bool operator==(const TextRange& other) const;
    bool contains(const TextPosition& pos) const;
#ifdef SWEETEDITOR_DEBUG
    void dump() const;
#endif
  };
}

#endif //SWEETEDITOR_FOUNDATION_H