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
    U8String dump() const;
  };

  /// 文字的范围区间描述
  struct TextRange {
    TextPosition start;
    TextPosition end;

    bool operator==(const TextRange& other) const;
    bool contains(const TextPosition& pos) const;
    U8String dump() const;
  };

  /// 横纵坐标数据包装
  struct PointF {
    float x {0};
    float y {0};

    float distance(const PointF& other) const;
    U8String dump() const;
  };

  /// 编辑器区域定义
  struct Viewport {
    /// 编辑器宽度
    float width {0};
    /// 编辑器高度
    float height {0};

    bool valid() const;
    U8String dump() const;
  };

  /// 编辑器状态定义
  struct ViewState {
    /// 缩放系数
    float scale {1};
    /// 水平滚动长度
    float scroll_x {0};
    /// 垂直滚动长度
    float scroll_y {0};

    U8String dump() const;
  };
}

#endif //SWEETEDITOR_FOUNDATION_H