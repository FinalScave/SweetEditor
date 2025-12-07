#ifndef SWEETEDITOR_JEDITOR_HPP
#define SWEETEDITOR_JEDITOR_HPP

#include <jni.h>
#include "jni_helper.h"
#include "editor_core.h"

using namespace NS_SWEETEDITOR;

// ====================================== DocumentJni ===========================================
class DocumentJni {
public:
  static jlong makeStringDocument(JNIEnv* env, jclass clazz, jstring text) {
    const char* content_str = env->GetStringUTFChars(text, JNI_FALSE);
    return makePtrHolderToJavaHandle<Document>(content_str);
  }

  static jlong makeFileDocument(JNIEnv* env, jclass clazz, jstring path) {
    const char* path_str = env->GetStringUTFChars(path, JNI_FALSE);
    UPtr<Buffer> buffer = makeUPtr<MappedFileBuffer>(path_str);
    Ptr<Document> document = makePtr<Document>(std::move(buffer));
    return toJavaHandle(document);
  }

  static void finalizeDocument(jlong handle) {
    deleteNativePtrHolder<Document>(handle);
  }

  static jstring getText(JNIEnv* env, jclass clazz, jlong handle) {
    Ptr<Document> document = getNativePtrHolderValue<Document>(handle);
    if (document == nullptr) {
      return env->NewStringUTF("");
    }
    return env->NewStringUTF(document->getU8Text().c_str());
  }

  static jint getLineCount(jlong handle) {
    Ptr<Document> document = getNativePtrHolderValue<Document>(handle);
    if (document == nullptr) {
      return 0;
    }
    return static_cast<jint>(document->getLineCount());
  }

  static jstring getLineText(JNIEnv* env, jclass clazz, jlong handle, jint line) {
    Ptr<Document> document = getNativePtrHolderValue<Document>(handle);
    if (document == nullptr) {
      return env->NewStringUTF("");
    }
    const U8String& line_text = document->getLineU8Text(line);
    return env->NewStringUTF(line_text.c_str());
  }

  static jlong getPositionFromCharIndex(jlong handle, jint index) {
    Ptr<Document> document = getNativePtrHolderValue<Document>(handle);
    if (document == nullptr) {
      return 0;
    }
    TextPosition position = document->getPositionFromCharIndex(index);
    jlong line = (jlong)position.line;
    jlong column = (jlong)position.column;
    return (line << 32) | (column & 0XFFFFFFFFLL);
  }

  static jint getCharIndexFromPosition(jlong handle, jlong position) {
    Ptr<Document> document = getNativePtrHolderValue<Document>(handle);
    if (document == nullptr) {
      return 0;
    }
    size_t line = (size_t)(jint)(position >> 32);
    size_t column = (size_t)(jint)(position & 0XFFFFFFFF);
    return (jint)document->getCharIndexFromPosition({line, column});
  }

  constexpr static const char *kJClassName = "com/qiplat/sweeteditor/core/Document";
  constexpr static const JNINativeMethod kJMethods[] = {
      {"nativeMakeStringDocument", "(Ljava/lang/String;)J", (void*) makeStringDocument},
      {"nativeMakeFileDocument", "(Ljava/lang/String;)J", (void*) makeStringDocument},
      {"nativeFinalizeDocument", "(J)V", (void*) makeStringDocument},
      {"nativeGetText", "(J)Ljava/lang/String;", (void*) getText},
      {"nativeGetLineCount", "(J)I", (void*) getLineCount},
      {"nativeGetLineText", "(JI)Ljava/lang/String;", (void*) getLineText},
      {"nativeCharIndexOfPosition", "(JJ)I", (void*) getCharIndexFromPosition},
      {"nativePositionOfCharIndex", "(JI)J", (void*) getPositionFromCharIndex},
  };

  static void RegisterMethods(JNIEnv *env) {
    jclass java_class = env->FindClass(kJClassName);
    env->RegisterNatives(java_class, kJMethods,
                         sizeof(kJMethods) / sizeof(JNINativeMethod));
  }
};

