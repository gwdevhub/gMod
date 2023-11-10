project(gMod)

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    message(FATAL_ERROR "You are configuring a 64bit build, this is not supported. Run cmake with `-A Win32`")
endif()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(VERSION_MAJOR 1)
set(VERSION_MINOR 5)
set(VERSION_PATCH 2)
set(VERSION_TWEAK 0)

set(VERSION_RC "${CMAKE_CURRENT_BINARY_DIR}/version.rc")
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/version.rc.in" "${VERSION_RC}" @ONLY)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(uMod "${CMAKE_CURRENT_LIST_DIR}/")
set(DIRECTX_SDK_INCLUDE_DIR "$ENV{DXSDK_DIR}/Include")
set(DIRECTX_SDK_LIB_DIR "$ENV{DXSDK_DIR}/Lib/x86")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake/")

link_directories(${DIRECTX_SDK_LIB_DIR})
include_directories(${DIRECTX_SDK_INCLUDE_DIR})
add_library(uMod SHARED)
file(GLOB SOURCES
    "source/pch.h"
    "source/*.cpp"
    "header/*.h*"
    ${VERSION_RC})

find_library(D3D9_LIB NAMES d3d9 PATHS ${DIRECTX_SDK_LIB_DIR} NO_DEFAULT_PATH)
find_library(D3DX9_LIB NAMES d3dx9 PATHS ${DIRECTX_SDK_LIB_DIR} NO_DEFAULT_PATH)
find_library(DXGUID_LIB NAMES dxguid PATHS ${DIRECTX_SDK_LIB_DIR} NO_DEFAULT_PATH)

if(NOT D3D9_LIB)
  find_library(D3D9_LIB NAMES d3d9 PATHS ${CMAKE_SOURCE_DIR}/Lib)
endif()

if(NOT D3DX9_LIB)
  find_library(D3DX9_LIB NAMES d3dx9 PATHS ${CMAKE_SOURCE_DIR}/Lib)
endif()

if(NOT DXGUID_LIB)
  find_library(DXGUID_LIB NAMES dxguid PATHS ${CMAKE_SOURCE_DIR}/Lib)
endif()

target_link_libraries(uMod PRIVATE ${D3D9_LIB} ${D3DX9_LIB} ${DXGUID_LIB})

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${SOURCES})
target_sources(uMod PRIVATE ${SOURCES})
target_compile_features(uMod PUBLIC cxx_std_17)
target_compile_definitions(uMod PRIVATE
    $<$<CONFIG:Debug>:BUILD_TYPE_DEBUG>
    $<$<CONFIG:Release>:BUILD_TYPE_RELEASE>
    INJECTION_METHOD=2
    DIRECT_INJECTION
    DEF_USE_DX9=true
    DX_XX_File=9
    LOG_MESSAGE
)

target_precompile_headers(uMod PRIVATE "header/pch.h")
target_include_directories(uMod PUBLIC "header")