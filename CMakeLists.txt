cmake_minimum_required(VERSION 3.10)
project(sweeteditor)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# compiler special
if (MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zc:__cplusplus /utf-8")
endif ()

# options
option(BUILD_TESTING "Includes testing source for unit tests" ON)
add_definitions(-DTESTS_DIR="${CMAKE_SOURCE_DIR}/tests")
add_definitions(-DSWEETEDITOR_DEBUG=1)

# initial config
set(LINK_LIB)
set(CMAKE_PROJECT_DIR ${CMAKE_SOURCE_DIR})
set(SRC_DIR ${CMAKE_PROJECT_DIR}/src)
set(3DPARTY_DIR ${CMAKE_PROJECT_DIR}/3dparty)

# header search path
list(APPEND INCLUDE_DIRS
        ${SRC_DIR}/include
        ${3DPARTY_DIR}/include
        ${3DPARTY_DIR}/include/simdutf
)
include_directories(${INCLUDE_DIRS})

# core source files
file(GLOB_RECURSE CORE_SOURCE_FILES ${SRC_DIR}/core/*.*)

# platform special
set(PLATFORM_FILES)
if (ANDROID)
    #file(GLOB_RECURSE PLATFORM_FILES ${CMAKE_PROJECT_DIR}/platform/Android/sweeteditor/src/main/cpp/*.*)
    add_definitions(-DANDROID)
elseif (OHOS)
    #file(GLOB_RECURSE PLATFORM_FILES ${CMAKE_PROJECT_DIR}/platform/OHOS/sweeteditor/src/main/cpp/*.*)
    add_definitions(-DOHOS)
elseif (WIN32)
    add_definitions(-DWINDOWS -D_POSIX -D_POSIX_THREAD_SAFE_FUNCTIONS)
    set(STATIC_LIB ON)
elseif (EMSCRIPTEN)
    add_definitions(-DWASM)
    set(STATIC_LIB ON)
    #file(GLOB_RECURSE WASM_FILES ${CMAKE_PROJECT_DIR}/platform/Emscripten/*.*)
    message(STATUS "add target: libsweeteditor for wasm")
    add_executable("libsweeteditor" ${WASM_FILES})
    target_link_libraries("libsweeteditor" PRIVATE ${CMAKE_PROJECT_NAME}
            "--bind"
            "-s WASM=1"
            "-s MODULARIZE=1"
            "-s EXPORT_NAME='sweeteditor'"
            "-s EXPORT_ES6=1"
            "-s FORCE_FILESYSTEM=1"
            "-s ALLOW_MEMORY_GROWTH=1"
            "-s EXPORTED_RUNTIME_METHODS=[\"FS\"]"
    )
endif ()

# project shared/dynamic lib
if (STATIC_LIB)
    add_definitions(-DONIG_STATIC=ON)
    message(STATUS "add target: sweeteditor as static lib")
    add_library(${CMAKE_PROJECT_NAME} STATIC ${CORE_SOURCE_FILES} ${PLATFORM_FILES})
else ()
    message(STATUS "add target: sweeteditor as dynamic lib")
    add_library(${CMAKE_PROJECT_NAME} SHARED ${CORE_SOURCE_FILES} ${PLATFORM_FILES})
endif ()

# add library functions
function(add_platform_library dir filename type)
    if (${type} STREQUAL "STATIC")
        add_library(${dir} STATIC IMPORTED)
    elseif (${type} STREQUAL "SHARED")
        add_library(${dir} SHARED IMPORTED)
    else ()
        message(FATAL_ERROR "Error: add_platform_library type must be STATIC/SHARED")
    endif ()
    set(LIB_PATH)
    if (ANDROID)
        set(LIB_PATH ${3DPARTY_DIR}/lib/${dir}/android/${CMAKE_ANDROID_ARCH_ABI}/${filename})
    elseif (OHOS)
        set(LIB_PATH ${3DPARTY_DIR}/lib/${dir}/ohos/${OHOS_ARCH}/${filename})
    elseif (WIN32)
        if (MSVC)
            if(CMAKE_BUILD_TYPE STREQUAL "Release")
                set(LIB_PATH ${3DPARTY_DIR}/lib/${dir}/windows/msvc/release/${filename})
            else ()
                set(LIB_PATH ${3DPARTY_DIR}/lib/${dir}/windows/msvc/debug/${filename})
            endif ()
        else ()
            set(LIB_PATH ${3DPARTY_DIR}/lib/${dir}/windows/mingw64/${filename})
        endif ()
    elseif (EMSCRIPTEN)
        set(LIB_PATH ${3DPARTY_DIR}/lib/${dir}/emscripten/${filename})
    endif ()
    message(STATUS "Add platform library: ${LIB_PATH}")
    set_target_properties(${dir} PROPERTIES IMPORTED_LOCATION ${LIB_PATH})
    set(LINK_LIB ${LINK_LIB} ${dir} PARENT_SCOPE)
endfunction()

# add library
if (ANDROID)
    find_library(ANDROID_LIB android)
    find_library(LOG_LIB log)
    add_platform_library(simdutf libsimdutf.a STATIC)
    set(LINK_LIB ${LINK_LIB} ${ANDROID_LIB} ${LOG_LIB})
elseif (OHOS)
    add_platform_library(simdutf libsimdutf.a STATIC)
    set(LINK_LIB ${LINK_LIB} libace_napi.z.so libhilog_ndk.z.so)
elseif (WIN32)
    add_platform_library(simdutf libsimdutf.a STATIC)
    set(LINK_LIB ${LINK_LIB} ws2_32)
elseif (EMSCRIPTEN)
    add_platform_library(simdutf libsimdutf.a STATIC)
endif ()
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE ${LINK_LIB})

# Unit tests
if (BUILD_TESTING)
    message(STATUS "Unit tests enabled")
    if (NOT ANDROID AND NOT OHOS AND NOT EMSCRIPTEN)
        add_subdirectory(${CMAKE_PROJECT_DIR}/tests)
    endif ()
endif()
