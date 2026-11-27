set(VCPKG_TARGET_ARCHITECTURE x86)
# The builds use Ninja, so ports must not pick a Visual Studio generator just because Visual Studio is installed.
set(VCPKG_CMAKE_GENERATOR Ninja)
set(VCPKG_CRT_LINKAGE dynamic)
set(VCPKG_LIBRARY_LINKAGE static)

# CMake 4 refuses cmake_minimum_required below 3.5, which some ports still declare. This lets those ports configure.
set(VCPKG_CMAKE_CONFIGURE_OPTIONS "-DCMAKE_POLICY_VERSION_MINIMUM=3.5")

# Only DirectXTex gets the MinGW system name: it needs it to pull in directx-headers, but it breaks other ports' "windows" match.
if(PORT STREQUAL "directxtex")
    set(VCPKG_CMAKE_SYSTEM_NAME MinGW)
    list(APPEND VCPKG_CMAKE_CONFIGURE_OPTIONS "-DBC_USE_OPENMP=OFF;-DBUILD_DX11=OFF")
endif()

set(VCPKG_CHAINLOAD_TOOLCHAIN_FILE "${CMAKE_CURRENT_LIST_DIR}/../../cmake/toolchain.cmake")
