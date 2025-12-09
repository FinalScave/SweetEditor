//
// Created by Scave on 2025/12/6.
//

#ifndef SWEETEDITOR_GESTURE_H
#define SWEETEDITOR_GESTURE_H

#include <cstdint>
#include <limits>
#include <nlohmann/json.hpp>
#include "foundation.h"

namespace NS_SWEETEDITOR {
  /// 手势处理初始化的一些配置
  struct TouchConfig {
    /// 判定手势操作为移动的阈值，低于这个阈值判定为单击
    float touch_slop {10};
    /// 双击的时间阈值
    int64_t double_tap_timeout {300};
    /// 长按的时间阈值
    int64_t long_press_ms {500};

    U8String dump() const;
  };

  /// 手势操作类型定义
  enum EventType : uint8_t {
    /// 未定义手势
    UNDEFINED = 0,
    /// 按下
    TOUCH_DOWN = 1,
    /// 再按下一只手指
    TOUCH_POINTER_DOWN = 2,
    /// 移动
    TOUCH_MOVE = 3,
    /// 松开一只手指
    TOUCH_POINTER_UP = 4,
    /// 松开手指
    TOUCH_UP = 5,
    /// 取消事件
    TOUCH_CANCEL = 6,
    /// 鼠标按下
    MOUSE_DOWN = 7,
  };

  /// 平台层发送过来的手势操作数据封装
  struct GestureEvent {
    /// 手势操作类型
    EventType type;
    /// 操作点坐标
    Vector<PointF> points;

    static GestureEvent create(const EventType type, const uint8_t pointer_count, const float* points);
    U8String dump() const;
  };

  /// 手势处理的结果类型
  enum struct GestureType : uint8_t {
    /// 未定义行为
    UNDEFINED = 0,
    /// 单击
    TAP = 1,
    /// 双击
    DOUBLE_TAP = 2,
    /// 长按
    LONG_PRESS = 3,
    /// 缩放
    SCALE = 4,
    /// 滚动
    SCROLL = 5,
    /// 快速滚动
    FAST_SCROLL = 6,
  };

  /// 手势处理结果定义
  struct GestureResult {
    GestureType type {GestureType::UNDEFINED};
    PointF tap_point {};
    float scale {1};
    float scroll_x {0};
    float scroll_y {0};
  };

  /// 手势处理器类
  class GestureHandler {
  public:
    explicit GestureHandler(const TouchConfig& config);

    /// 处理手势事件
    /// @param event 平台侧传递的手势数据
    GestureResult handleGestureEvent(const GestureEvent& event);
  private:
    TouchConfig m_config_;
    Vector<PointF> m_down_points_;
    int64_t m_down_time_ {std::numeric_limits<int64_t>::max()};
    PointF m_last_move_point_;
    float m_last_distance_ {0};
    bool m_is_tap_ {false};
    PointF m_last_tap_point_;
    int64_t m_last_tap_time_ {0};
  };

  NLOHMANN_JSON_SERIALIZE_ENUM(EventType, {
    {EventType::UNDEFINED, "UNDEFINED"},
    {EventType::TOUCH_DOWN, "TOUCH_DOWN"},
    {EventType::TOUCH_POINTER_DOWN, "TOUCH_POINTER_DOWN"},
    {EventType::TOUCH_MOVE, "TOUCH_MOVE"},
    {EventType::TOUCH_POINTER_UP, "TOUCH_POINTER_UP"},
    {EventType::TOUCH_UP, "TOUCH_UP"},
    {EventType::TOUCH_CANCEL, "TOUCH_CANCEL"},
  })
  NLOHMANN_JSON_SERIALIZE_ENUM(GestureType, {
    {GestureType::UNDEFINED, "UNDEFINED"},
    {GestureType::TAP, "TAP"},
    {GestureType::DOUBLE_TAP, "DOUBLE_TAP"},
    {GestureType::LONG_PRESS, "LONG_PRESS"},
    {GestureType::SCALE, "SCALE"},
    {GestureType::SCROLL, "SCROLL"},
    {GestureType::FAST_SCROLL, "FAST_SCROLL"},
  })
  NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(GestureResult, type, tap_point, scale, scroll_x, scroll_y)
}

#endif //SWEETEDITOR_GESTURE_H