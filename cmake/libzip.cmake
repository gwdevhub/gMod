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

execute_process(
    COMMAND ${CMAKE_COMMAND} -A Win32 -DZLIB_LIBRARY:PATH="${CMAKE_INSTALL_PREFIX}/lib/zlibstatic.lib" -DZLIB_INCLUDE_DIR:PATH="${CMAKE_INSTALL_PREFIX}/include" -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX} .
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
