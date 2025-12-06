# 项目结构
```
├── 3dparty 第三方C++库
├── docs 文档目录
├── platform 各平台C++接口桥接项目（Windows和Linux不需要）
    ├── Android Android平台SDK工程
    ├── Emscripten WASM SDK工程
    └── OHOS 鸿蒙 SDK工程
├── src C++源代码目录
    ├── core 核心代码
    └── include 对外开放的头文件
└── tests 单元测试目录
```

# 开发环境
## Visual Studio
1. 直接打开该工程即可

## Clion
1. 直接打开该工程即可

## Visual Studio Code
1. 如果是Windows系统，安装 [MinGW64](https://github.com/niXman/mingw-builds-binaries/releases) 和 [CMake](https://github.com/Kitware/CMake/releases)，需要注意的是Windows系统MinGW64要下载带`posix-seh-ucrt`字段的；如果是MacOS系统，安装 Xcode 和 cmake 即可
2. 打开该工程，安装C/C++ Extension Pack(包含CMake)
3. 按 Ctrl + F5 运行
   (如果无法运行，就使用CMake插件在IDE左侧添加的CMake选项运行)

## Android Studio
1. 打开 platform/Android 工程 即可

# C++命名规范
## 文件命名
文件名使用全小写命名, 可以包含下划线(_) 或连字符(-)。举例：
```
component.cpp
plugin_api.h

1> C++ 文件要以 .cpp 结尾, 头文件以 .h 结尾. 专门插入文本的文件则以 .inc 结尾
2> 内联函数直接放在.h文件中
```
## 类型命名
【类, 结构体, 类型定义 (typedef), 枚举, 类型模板参数 均为类型】，以大写字母开始, 每个单词首字母均大写, 不包含下划线。
举例:
```
// 类和结构体
class UrlTable { ...
class UrlTableTester { ...
struct UrlTableProperties { ...

// 类型定义
typedef hash_map<UrlTableProperties *, string> PropertiesMap;

// using 别名
using PropertiesMap = hash_map<UrlTableProperties *, string>;

// 枚举
enum UrlTableErrors { ...
```
## 变量命名
变量 (包括函数参数) 和数据成员名一律小写, 单词之间用下划线连接. 类的成员变量以'm_'开头, 类的公开变量和结构体内部的变量的就不用。
### 普通变量命名
```
string table_name;  // 好 - 用下划线.
string tablename;   // 好 - 全小写.

string tableName;  // 差 - 混合大小写
```
### 类数据成员变量
```
class TableInfo {
...
public:
   string name_end;     // 好
   string m_name_start_;  // 坏，public变量不加 m_
private:
   string m_table_name;  // 好 - 前面加 m_
   string tablename_;   // 差，没有加 m_
   static Pool<TableInfo>* m_pool_;  // 好，末尾下划线可选
};
```
### 结构体成员变量
```
struct UrlTableProperties {
   string name;
   int num_entries;
   static Pool<UrlTableProperties>* pool;
};
```
### 全局变量
对全局变量没有特别要求, 少用就好, 但如果要用, 可以用 g 或其它标志作为前缀, 以便更好的区分局部变量.
### 常量命名
声明为 constexpr 或 const 的变量, 或在程序运行期间其值始终保持不变的, 命名时以 “k” 开头, 大小写混合，举例：
```
const int kDaysInAWeek = 7;
```
## 函数命名
函数名的每个单词首字母小写 (即 “小驼峰变量名”)，没有下划线，举例：
```
addTableEntry()
deleteUrl()
openFileOrDie()
```
## 枚举命名
枚举的命名使用k+大驼峰命名或全大写命名。举例：
```
enum UrlTableErrors {
   kOk = 0,
   kErrorOutOfMemory,
   kErrorMalformedInput,
};
enum AlternateUrlTableErrors {
   OK = 0,
   OUT_OF_MEMORY = 1,
   MALFORMED_INPUT = 2,
};
```
## 宏命名
命名全部大写, 单词间使用下划线。举例：
```
#define ROUND(x) ...
#define PI_ROUNDED 3.0
```