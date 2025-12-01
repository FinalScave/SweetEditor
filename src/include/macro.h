#ifndef SWEETEDITOR_MACRO_H
#define SWEETEDITOR_MACRO_H

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
using List = std::vector<T>;
template<typename K, typename V, typename KeyHash = std::hash<K>, typename KeyEqualTo = std::equal_to<K>>
using HashMap = std::unordered_map<K, V, KeyHash, KeyEqualTo>;
template<typename T, typename Hash = std::hash<T>, typename EqualTo = std::equal_to<T>>
using HashSet = std::unordered_set<T, Hash, EqualTo>;

using String = std::string;

#endif //SWEETEDITOR_MACRO_H