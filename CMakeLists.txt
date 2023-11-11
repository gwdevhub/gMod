cmake_minimum_required(VERSION 3.16)

set(CMAKE_GENERATOR_PLATFORM win32)

project(gMod)
set_property(GLOBAL PROPERTY USE_FOLDERS OFF)

if(NOT(CMAKE_SIZEOF_VOID_P EQUAL 4))
    message(FATAL_ERROR "You are configuring a non 32-bit build, this is not supported. Run cmake with `-A Win32`")
endif()

set(VERSION_MAJOR 1)
set(VERSION_MINOR 5)
set(VERSION_PATCH 3)
set(VERSION_TWEAK 0)

set(VERSION_RC "${CMAKE_CURRENT_BINARY_DIR}/version.rc")
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/version.rc.in" "${VERSION_RC}" @ONLY)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin/")
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

add_compile_options(/MP /permissive-)

find_library(D3D9_LIB NAMES d3d9 PATHS $ENV{DXSDK_DIR}/lib/x86 NO_DEFAULT_PATH)
find_library(D3DX9_LIB NAMES d3dx9 PATHS $ENV{DXSDK_DIR}/lib/x86 NO_DEFAULT_PATH)
find_library(DXGUID_LIB NAMES dxguid PATHS $ENV{DXSDK_DIR}/lib/x86 NO_DEFAULT_PATH)

if(NOT D3D9_LIB)
  find_library(D3D9_LIB NAMES d3d9 PATHS ${CMAKE_SOURCE_DIR}/Lib)
endif()

if(NOT D3DX9_LIB)
  find_library(D3DX9_LIB NAMES d3dx9 PATHS ${CMAKE_SOURCE_DIR}/Lib)
endif()

if(NOT DXGUID_LIB)
  find_library(DXGUID_LIB NAMES dxguid PATHS ${CMAKE_SOURCE_DIR}/Lib)
endif()

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
include(miniz)

add_library(gMod SHARED)

file(GLOB SOURCES
    "header/*.h"
    "source/*.cpp"
    ${VERSION_RC}
    )

target_link_libraries(gMod PRIVATE
    ${D3D9_LIB}
    ${D3DX9_LIB}
    ${DXGUID_LIB}
    miniz
    )
target_include_directories(gMod PRIVATE $ENV{DXSDK_DIR}/Include)
target_include_directories(gMod PUBLIC "header")
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${SOURCES})
target_sources(gMod PRIVATE ${SOURCES})
target_compile_definitions(gMod PRIVATE
    "NOMINMAX"
    "_WIN32_WINNT=_WIN32_WINNT_WIN7"
    "WIN32_LEAN_AND_MEAN"
    "VC_EXTRALEAN"
    DIRECT_INJECTION
    LOG_MESSAGE
)
