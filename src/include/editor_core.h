//
// Created by Scave on 2025/12/1.
//
#ifndef SWEETEDITOR_EDITOR_CORE_H
#define SWEETEDITOR_EDITOR_CORE_H

#include "document.h"
#include "visual.h"
#include "gesture.h"

namespace NS_SWEETEDITOR {
  /// EditorCore初始化的一些配置
  struct EditorConfig {
    TouchConfig touch_config;
#ifdef SWEETEDITOR_DEBUG
    U8String dump() const;
#endif
  };

  /// 编辑器核心类
  class EditorCore {
  public:
    explicit EditorCore(const EditorConfig& config);

    /// 加载文本内容
    /// @param document Document实例
    void loadDocument(const Ptr<Document>& document);

    /// 处理手势事件
    /// @param event 手势数据
    /// @return 手势事件处理的结果
    GestureResult handleGestureEvent(const GestureEvent& event);
  private:
    Ptr<Document> m_document_;
    UPtr<GestureHandler> m_gesture_handler_;
    EditorConfig m_config_;
  };
}

#endif //SWEETEDITOR_EDITOR_CORE_H