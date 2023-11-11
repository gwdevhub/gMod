include_guard()
include(FetchContent)

FetchContent_Declare(
    libzip
    GIT_REPOSITORY https://github.com/nih-at/libzip
    GIT_TAG v1.10.1)
FetchContent_GetProperties(libzip)
if (libzip_POPULATED)
    return()
endif()

FetchContent_Populate(libzip)

add_library(libzip)
file(GLOB SOURCES
    "${libzip_SOURCE_DIR}/src/*.h"
    "${libzip_SOURCE_DIR}/src/*.c"
    )
source_group(TREE ${libzip_SOURCE_DIR} FILES ${SOURCES})
target_sources(libzip PRIVATE ${SOURCES})
target_include_directories(libzip PRIVATE "${libzip_SOURCE_DIR}")
target_compile_definitions(libzip PRIVATE
    "-DLIBZIP_DO_INSTALL=OFF"
    )


set_target_properties(libzip PROPERTIES FOLDER "Dependencies/")
