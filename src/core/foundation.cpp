//
// Created by Scave on 2025/12/2.
//
#ifdef SWEETEDITOR_DEBUG
#include <iostream>
#endif

#include <nlohmann/json.hpp>
#include "foundation.h"

namespace NS_SWEETEDITOR {
  NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(TextPosition, line, column);
  NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(TextRange, start, end);
  // ===================================== TextPosition ============================================
  bool TextPosition::operator<(const TextPosition& other) const {
    if (line != other.line) return line < other.line;
    return column < other.column;
  }

  bool TextPosition::operator==(const TextPosition& other) const {
    return line == other.line && column == other.column;
  }

#ifdef SWEETEDITOR_DEBUG
  void TextPosition::dump() const {
    const nlohmann::json json = *this;
    std::cout << json.dump(2) << std::endl;
  }
#endif

  // ===================================== TextRange ============================================
  bool TextRange::operator==(const TextRange& other) const {
    return start == other.start && end == other.end;
  }

  bool TextRange::contains(const TextPosition& pos) const {
    return !(pos < start) && (pos < end || pos == end);
  }

#ifdef SWEETEDITOR_DEBUG
  void TextRange::dump() const {
    const nlohmann::json json = *this;
    std::cout << json.dump(2) << std::endl;
  }
#endif
}