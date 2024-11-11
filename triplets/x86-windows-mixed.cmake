set(VCPKG_TARGET_ARCHITECTURE x86)

if(PORT MATCHES "dxsdk-d3dx")
    set(VCPKG_CRT_LINKAGE dynamic)
    set(VCPKG_LIBRARY_LINKAGE dynamic)
else()
    set(VCPKG_CRT_LINKAGE static)
    set(VCPKG_LIBRARY_LINKAGE static)
endif()
