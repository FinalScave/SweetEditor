//
// Created by Scave on 2025/12/1.
//
#ifndef SWEETEDITOR_EDITOR_CORE_H
#define SWEETEDITOR_EDITOR_CORE_H

#include "document.h"
#include "visual.h"
#include "gesture.h"
#include "layout.h"

namespace NS_SWEETEDITOR {
  /// EditorCore初始化的一些配置
  struct EditorConfig {
    TouchConfig touch_config;
    float max_scale {5};

    U8String dump() const;
  };

  enum struct ScrollBehavior {
    /// 首行在顶部可见
    GOTO_TOP,
    /// 首行滚动到中间
    GOTO_CENTER,
    /// 首行滚动到底部
    GOTO_BOTTOM,
  };

  /// 编辑器核心类
  class EditorCore {
  public:
    explicit EditorCore(const EditorConfig& config, const Ptr<TextMeasurer>& measurer);

    /// 加载文本内容
    /// @param document Document实例
    void loadDocument(const Ptr<Document>& document);

    /// 处理手势事件
    /// @param event 手势数据
    /// @return 手势事件处理的结果
    GestureResult handleGestureEvent(const GestureEvent& event);

    /// 重置文本测量，一般在编辑器重新设置字体的时候调用
    void resetMeasurer();

    /// 构建编辑器渲染模型
    /// @param model 传入的 EditorRenderModel
    void buildRenderModel(EditorRenderModel& model);

    /// 获取视觉文本片段id对应的文本
    /// @param run_text_id 片段id
    /// @return UTF8文本
    const U8String& getVisualRunText(int64_t run_text_id) const;

    /// 设置编辑器视口大小
    /// @param viewport 视口区域
    void setViewport(const Viewport& viewport);

    /// 设置自动换行模式
    /// @param mode WrapMode
    void setWrapMode(WrapMode mode);

    /// 手动设置编辑器缩放系数
    /// @param scale 缩放系数
    void setScale(float scale);

    /// 滚动到指定行
    /// @param line 行号
    /// @param behavior 滚动的形式
    void scrollToLine(size_t line, ScrollBehavior behavior);

    /// 手动设置编辑器滚动长度
    /// @param scroll_x 水平方向上滚动长度
    /// @param scroll_y 垂直方向上滚动长度
    void setScroll(float scroll_x, float scroll_y);

    /// 获取编辑器当前状态，包含缩放，滚动等数据
    ViewState getViewState();
  private:
    EditorConfig m_config_;
    Ptr<TextMeasurer> m_measurer_;
    Ptr<Document> m_document_;
    UPtr<GestureHandler> m_gesture_handler_;
    UPtr<TextLayout> m_text_layout_;
    // 每个逻辑行渲染高度的缓存
    HashMap<size_t, float> m_line_heights_;

    Viewport m_viewport_;
    ViewState m_view_state_;
  };
}

#endif //SWEETEDITOR_EDITOR_CORE_H