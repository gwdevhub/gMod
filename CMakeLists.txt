cmake_minimum_required(VERSION 3.16)

set(CMAKE_GENERATOR_PLATFORM win32)

project(gMod)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if(NOT(CMAKE_SIZEOF_VOID_P EQUAL 4))
    message(FATAL_ERROR "You are configuring a non 32-bit build, this is not supported. Run cmake with `-A Win32`")
endif()

set(VERSION_MAJOR 1)
set(VERSION_MINOR 7)
set(VERSION_PATCH 0)
set(VERSION_TWEAK 1)

set(VERSION_RC "${CMAKE_CURRENT_BINARY_DIR}/version.rc")
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/version.rc.in" "${VERSION_RC}" @ONLY)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR}/bin/install)

add_compile_options(/MP /permissive-)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
find_package(libzippp CONFIG REQUIRED)
find_package(minhook CONFIG REQUIRED)
find_package(Microsoft.GSL CONFIG REQUIRED)
include(dxtk)

add_library(gMod SHARED)

file(GLOB SOURCES
    "header/*.h"
    "source/*.cpp"
    "modules/*.ixx"
    ${VERSION_RC}
    )

target_include_directories(gMod PRIVATE
    "header"
    ${CMAKE_INSTALL_PREFIX}/include
    )
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${SOURCES})
target_sources(gMod PRIVATE ${SOURCES})
target_compile_options(gMod PRIVATE /W4 /WX)

target_link_libraries(gMod PRIVATE
    dxguid
    libzippp::libzippp
    psapi
    minhook::minhook
    directxtex
    Microsoft.GSL::GSL
    )
target_link_options(gMod PRIVATE
    "$<$<CONFIG:DEBUG>:/NODEFAULTLIB:LIBCMT>"
	"/LARGEADDRESSAWARE"
    )
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
	
add_subdirectory(TpfConvert)
