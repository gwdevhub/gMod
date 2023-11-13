include_guard()
include(FetchContent)
include(libzip)

find_library(ZLIB_LIBRARY REQUIRED
    NAMES zlibstatic
    PATHS ${CMAKE_INSTALL_PREFIX}/lib
)
find_library(LIBZIP_LIBRARY REQUIRED
    NAMES zip
    PATHS ${CMAKE_INSTALL_PREFIX}/lib
)
find_library(MBEDTLS_LIBRARY REQUIRED
    NAMES mbedtls
    PATHS ${CMAKE_INSTALL_PREFIX}/lib
)

FetchContent_Declare(
    libzippp
    GIT_REPOSITORY https://github.com/ctabin/libzippp
    GIT_TAG libzippp-v7.0-1.10.1)
FetchContent_GetProperties(libzippp)

if (NOT libzippp_POPULATED)
    FetchContent_Populate(libzippp)
endif()

add_library(libzippp)

set(SOURCES
    "${libzippp_SOURCE_DIR}/src/libzippp.h"
    "${libzippp_SOURCE_DIR}/src/libzippp.cpp"
    )
source_group(TREE ${libzippp_SOURCE_DIR} FILES ${SOURCES})
target_sources(libzippp PRIVATE ${SOURCES})
target_include_directories(libzippp PUBLIC "${libzippp_SOURCE_DIR}/src")
target_include_directories(libzippp PRIVATE "${CMAKE_INSTALL_PREFIX}/include")
target_compile_definitions(libzippp PUBLIC
    LIBZIPPP_WITH_ENCRYPTION
	)
target_link_libraries(libzippp PRIVATE
	${LIBZIP_LIBRARY}
	${MBEDTLS_LIBRARY}
	${ZLIB_LIBRARY}
	)

set_target_properties(libzippp PROPERTIES FOLDER "Dependencies/")
