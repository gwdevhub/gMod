include_guard()
include(FetchContent)

FetchContent_Declare(
    DirectXTex
    GIT_REPOSITORY https://github.com/microsoft/DirectXTex
    GIT_TAG jul2022)
FetchContent_GetProperties(directxtex)
if (directxtex_POPULATED)
    return()
endif()

FetchContent_Populate(directxtex)

file(READ "${directxtex_SOURCE_DIR}/WICTextureLoader/WICTextureLoader9.cpp" FILE_CONTENT)
string(REPLACE
    "hr = frame->GetPixelFormat(&pixelFormat);\n"
	"hr = frame->GetPixelFormat(&pixelFormat); if (pixelFormat == _GUID{0x6fddc324, 0x4e03, 0x4bfe, {0xb1, 0x85, 0x3d, 0x77, 0x76, 0x8d, 0xc9, 0xe}}) pixelFormat = GUID_WICPixelFormat32bppBGRA;\n"
	FILE_CONTENT "${FILE_CONTENT}" )
file(WRITE "${directxtex_SOURCE_DIR}/WICTextureLoader/WICTextureLoader9.cpp" "${FILE_CONTENT}")

add_library(directxtex)
file(GLOB SOURCES
    "${directxtex_SOURCE_DIR}/DDSTextureLoader/DDSTextureLoader9.h"
    "${directxtex_SOURCE_DIR}/DDSTextureLoader/DDSTextureLoader9.cpp"
    "${directxtex_SOURCE_DIR}/WICTextureLoader/WICTextureLoader9.h"
    "${directxtex_SOURCE_DIR}/WICTextureLoader/WICTextureLoader9.cpp"
    "${directxtex_SOURCE_DIR}/DirectXTex/*.h"
    "${directxtex_SOURCE_DIR}/DirectXTex/*.cpp"
    )
list(REMOVE_ITEM SOURCES
    "${directxtex_SOURCE_DIR}/DirectXTex/BCDirectCompute.cpp"
	)
source_group(TREE ${directxtex_SOURCE_DIR} FILES ${SOURCES})
target_sources(directxtex PRIVATE ${SOURCES})
target_include_directories(directxtex PUBLIC "${directxtex_SOURCE_DIR}")

set_target_properties(directxtex PROPERTIES FOLDER "Dependencies/")
