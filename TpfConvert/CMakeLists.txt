add_executable(TpfConvert)

# Find all .ixx files in the TpfConvert folder
file(GLOB TPF_SOURCES "src/*.ixx")

# Add the .ixx files to the TpfConvert target
target_sources(TpfConvert PRIVATE ${TPF_SOURCES})

set_target_properties(TpfConvert PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED ON
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

find_package(dxsdk-d3dx CONFIG REQUIRED)

target_link_libraries(TpfConvert PRIVATE
    libzippp::libzippp
    Microsoft::D3DX9
    d3d9
)
