//
// Created by Scave on 2025/12/1.
//
#include "editor_core.h"
#include "logging.h"

namespace NS_SWEETEDITOR {
  U8String EditorConfig::dump() const {
    return "EditorConfig {touch_config = " + touch_config.dump() + "}";
  }

  EditorCore::EditorCore(const EditorConfig& config, const Ptr<TextMeasurer>& measurer): m_config_(config), m_measurer_(measurer) {
    m_gesture_handler_ = makeUPtr<GestureHandler>(config.touch_config);
    m_text_layout_ = makeUPtr<TextLayout>(measurer);
    LOGD("EditorCore::EditorCore(), config = %s", config.dump().c_str());
  }

  void EditorCore::loadDocument(const Ptr<Document>& document) {
    m_document_ = document;
    m_text_layout_->loadDocument(document);
    LOGD("EditorCore::loadDocument()");
  }

  GestureResult EditorCore::handleGestureEvent(const GestureEvent& event) {
    GestureResult result = m_gesture_handler_->handleGestureEvent(event);
    switch (result.type) {
    case GestureType::TAP:
      // 重新定位光标
      break;
    case GestureType::DOUBLE_TAP:
      // 选中文本
      break;
    case GestureType::SCALE:
      // 缩放编辑器
      m_view_state_.scale = std::max(1.0f, std::min(m_config_.max_scale, m_view_state_.scale * result.scale));
      break;
    case GestureType::SCROLL:
    case GestureType::FAST_SCROLL:
      // 滚动到指定位置
      m_view_state_.scroll_x = std::max(0.0f, m_view_state_.scroll_x + result.scroll_x);
      m_view_state_.scroll_y = std::max(0.0f, m_view_state_.scroll_y + result.scroll_y);
      break;
    default:
      break;
    }
    m_text_layout_->setViewState(m_view_state_);
    LOGD("EditorCore::handleGestureEvent, m_view_state_ = %s", m_view_state_.dump().c_str());
    return result;
  }

  void EditorCore::resetMeasurer() {
    m_text_layout_->resetMeasurer();
  }

  void EditorCore::buildRenderModel(EditorRenderModel& model) {
    model.lines = std::move(m_text_layout_->composeVisibleVisualLines());
  }

  const U16String& EditorCore::getVisualRunText(int64_t run_text_id) const {
    return m_text_layout_->getTextById(run_text_id);
  }

  void EditorCore::setViewport(const Viewport& viewport) {
    m_viewport_ = viewport;
    m_text_layout_->setViewport(viewport);
    LOGD("EditorCore::setViewport, viewport = %s", m_viewport_.dump().c_str());
  }

  void EditorCore::setWrapMode(WrapMode mode) {
    m_text_layout_->setWrapMode(mode);
  }

  void EditorCore::setScale(float scale) {
    m_view_state_.scale = scale;
    m_text_layout_->setViewState(m_view_state_);
    LOGD("EditorCore::setScale, m_view_state_ = %s", m_view_state_.dump().c_str());
  }

  void EditorCore::scrollToLine(size_t line, ScrollBehavior behavior) {

  }

  void EditorCore::setScroll(float scroll_x, float scroll_y) {
    m_view_state_.scroll_x = scroll_x;
    m_view_state_.scroll_y = scroll_y;
    m_text_layout_->setViewState(m_view_state_);
    LOGD("EditorCore::setScroll, m_view_state_ = %s", m_view_state_.dump().c_str());
  }

  ViewState EditorCore::getViewState() {
    return m_view_state_;
  }
}
