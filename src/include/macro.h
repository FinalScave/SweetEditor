//
// Created by Scave on 2025/12/1.
//
#ifndef SWEETEDITOR_MACRO_H
#define SWEETEDITOR_MACRO_H

#include <list>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#define NS_SWEETEDITOR sweeteditor

template<typename T>
using Ptr = std::shared_ptr<T>;
template<typename T, typename... Args>
constexpr Ptr<T> makePtr(Args&&... args) {
  return std::make_shared<T>(std::forward<Args>(args)...);
}

template<typename T>
using UPtr = std::unique_ptr<T>;
template<typename T, typename... Args>
constexpr UPtr<T> makeUPtr(Args&&... args) {
  return std::make_unique<T>(std::forward<Args>(args)...);
}

template<typename T>
using WPtr = std::weak_ptr<T>;

template<typename T>
using Vector = std::vector<T>;
template<typename T>
using List = std::list<T>;
template<typename K, typename V, typename KeyHash = std::hash<K>, typename KeyEqualTo = std::equal_to<K>>
using HashMap = std::unordered_map<K, V, KeyHash, KeyEqualTo>;
template<typename T, typename Hash = std::hash<T>, typename EqualTo = std::equal_to<T>>
using HashSet = std::unordered_set<T, Hash, EqualTo>;

using U8String = std::string;
#ifdef _WIN32
using U16String = std::wstring;
#else
using U16String = std::u16string;
#endif

/// lambda或函数签名检查（允许返回类型转换）
template <typename T, typename Ret, typename... Args>
constexpr bool kIsLambdaOrFunc = std::is_invocable_r_v<Ret, T, Args...>;

/// lambda签名检查（允许返回类型转换）
template <typename T, typename Ret, typename... Args>
constexpr bool kIsLambdaWithSignature =
  std::is_class_v<T> && !std::is_function_v<T> && std::is_invocable_r_v<Ret, T, Args...>;

/// lambda签名检查（严格匹配返回类型）
template <typename T, typename Ret, typename... Args>
constexpr bool kIsLambdaWithExactSignature =
  std::is_class_v<T> && !std::is_function_v<T> && std::is_invocable_v<T, Args...>
  && std::is_same_v<std::invoke_result_t<T, Args...>, Ret>;

/// lambda签名检查（仅检查参数类型，不检查返回类型）
template <typename T, typename... Args>
constexpr bool kIsLambdaWithArgs =
  std::is_class_v<T> && !std::is_function_v<T> && std::is_invocable_v<T, Args...>;

#endif //SWEETEDITOR_MACRO_H