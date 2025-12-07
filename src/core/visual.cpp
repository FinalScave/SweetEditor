//
// Created by Scave on 2025/12/7.
//
#include "visual.h"

namespace NS_SWEETEDITOR {
  U8String dumpEnum(VisualRunType type) {
    switch (type) {
    case VisualRunType::TEXT:
      return "TEXT";
    case VisualRunType::WHITESPACE:
      return "WHITESPACE";
    case VisualRunType::NEWLINE:
      return "NEWLINE";
    case VisualRunType::INLAY_HINT:
      return "INLAY_HINT";
    case VisualRunType::PHANTOM_TEXT:
      return "PHANTOM_TEXT";
    default:
      return "UNDEFINED";
    }
  }

  U8String dumpEnum(GuideLineDirection direction) {
    switch (direction) {
    case GuideLineDirection::VERTICAL:
      return "VERTICAL";
    case GuideLineDirection::HORIZONTAL:
      return "HORIZONTAL";
    default:
      return "UNDEFINED";
    }
  }

  U8String VisualRun::dump() const {
    return "VisualRun {type = " + dumpEnum(type) + ", x = " + std::to_string(x) + ", y = " + std::to_string(y)
      + ", text_id = " + std::to_string(text_id) + ", style_id = " + std::to_string(style_id) + "}";
  }

  U8String VisualLine::dump() const {
    U8String result = "VisualLine {logical_line = " + std::to_string(logical_line) + ", runs = [";
    for (const VisualRun& run : runs) {
      result += "\n  ";
      result += run.dump();
    }
    result += "\n]\n}";
    return result;
  }

  U8String Cursor::dump() const {
    return "Cursor = {position = " + position.dump() + ", show_dragger = " + std::to_string(show_dragger) + "}";
  }

  U8String EditorRenderModel::dump() const {
    U8String result = "EditorRenderModel {current_line = " + current_line.dump() + ", visual_lines = [";
    for (const VisualLine& line : lines) {
      result += "\n  ";
      result += line.dump();
    }
    result += "\n], cursor = " + cursor.dump() + "}";
    return result;
  }

  U8String EditorRenderModel::toJson() const {
    nlohmann::json root = *this;
    return root.dump(2);
  }
}