// ====================================== EditorCoreJni ===========================================
class EditorCoreJni {
public:
  static jlong makeEditorCore(jfloat touch_slop, jlong double_tap_timeout) {
    TouchConfig touch_config = {touch_slop, double_tap_timeout};
    EditorConfig editor_config = {touch_config};
    return makePtrHolderToJavaHandle<EditorCore>(editor_config);
  }

  static void finalizeEditorCore(jlong handle) {
    deleteNativePtrHolder<EditorCore>(handle);
  }

  static jobject handleGestureEvent(JNIEnv* env, jclass clazz, jlong handle, jint type, jint pointer_count, jfloatArray points) {
    Ptr<EditorCore> editor_core = getNativePtrHolderValue<EditorCore>(handle);
    if (editor_core == nullptr) {
      return NULL;
    }
    GestureEvent event;
    event.type = static_cast<EventType>(type);
    jfloat* points_arr = env->GetFloatArrayElements(points, JNI_FALSE);
    for (int i = 0; i < pointer_count; ++i) {
      event.points.push_back({points_arr[i * 2], points_arr[i * 2 + 1]});
    }
    GestureResult result = editor_core->handleGestureEvent(event);
    jint jint_type = static_cast<jint>(result.type);
    char* buffer = nullptr;
    char* buffer_ptr = nullptr;
    jint buffer_len = sizeof(jint);
    switch (result.type) {
      case GestureType::TAP:
      case GestureType::DOUBLE_TAP:
      case GestureType::LONG_PRESS: {
        buffer_len += sizeof(jfloat) * 2;
        buffer = static_cast<char*>(malloc(buffer_len));
        buffer_ptr = buffer;
        memcpy(buffer_ptr, &jint_type, sizeof(jint));
        buffer_ptr += sizeof(jint);
        memcpy(buffer_ptr, &result.tap_point.x, sizeof(jfloat));
        buffer_ptr += sizeof(jfloat);
        memcpy(buffer_ptr, &result.tap_point.y, sizeof(jfloat));
        buffer_ptr += sizeof(jfloat);
        break;
      }
      case GestureType::SCALE:
        buffer_len += sizeof(jfloat);
        buffer = static_cast<char*>(malloc(buffer_len));
        buffer_ptr = buffer;
        memcpy(buffer_ptr, &jint_type, sizeof(jint));
        buffer_ptr += sizeof(jint);
        memcpy(buffer_ptr, &result.scale, sizeof(jfloat));
        buffer_ptr += sizeof(jfloat);
        break;
      case GestureType::SCROLL:
      case GestureType::FAST_SCROLL:
        buffer_len += sizeof(jfloat) * 2;
        buffer = static_cast<char*>(malloc(buffer_len));
        buffer_ptr = buffer;
        memcpy(buffer_ptr, &jint_type, sizeof(jint));
        buffer_ptr += sizeof(jint);
        memcpy(buffer_ptr, &result.scroll_x, sizeof(jfloat));
        buffer_ptr += sizeof(jfloat);
        memcpy(buffer_ptr, &result.scroll_y, sizeof(jfloat));
        buffer_ptr += sizeof(jfloat);
        break;
      default:
        buffer = static_cast<char*>(malloc(buffer_len));
        memcpy(buffer, &jint_type, sizeof(jint));
        break;
    }
    if (buffer != nullptr) {
      return env->NewDirectByteBuffer(buffer, buffer_len);
    } else {
      return NULL;
    }
  }

  constexpr static const char *kJClassName = "com/qiplat/sweeteditor/core/EditorCore";
  constexpr static const JNINativeMethod kJMethods[] = {
      {"nativeMakeEditorCore", "(FJ)J", (void*) makeEditorCore},
      {"nativeFinalizeEditorCore", "(J)V", (void*) finalizeEditorCore},
      {"nativeHandleGestureEvent", "(JII[F)Ljava/nio/ByteBuffer;", (void*) handleGestureEvent},
  };

  static void RegisterMethods(JNIEnv *env) {
    jclass java_class = env->FindClass(kJClassName);
    env->RegisterNatives(java_class, kJMethods,
                         sizeof(kJMethods) / sizeof(JNINativeMethod));
  }
};

#endif //SWEETEDITOR_JEDITOR_HPP
