//
// Created by Scave on 2025/12/10.
//
#include "decoration.h"

namespace NS_SWEETEDITOR {
  DecorationManager::DecorationManager() {
    m_style_reg_ = makePtr<StyleRegistry>();
  }

  Ptr<StyleRegistry> DecorationManager::getStyleRegistry() {
    return m_style_reg_;
  }
}
