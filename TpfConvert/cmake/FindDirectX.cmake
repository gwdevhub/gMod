set(DIRECTX_SEARCH_PATHS
	"C:/Program Files/Microsoft DirectX SDK (June 2010)"
	"C:/Program Files (x86)/Microsoft DirectX SDK (June 2010)"
)

find_path(DirectX_INCLUDE_DIR
    NAMES d3d9.h d3dx9.h
    PATHS ${DIRECTX_SEARCH_PATHS}
    PATH_SUFFIXES Include
)

find_library(DirectX_D3DX9_LIBRARY
    NAMES d3dx9 d3dx9d
    PATHS ${DIRECTX_SEARCH_PATHS}
    PATH_SUFFIXES Lib/x86
	NO_DEFAULT_PATH
)

find_library(DirectX_D3D9_LIBRARY
    NAMES d3d9 d3d9d
    PATHS ${DIRECTX_SEARCH_PATHS}
    PATH_SUFFIXES Lib/x86
	NO_DEFAULT_PATH
)

set(DirectX_LIBRARIES ${DirectX_D3DX9_LIBRARY} ${DirectX_D3D9_LIBRARY})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(DirectX
    REQUIRED_VARS DirectX_INCLUDE_DIR DirectX_LIBRARIES
    VERSION_VAR DirectX_VERSION
)

if(DirectX_FOUND)
    message(STATUS "Found DirectX SDK: ${DirectX_INCLUDE_DIR}")
    message(STATUS "DirectX Libraries: ${DirectX_LIBRARIES}")
    set(DirectX_INCLUDE_DIRS ${DirectX_INCLUDE_DIR})
else()
    message(FATAL_ERROR "DirectX SDK not found")
endif()
