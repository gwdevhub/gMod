cmake_minimum_required(VERSION 3.16)

set(CMAKE_GENERATOR_PLATFORM win32)

project(gMod)

if(NOT(CMAKE_SIZEOF_VOID_P EQUAL 4))
    message(FATAL_ERROR "You are configuring a non 32-bit build, this is not supported. Run cmake with `-A Win32`")
endif()

set(VERSION "1.0")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

set_property(GLOBAL PROPERTY USE_FOLDERS OFF)

add_compile_options(/MP /permissive-)

find_library(D3D9_LIB NAMES d3d9 PATHS $ENV{DXSDK_DIR}/lib/x86 NO_DEFAULT_PATH)
find_library(D3DX9_LIB NAMES d3dx9 PATHS $ENV{DXSDK_DIR}/lib/x86 NO_DEFAULT_PATH)
find_library(DXGUID_LIB NAMES dxguid PATHS $ENV{DXSDK_DIR}/lib/x86 NO_DEFAULT_PATH)

add_library(gMod SHARED)

file(GLOB SOURCES
    "header/*.h"
    "source/*.cpp"
    ${VERSION_RC}
    )

target_link_libraries(gMod PRIVATE ${D3D9_LIB} ${D3DX9_LIB} ${DXGUID_LIB})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${SOURCES})
target_sources(gMod PRIVATE ${SOURCES})
target_compile_definitions(gMod PRIVATE
    "NOMINMAX"
    "_WIN32_WINNT=_WIN32_WINNT_WIN7"
    "WIN32_LEAN_AND_MEAN"
    "VC_EXTRALEAN"
    $<$<CONFIG:Debug>:BUILD_TYPE_DEBUG>
    $<$<CONFIG:Release>:BUILD_TYPE_RELEASE>
    INJECTION_METHOD=2
    DIRECT_INJECTION
    DEF_USE_DX9=true
    _UNICODE=true
    UNICODE=true
    DX_XX_File=9
    LOG_MESSAGE
)

target_precompile_headers(uMod PRIVATE "header/pch.h")
target_include_directories(uMod PUBLIC "header")