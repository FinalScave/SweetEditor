#ifndef SWEETEDITOR_JNIUTIL_H
#define SWEETEDITOR_JNIUTIL_H

#include <jni.h>
#include <memory>

template<typename T>
class JPtrHolder {
public:
  explicit JPtrHolder(const std::shared_ptr<T>& ptr): m_ptr_(ptr) {
  }

  std::shared_ptr<T>& get() {
    return m_ptr_;
  }
private:
  std::shared_ptr<T> m_ptr_;
};

template<typename T, typename... Args>
jlong makePtrHolderToJavaHandle(Args... args) {
  std::shared_ptr<T> ptr = std::make_shared<T>((args)...);
  JPtrHolder<T>* holder = new JPtrHolder<T>(ptr);
  return reinterpret_cast<jlong>(holder);
}

template<typename T>
jlong toJavaHandle(JPtrHolder<T>* holder) {
  return reinterpret_cast<jlong>(holder);
}

template<typename T>
jlong toJavaHandle(const std::shared_ptr<T>& ptr) {
  if (ptr == nullptr) {
    return 0;
  }
  JPtrHolder<T>* holder = new JPtrHolder<T>(ptr);
  return reinterpret_cast<jlong>(holder);
}

template<typename T>
JPtrHolder<T>* toNativePtrHolder(jlong handle) {
  if (handle == 0) {
    return nullptr;
  }
  return reinterpret_cast<JPtrHolder<T>*>(handle);
}

template<typename T>
std::shared_ptr<T> getNativePtrHolderValue(jlong handle) {
  if (handle == 0) {
    return nullptr;
  }
  JPtrHolder<T>* holder = reinterpret_cast<JPtrHolder<T>*>(handle);
  if (holder != nullptr) {
    return holder->get();
  } else {
    return nullptr;
  }
}

template<typename T>
void deleteNativePtrHolder(jlong handle) {
  if (handle == 0) {
    return;
  }
  JPtrHolder<T>* holder = reinterpret_cast<JPtrHolder<T>*>(handle);
  if (holder != nullptr) {
    delete holder;
  }
}

#endif //SWEETEDITOR_JNIUTIL_H
