add_executable(TpfConvert)

# .ixx files are C++ module interfaces; the .cpp file holds main() and is an ordinary source.
file(GLOB TPF_MODULES "src/*.ixx")
file(GLOB TPF_SOURCES "src/*.cpp")

target_sources(TpfConvert PRIVATE ${TPF_SOURCES})
target_sources(TpfConvert PRIVATE
    FILE_SET CXX_MODULES
    FILES ${TPF_MODULES}
    )

set_target_properties(TpfConvert PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED ON
    CXX_SCAN_FOR_MODULES ON
    CXX_MODULE_STD ON
)

target_compile_definitions(TpfConvert PRIVATE
    "NOMINMAX"
    "_WIN32_WINNT=_WIN32_WINNT_WIN7"
    "WIN32_LEAN_AND_MEAN"
    "VC_EXTRALEAN"
)

if(DEFINED ENV{VCPKG_ROOT})
    message(STATUS "VCPKG_ROOT: $ENV{VCPKG_ROOT}")
else()
    message(STATUS "VCPKG_ROOT is not set")
endif()

if(DEFINED CMAKE_TOOLCHAIN_FILE)
    message(STATUS "CMAKE_TOOLCHAIN_FILE: ${CMAKE_TOOLCHAIN_FILE}")
else()
    message(STATUS "CMAKE_TOOLCHAIN_FILE is not set")
endif()

# D3DX9 comes from llvm-mingw's MinGW-w64 import libraries (libd3dx9_43), not a vcpkg port.
target_link_libraries(TpfConvert PRIVATE
    libzippp::libzippp
    d3dx9
    d3d9
)
