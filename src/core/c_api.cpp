//
// Created by Scave on 2025/12/8.
//
#ifdef _WIN32
#include <Windows.h>
#include <DbgHelp.h>
#pragma comment(lib, "DbgHelp.lib")
#endif

#include "utility.h"
#include "c_api.h"
#include "editor_core.h"

template<typename T>
class CPtrHolder {
public:
  explicit CPtrHolder(const Ptr<T>& ptr): m_ptr_(ptr) {
  }

  Ptr<T>& get() {
    return m_ptr_;
  }
private:
  Ptr<T> m_ptr_;
};

template<typename T, typename... Args>
intptr_t makeCPtrHolderToIntPtr(Args... args) {
  Ptr<T> ptr = std::make_shared<T>((args)...);
  CPtrHolder<T>* holder = new CPtrHolder<T>(ptr);
  return reinterpret_cast<intptr_t>(holder);
}

template<typename T>
intptr_t toIntPtr(CPtrHolder<T>* holder) {
  return reinterpret_cast<intptr_t>(holder);
}

template<typename T>
intptr_t toIntPtr(const Ptr<T>& ptr) {
  if (ptr == nullptr) {
    return 0;
  }
  CPtrHolder<T>* holder = new CPtrHolder<T>(ptr);
  return reinterpret_cast<intptr_t>(holder);
}

template<typename T>
CPtrHolder<T>* toCPtrHolder(intptr_t handle) {
  if (handle == 0) {
    return nullptr;
  }
  return reinterpret_cast<CPtrHolder<T>*>(handle);
}

template<typename T>
Ptr<T> getCPtrHolderValue(intptr_t handle) {
  if (handle == 0) {
    return nullptr;
  }
  CPtrHolder<T>* holder = reinterpret_cast<CPtrHolder<T>*>(handle);
  if (holder != nullptr) {
    return holder->get();
  } else {
    return nullptr;
  }
}

template<typename T>
void deleteCPtrHolder(intptr_t handle) {
  if (handle == 0) {
    return;
  }
  CPtrHolder<T>* holder = reinterpret_cast<CPtrHolder<T>*>(handle);
  if (holder != nullptr) {
    delete holder;
  }
}

using namespace NS_SWEETEDITOR;

class CTextMeasurer : public TextMeasurer {
public:
  explicit CTextMeasurer(MeasureTextWidth measure_func, GetFontMetrics metrics_func): m_measurer_func_(measure_func), m_metrics_func_(metrics_func) {
  }

  float measureWidth(const U16String& text, uint32_t style_id) override {
    if (m_measurer_func_ == nullptr) {
      return 0;
    }
    return m_measurer_func_(text.c_str(), style_id);
  }

  FontMetrics getFontMetrics() override {
    if (m_measurer_func_ == nullptr) {
      return {0, 0};
    }
    float arr[2];
    m_metrics_func_(arr, 2);
    return {arr[0], arr[1]};
  }
private:
  MeasureTextWidth m_measurer_func_;
  GetFontMetrics m_metrics_func_;
};

