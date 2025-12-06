#include <catch2/catch_amalgamated.hpp>
#include <iostream>
#include "document.h"

using namespace NS_SWEETEDITOR;

const char* text = R"(
行1: 你好
行2: World
行3: 结束)";

TEST_CASE("Edit Text") {
  Document document(text);

  std::cout << "原始文本:" << std::endl;
  std::cout << document.getU8Text() << std::endl << std::endl;

  // 单行更新：替换 "你好" 为 "您不好"
  TextRange range = {{1, 4}, {1, 6}};
  document.replaceU8Text(range, "您不好");
  std::cout << "单行替换后:" << std::endl;
  std::cout << document.getU8Text() << std::endl;

  // 跨行更新：替换 "World\n行3" 为 "宇宙\n最后一行"
  range = {{2, 4}, {3, 2}};
  document.replaceU8Text(range, "宇宙\n最后一行");
  std::cout << "跨行替换后:" << std::endl;
  std::cout << document.getU8Text() << std::endl << std::endl;

  // 插入文本
  TextPosition position = {2, 1};
  document.insertU8Text(position, "=====");
  std::cout << "插入后:" << std::endl;
  std::cout << document.getU8Text() << std::endl << std::endl;

  // 删除文本
  range = {{1, 0}, {2, 9}};
  document.deleteU8Text(range);
  std::cout << "删除后:" << std::endl;
  std::cout << document.getU8Text() << std::endl;
}

TEST_CASE("Edit Benchmark") {
  BENCHMARK("Replace Performance") {
    Document document(text);
    TextRange range = {{1, 0}, {1, 1}};
    document.replaceU8Text(range, "H");
  };
}
