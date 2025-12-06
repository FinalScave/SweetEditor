# SweetEditor 代码编辑器

## 概述
SweetEditor 是一款跨平台、高性能、可扩展的代码编辑器，专为现代IDE设计。其采用C++作为计算核心，平台层进行渲染，SweetEditorCore可以单独使用，不与Editor View绑定
### SweetEditorCore
SweetEditorCore是编辑器的计算核心，负责文本处理、增量更新、计算渲染区域、计算渲染数据等核心功能，可作为独立库使用，不涉及任何UI操作

## 快速开始
### 基础使用
```c++
#include "editor_core.h"
```

## 高级功能
### 代码区块划线
```c++
// TODO: 代码区块划线
```

## 共建代码仓库
欢迎各位小伙伴一起共建 SweetEditor，如果您有参与项目的想法可直接拉分支修改并提交合并请求，项目协作说明可参见[项目协作说明](docs/join.md)