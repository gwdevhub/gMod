include_guard()
include(FetchContent)
include(libzip)

FetchContent_Declare(
    libzippp
    GIT_REPOSITORY https://github.com/ctabin/libzippp
    GIT_TAG libzippp-v7.0-1.10.1)
FetchContent_GetProperties(libzippp)

if (NOT libzippp_POPULATED)
    FetchContent_Populate(libzippp)
endif()

option(CMAKE_PREFIX_PATH "" ${CMAKE_INSTALL_PREFIX})
set(CMAKE_PREFIX_PATH ${CMAKE_INSTALL_PREFIX})

add_subdirectory(${libzippp_SOURCE_DIR} EXCLUDE_FROM_ALL)

set_target_properties(libzippp PROPERTIES FOLDER "Dependencies/")
