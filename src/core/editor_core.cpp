//
// Created by Scave on 2025/12/1.
//
#include "editor_core.h"

#include "logging.h"

namespace NS_SWEETEDITOR {
#ifdef SWEETEDITOR_DEBUG
  U8String EditorConfig::dump() const {
    return "EditorConfig {touch_config = " + touch_config.dump() + "}";
  }
#endif

  EditorCore::EditorCore(const EditorConfig& config): m_config_(config) {
    m_gesture_handler_ = makeUPtr<GestureHandler>(config.touch_config);
#ifdef SWEETEDITOR_DEBUG
    LOGD("EditorCore::EditorCore(), config = %s", config.dump().c_str());
#endif
  }

  void EditorCore::loadDocument(const Ptr<Document>& document) {
    m_document_ = document;
  }

  GestureResult EditorCore::handleGestureEvent(const GestureEvent& event) {
    return m_gesture_handler_->handleGestureEvent(event);
  }
}
