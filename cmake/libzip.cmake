include_guard()
include(FetchContent)
include(zlib)

FetchContent_Declare(
    libzip
    GIT_REPOSITORY https://github.com/nih-at/libzip
    GIT_TAG v1.10.1)
FetchContent_GetProperties(libzip)

if (libzip_POPULATED)
    message(STATUS "Skipping libzip download")
    return()
endif()

FetchContent_Populate(libzip)
if(EXISTS ${CMAKE_INSTALL_PREFIX}/lib/zip.lib)
    message(STATUS "Skipping libzip build")
    return()
endif()

set(OPTIONS
    "-A" "Win32"
    "-DZLIB_LIBRARY:PATH=${CMAKE_INSTALL_PREFIX}/lib/zlibstatic.lib"
    "-DZLIB_INCLUDE_DIR:PATH=${CMAKE_INSTALL_PREFIX}/include"
    "-DENABLE_COMMONCRYPTO=OFF"
    "-DENABLE_GNUTLS=OFF"
    "-DENABLE_MBEDTLS=OFF"
    "-DENABLE_OPENSSL=OFF"
    "-DENABLE_WINDOWS_CRYPTO=ON"
    "-DENABLE_FDOPEN=OFF"
    "-DENABLE_BZIP2=OFF"
    "-DENABLE_LZMA=OFF"
    "-DENABLE_ZSTD=OFF"
    "-DBUILD_TOOLS=OFF"
    "-DBUILD_REGRESS=OFF"
    "-DBUILD_EXAMPLES=OFF"
    "-DBUILD_DOC=OFF"
    "-DLIBZIP_DO_INSTALL=ON"
    "-DBUILD_SHARED_LIBS=OFF"
    "-DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}"
    "-DCMAKE_MSVC_RUNTIME_LIBRARY=${CMAKE_MSVC_RUNTIME_LIBRARY}"
    )

message("Building libzip with OPTIONS:\n${OPTIONS}")

# Patch CMakeLists.txt to require version 3.15 - otherwise cmake ignores MSVC_RUNTIME_LIBRARY
file(READ ${libzip_SOURCE_DIR}/CMakeLists.txt FILE_CONTENT)
string(REPLACE "VERSION 3.5.0" "VERSION 3.15.0" UPDATED_CONTENT "${FILE_CONTENT}")
file(WRITE ${libzip_SOURCE_DIR}/CMakeLists.txt "${UPDATED_CONTENT}")
      
execute_process(
    COMMAND ${CMAKE_COMMAND} ${OPTIONS} .
    WORKING_DIRECTORY ${libzip_SOURCE_DIR}
)
execute_process(
    COMMAND ${CMAKE_COMMAND} --build . --config Release
    WORKING_DIRECTORY ${libzip_SOURCE_DIR}
)
execute_process(
    COMMAND ${CMAKE_COMMAND} --install . --config Release
    WORKING_DIRECTORY ${libzip_SOURCE_DIR}
)

message(STATUS "Finished libzip build")
