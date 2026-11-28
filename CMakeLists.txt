cmake_minimum_required(VERSION 3.30)

# import std (the C++23 standard library module) needs this experimental switch, set before project().
set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "f35a9ac6-8463-4d38-8eec-5d6008153e7d")

project(gMod)
enable_language(RC)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if(NOT(CMAKE_SIZEOF_VOID_P EQUAL 4))
    message(FATAL_ERROR "You are configuring a non 32-bit build, this is not supported. Use the llvm-mingw presets (i686).")
endif()

set(VERSION_MAJOR 1)
set(VERSION_MINOR 10)
set(VERSION_PATCH 2)
set(VERSION_TWEAK 0)

set(VERSION_RC "${CMAKE_CURRENT_BINARY_DIR}/version.rc")
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/version.rc.in" "${VERSION_RC}" @ONLY)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

find_package(libzippp CONFIG REQUIRED)
find_package(minhook CONFIG REQUIRED)
find_package(Microsoft.GSL CONFIG REQUIRED)
find_package(directxtex CONFIG REQUIRED)
# vcpkg's directxtex has no DDS/WIC texture loaders; these come from the same DirectXTex release, fetched by CMake.
include(directxtexloader)

# Drop unreferenced sections (the vcpkg ports and DirectXTex build with -ffunction-sections -fdata-sections) and fold
# identical code outside Debug. Exports are roots, so the DLL's entry points stay.
add_link_options(-Wl,--gc-sections $<$<NOT:$<CONFIG:Debug>>:-Wl,--icf=all>)

add_library(gMod SHARED)

file(GLOB SOURCES
    "header/*.h"
    "source/*.cpp"
    ${VERSION_RC}
    )
file(GLOB MODULE_SOURCES "modules/*.ixx")

target_include_directories(gMod PRIVATE
    "header"
    )
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${SOURCES} ${MODULE_SOURCES})
target_sources(gMod PRIVATE ${SOURCES})
# CMake only scans C++ modules declared in a CXX_MODULES file set.
target_sources(gMod PRIVATE
    FILE_SET CXX_MODULES
    FILES ${MODULE_SOURCES}
    )

# The C++ modules (modules/*.ixx) need the module scanner, and the module-aware standard library for import std.
set_target_properties(gMod PROPERTIES
    CXX_SCAN_FOR_MODULES ON
    CXX_MODULE_STD ON
    PREFIX ""  # MinGW would name the dll libgMod.dll; gMod.dll is the name the launchers expect
    )

# clang's warnings in place of MSVC's /W4 /WX.
# -gdwarf-4 keeps DWARF in the link so the split below can take it out; the PDB is written separately.
# -fno-exceptions: the DLL reports failures through return values and error_code, so no unwind runtime is needed.
target_compile_options(gMod PRIVATE -Wall -Wextra -gdwarf-4 -fno-exceptions)

# The DLL ships without DWARF. GWToolboxdll.debug-style split: the debug sections go to gMod.debug for GDB, and the
# DLL is stripped. The CodeView PDB (gMod.pdb) is unaffected.
find_program(GMOD_LLVM_OBJCOPY NAMES llvm-objcopy llvm-objcopy.exe HINTS "${LLVM_MINGW_ROOT}/bin" REQUIRED)
add_custom_command(TARGET gMod POST_BUILD
    COMMAND "${GMOD_LLVM_OBJCOPY}" --only-keep-debug "$<TARGET_FILE:gMod>" "$<TARGET_FILE_DIR:gMod>/gMod.debug"
    COMMAND "${GMOD_LLVM_OBJCOPY}" --strip-all "$<TARGET_FILE:gMod>"
    VERBATIM)

target_link_libraries(gMod PRIVATE
    dxguid
    libzippp::libzippp
    psapi
    minhook::minhook
    Microsoft::DirectXTex
    directxtexloader
    Microsoft.GSL::GSL
    )
target_link_options(gMod PRIVATE
    "-Wl,--large-address-aware"
    )
target_compile_definitions(gMod PRIVATE
    "NOMINMAX"
    "_WIN32_WINNT=_WIN32_WINNT_WIN7"
    "WIN32_LEAN_AND_MEAN"
    "VC_EXTRALEAN"
    DIRECT_INJECTION
    LOG_MESSAGE
    )

add_subdirectory(TpfConvert)
