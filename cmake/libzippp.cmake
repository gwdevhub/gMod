include_guard()
include(FetchContent)
include(libzip)

# Find the libzip package after it's built
# find_package(libzip REQUIRED)

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
set(SOURCES
    "${libzippp_SOURCE_DIR}/src/libzippp.h"
    "${libzippp_SOURCE_DIR}/src/libzippp.cpp"
    )
source_group(TREE ${libzippp_SOURCE_DIR} FILES ${SOURCES})
target_sources(libzippp PRIVATE ${SOURCES})
target_include_directories(libzippp PUBLIC "${libzippp_SOURCE_DIR}")
target_include_directories(libzippp PRIVATE "${libzip_SOURCE_DIR}")
target_link_libraries(libzippp PRIVATE libzip)

set_target_properties(libzippp PROPERTIES FOLDER "Dependencies/")