extern "C" {

intptr_t create_document_from_utf16(const U16Char* text) {
  return makeCPtrHolderToIntPtr<Document>(text);
}

intptr_t create_document_from_file(const char* path) {
  UPtr<Buffer> buffer = makeUPtr<MappedFileBuffer>(path);
  Ptr<Document> document = makePtr<Document>(std::move(buffer));
  return toIntPtr(document);
}

void free_document(intptr_t document_handle) {
  deleteCPtrHolder<Document>(document_handle);
}

const char* get_document_text(intptr_t document_handle) {
  Ptr<Document> document = getCPtrHolderValue<Document>(document_handle);
  if (document == nullptr) {
    return "";
  }
  U8String u8_text = document->getU8Text();
  char* result = new char[u8_text.size() + 1];
  std::strcpy(result, u8_text.c_str());
  return result;
}

size_t get_document_line_count(intptr_t document_handle) {
  Ptr<Document> document = getCPtrHolderValue<Document>(document_handle);
  if (document == nullptr) {
    return 0;
  }
  return document->getLineCount();
}

const U16Char* get_document_line_text(intptr_t document_handle, size_t line) {
  Ptr<Document> document = getCPtrHolderValue<Document>(document_handle);
  if (document == nullptr) {
    return CHAR16_NONE;
  }
  U16String u16_text = document->getLineU16Text(line);
  return StrUtil::allocU16Chars(u16_text);
}

intptr_t create_editor(float touch_slop, int64_t double_tap_timeout, MeasureTextWidth measurer_func, GetFontMetrics metrics_func) {
  Ptr<CTextMeasurer> c_measurer = makePtr<CTextMeasurer>(measurer_func, metrics_func);
  TouchConfig touch_config = {touch_slop, double_tap_timeout};
  EditorConfig config;
  config.touch_config = touch_config;
  Ptr<EditorCore> editor_core = makePtr<EditorCore>(config, c_measurer);
  return toIntPtr(editor_core);
}

void free_editor(intptr_t editor_handle) {
  deleteCPtrHolder<EditorCore>(editor_handle);
}

void set_editor_viewport(intptr_t editor_handle, int16_t width, int16_t height) {
  Ptr<EditorCore> editor_core = getCPtrHolderValue<EditorCore>(editor_handle);
  if (editor_core == nullptr) {
    return;
  }
  editor_core->setViewport({(float)width, (float)height});
}

void set_editor_document(intptr_t editor_handle, intptr_t document_handle) {
  Ptr<EditorCore> editor_core = getCPtrHolderValue<EditorCore>(editor_handle);
  if (editor_core == nullptr) {
    return;
  }
  Ptr<Document> document = getCPtrHolderValue<Document>(document_handle);
  if (document == nullptr) {
    return;
  }
  editor_core->loadDocument(document);
}

const U16Char* handle_editor_gesture_event(intptr_t editor_handle, uint8_t type, uint8_t pointer_count,
    float* points) {
  Ptr<EditorCore> editor_core = getCPtrHolderValue<EditorCore>(editor_handle);
  if (editor_core == nullptr) {
    return CHAR16_NONE;
  }
  GestureEvent event;
  event.type = static_cast<EventType>(type);
  for (int i = 0; i < pointer_count; i++) {
    event.points.push_back({points[i * 2], points[i * 2 + 1]});
  }
  GestureResult result = editor_core->handleGestureEvent(event);
  nlohmann::json json = result;
  U8String u8_text = json.dump(2);
  U16Char* u16_json;
  StrUtil::convertUTF8ToUTF16(u8_text, &u16_json);
  return u16_json;
}

void reset_editor_text_measurer(intptr_t editor_handle) {
  Ptr<EditorCore> editor_core = getCPtrHolderValue<EditorCore>(editor_handle);
  if (editor_core == nullptr) {
    return;
  }
  editor_core->resetMeasurer();
}

const U16Char* build_editor_render_model(intptr_t editor_handle) {
  Ptr<EditorCore> editor_core = getCPtrHolderValue<EditorCore>(editor_handle);
  if (editor_core == nullptr) {
    return CHAR16_NONE;
  }
  EditorRenderModel model;
  editor_core->buildRenderModel(model);
  U8String u8_text = model.toJson();
  U16Char* result;
  StrUtil::convertUTF8ToUTF16(u8_text, &result);
  return result;
}

const U16Char* get_editor_visual_run_text(intptr_t editor_handle, int64_t run_text_id) {
  Ptr<EditorCore> editor_core = getCPtrHolderValue<EditorCore>(editor_handle);
  if (editor_core == nullptr) {
    return CHAR16_NONE;
  }
  U16String u16_text = editor_core->getVisualRunText(run_text_id);
  return StrUtil::allocU16Chars(u16_text);
}

const U16Char* get_editor_params(intptr_t editor_handle) {
  Ptr<EditorCore> editor_core = getCPtrHolderValue<EditorCore>(editor_handle);
  if (editor_core == nullptr) {
    return CHAR16_NONE;
  }
  U8String u8_text = editor_core->getEditorParams().toJson();
  U16Char* result;
  StrUtil::convertUTF8ToUTF16(u8_text, &result);
  return result;
}

void free_u16_string(intptr_t string_ptr) {
  const U16Char* ptr = reinterpret_cast<const U16Char*>(string_ptr);
  delete[] ptr;
}

#ifdef _WIN32
LONG WINAPI MyUnhandledExceptionFilter(PEXCEPTION_POINTERS pExceptionInfo) {
  HANDLE hFile = CreateFileW(L"SweetEditor_Crash.dmp",
                            GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
                            FILE_ATTRIBUTE_NORMAL, NULL);
  if (hFile != INVALID_HANDLE_VALUE) {
    MINIDUMP_EXCEPTION_INFORMATION dumpInfo;
    dumpInfo.ThreadId = GetCurrentThreadId();
    dumpInfo.ExceptionPointers = pExceptionInfo;
    dumpInfo.ClientPointers = FALSE;
    MiniDumpWriteDump(GetCurrentProcess(),
                     GetCurrentProcessId(),
                     hFile,
                     MiniDumpNormal,
                     &dumpInfo,
                     NULL,
                     NULL);
    CloseHandle(hFile);
  }
  return EXCEPTION_EXECUTE_HANDLER;
}

void init_unhandled_exception_handler() {
  SetUnhandledExceptionFilter(MyUnhandledExceptionFilter);
}
#endif

}