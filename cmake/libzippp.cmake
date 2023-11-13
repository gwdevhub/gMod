include_guard()
include(FetchContent)
include(libzip)

# Find the libzip package after it's built
find_library(ZLIB_LIBRARY REQUIRED
    NAMES zlibstatic zlib
    PATHS ${CMAKE_INSTALL_PREFIX}/lib
)
find_library(LIBZIP REQUIRED
    NAMES zip
    PATHS ${CMAKE_INSTALL_PREFIX}/lib
)
find_library(MBEDTLS REQUIRED
    NAMES mbedtls
    PATHS ${CMAKE_INSTALL_PREFIX}/lib
)


FetchContent_Declare(
    libzippp
    GIT_REPOSITORY https://github.com/ctabin/libzippp
    GIT_TAG libzippp-v7.0-1.10.1)
FetchContent_GetProperties(libzippp)
if (libzippp_POPULATED)
    return()
endif()

FetchContent_Populate(libzippp)

add_library(libzippp)

set(LIBZIPPP_ENABLE_ENCRYPTION ON)

set(SOURCES
    "${libzippp_SOURCE_DIR}/src/libzippp.h"
    "${libzippp_SOURCE_DIR}/src/libzippp.cpp"
    )
source_group(TREE ${libzippp_SOURCE_DIR} FILES ${SOURCES})
target_sources(libzippp PRIVATE ${SOURCES})
target_include_directories(libzippp PUBLIC "${libzippp_SOURCE_DIR}/src")
target_include_directories(libzippp PRIVATE "${CMAKE_INSTALL_PREFIX}/include")
target_link_libraries(libzippp PRIVATE libzip)

set_target_properties(libzippp PROPERTIES FOLDER "Dependencies/")
