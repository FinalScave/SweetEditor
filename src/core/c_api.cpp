//
// Created by Scave on 2025/12/8.
//
#ifdef _WIN32
#include <Windows.h>
#include <DbgHelp.h>
#pragma comment(lib, "DbgHelp.lib")
#endif

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
  explicit CTextMeasurer(MeasureTextWidth func): m_func_(func) {
  }

  float measureWidth(const U8String& text, bool is_bold) override {
    if (m_func_ == nullptr) {
      return 0;
    }
    return m_func_(text.c_str(), is_bold);
  }

  float getFontHeight() override {
    return 20;
  }
private:
  MeasureTextWidth m_func_;
};

extern "C" {

intptr_t create_document_from_utf8(const char* text) {
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

const char* get_document_line_text(intptr_t document_handle, size_t line) {
  Ptr<Document> document = getCPtrHolderValue<Document>(document_handle);
  if (document == nullptr) {
    return "";
  }
  U8String u8_text = document->getLineU8Text(line);
  char* result = new char[u8_text.size() + 1];
  std::strcpy(result, u8_text.c_str());
  return result;
}

intptr_t create_editor(float touch_slop, int64_t double_tap_timeout, MeasureTextWidth text_measurer) {
  Ptr<CTextMeasurer> c_measurer = makePtr<CTextMeasurer>(text_measurer);
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

const char* handle_editor_gesture_event(intptr_t editor_handle, uint8_t type, uint8_t pointer_count,
    float* points) {
  Ptr<EditorCore> editor_core = getCPtrHolderValue<EditorCore>(editor_handle);
  if (editor_core == nullptr) {
    return "";
  }
  GestureEvent event;
  event.type = static_cast<EventType>(type);
  for (int i = 0; i < pointer_count; i++) {
    event.points.push_back({points[i * 2], points[i * 2 + 1]});
  }
  GestureResult result = editor_core->handleGestureEvent(event);
  nlohmann::json json = result;
  U8String u8_text = json.dump(2);
  char* json_str = new char[u8_text.size() + 1];
  std::strcpy(json_str, u8_text.c_str());
  return json_str;
}

void reset_editor_text_measurer(intptr_t editor_handle) {
  Ptr<EditorCore> editor_core = getCPtrHolderValue<EditorCore>(editor_handle);
  if (editor_core == nullptr) {
    return;
  }
  editor_core->resetMeasurer();
}

const char* build_editor_render_model(intptr_t editor_handle) {
  Ptr<EditorCore> editor_core = getCPtrHolderValue<EditorCore>(editor_handle);
  if (editor_core == nullptr) {
    return "";
  }
  EditorRenderModel model;
  editor_core->buildRenderModel(model);
  U8String u8_text = model.toJson();
  char* json_str = new char[u8_text.size() + 1];
  std::strcpy(json_str, u8_text.c_str());
  return json_str;
}

const char* get_editor_visual_run_text(intptr_t editor_handle, int64_t run_text_id) {
  Ptr<EditorCore> editor_core = getCPtrHolderValue<EditorCore>(editor_handle);
  if (editor_core == nullptr) {
    return "";
  }
  U8String u8_text = editor_core->getVisualRunText(run_text_id);
  char* result = new char[u8_text.size() + 1];
  std::strcpy(result, u8_text.c_str());
  return result;
}

void free_c_string(intptr_t string_ptr) {
  const char* ptr = reinterpret_cast<const char*>(string_ptr);
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

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved) {
  switch (ul_reason_for_call) {
  case DLL_PROCESS_ATTACH:
    SetUnhandledExceptionFilter(MyUnhandledExceptionFilter);
    break;
  case DLL_THREAD_ATTACH:
  case DLL_THREAD_DETACH:
  case DLL_PROCESS_DETACH:
    break;
  }
  return TRUE;
}
#endif

}