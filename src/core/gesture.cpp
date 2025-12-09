//
// Created by Scave on 2025/12/6.
//

#include <cmath>
#include "gesture.h"
#include "utility.h"
#include "logging.h"

namespace NS_SWEETEDITOR {

  U8String TouchConfig::dump() const {
    return "TouchConfig {touch_slop = " + std::to_string(touch_slop) + ", double_tap_timeout = " + std::to_string(double_tap_timeout) + "}";
  }

  U8String GestureEvent::dump() const {
    return "GestureEvent {type = " + std::to_string(type) + ", point size = " + std::to_string(points.size()) + "}";
  }

  GestureEvent GestureEvent::create(const EventType type, const uint8_t pointer_count, const float* points) {
    GestureEvent event;
    event.type = type;
    for (uint8_t i = 0; i < pointer_count; i++) {
      event.points.push_back({points[i * 2], points [i * 2 + 1]});
    }
    return event;
  }

  GestureHandler::GestureHandler(const TouchConfig& config): m_config_(config) {
  }

  GestureResult GestureHandler::handleGestureEvent(const GestureEvent& event) {
    if (event.points.empty()) {
      LOGD("GestureHandler::handleGestureEvent, points empty");
      return {};
    }
    //LOGD("GestureHandler::handleGestureEvent, event: %s", event.dump().c_str());
    int64_t current_time = TimeUtil::milliTime();
    switch (event.type) {
    case EventType::MOUSE_DOWN:
      m_down_points_ = event.points;
      if (current_time - m_last_tap_time_ <= m_config_.double_tap_timeout
        && m_down_points_[0].distance(m_last_tap_point_) < m_config_.touch_slop) {
        m_is_tap_ = false;
        m_last_tap_time_ = 0;
        return {GestureType::DOUBLE_TAP, m_down_points_[0]};
      } else {
        m_last_tap_time_ = current_time;
        m_last_tap_point_ = m_down_points_[0];
        m_is_tap_ = true;
        return {GestureType::TAP, m_down_points_[0]};
      }
      break;
    case EventType::TOUCH_DOWN:
      m_down_points_ = event.points;
      m_down_time_ = current_time;
      m_last_move_point_ = m_down_points_[0];
      m_is_tap_ = true;
      break;
    case EventType::TOUCH_POINTER_DOWN:
      m_down_points_ = event.points;
      if (m_down_points_.size() > 1) {
        m_last_distance_ = m_down_points_[0].distance(m_down_points_[1]);
      }
      m_is_tap_ = false;
      break;
    case EventType::TOUCH_POINTER_UP:
      m_down_points_ = event.points;
      m_is_tap_ = false;
      break;
    case EventType::TOUCH_MOVE:
      if (m_down_points_.size() == 1) {
        const PointF& curr_point = event.points[0];
        // 单点触摸情况下，移动距离小于阈值判定为单击，大于阈值才判定为移动
        if (curr_point.distance(m_last_move_point_) > m_config_.touch_slop) {
          m_is_tap_ = false;
          float scroll_x = curr_point.x - m_last_move_point_.x;
          float scroll_y = curr_point.y - m_last_move_point_.y;
          m_last_move_point_ = curr_point;
          return {GestureType::SCROLL, {}, {}, -scroll_x, -scroll_y};
        }
      } else {
        m_is_tap_ = false;
        const PointF& down_point0 = m_down_points_[0];
        const PointF& down_point1 = m_down_points_[1];
        const PointF& curr_point0 = event.points[0];
        const PointF& curr_point1 = event.points[1];
        // 多点触摸情况下，两点相向而行为缩放，同向而行为快速滚动
        float delta_x0 = curr_point0.x - down_point0.x;
        float delta_y0 = curr_point0.y - down_point0.y;
        float delta_x1 = curr_point1.x - down_point1.x;
        float delta_y1 = curr_point1.y - down_point1.y;
        bool is_x_same_direction = (delta_x0 > 0 && delta_x1 > 0) || (delta_x0 < 0 && delta_x1 < 0);
        bool is_y_same_direction = (delta_y0 > 0 && delta_y1 > 0) || (delta_y0 < 0 && delta_y1 < 0);
        if (is_x_same_direction || is_y_same_direction) {
          // 快速滚动只能往一个方向滚
          float max_delta_x = std::max(delta_x0, delta_x1);
          float max_delta_y = std::max(delta_y0, delta_y1);
          if (max_delta_x > max_delta_y) {
            return {GestureType::FAST_SCROLL, {}, {}, -max_delta_x};
          } else {
            return {GestureType::FAST_SCROLL, {}, {}, {}, -max_delta_y};
          }
        } else {
          float curr_distance = curr_point0.distance(curr_point1);
          float scale = curr_distance / m_last_distance_;
          m_last_distance_ = curr_distance;
          return {GestureType::SCALE, {}, scale};
        }
      }
      break;
    case EventType::TOUCH_UP:
      if (m_is_tap_) {
        if (current_time - m_last_tap_time_ <= m_config_.double_tap_timeout
            && m_down_points_[0].distance(m_last_tap_point_) < m_config_.touch_slop) {
          m_is_tap_ = false;
          m_last_tap_time_ = 0;
          return {GestureType::DOUBLE_TAP, m_down_points_[0]};
        } else {
          if (current_time - m_down_time_ > m_config_.long_press_ms) {
            return {GestureType::LONG_PRESS, m_down_points_[0]};
          } else {
            m_last_tap_time_ = current_time;
            m_last_tap_point_ = m_down_points_[0];
            return {GestureType::TAP, m_last_tap_point_};
          }
        }
      }
      m_down_time_ = std::numeric_limits<int64_t>::max();
      break;
    default:
      break;
    }
    return {};
  }
}
