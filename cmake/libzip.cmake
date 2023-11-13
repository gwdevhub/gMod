include_guard()
include(FetchContent)
include(zlib)
include(mbedtls)

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

execute_process(
    COMMAND ${CMAKE_COMMAND} -A Win32 -DZLIB_LIBRARY:PATH=${CMAKE_INSTALL_PREFIX}/lib/zlibstatic.lib -DZLIB_INCLUDE_DIR:PATH=${CMAKE_INSTALL_PREFIX}/include -DMbedTLS_INCLUDE_DIR:PATH=${CMAKE_INSTALL_PREFIX}/include -DMbedTLS_LIBRARY:PATH=${CMAKE_INSTALL_PREFIX}/lib/mbedtls.lib -DENABLE_MBEDTLS=ON -DENABLE_OPENSSL=OFF -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX} .
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
