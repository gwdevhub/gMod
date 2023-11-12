include_guard()
include(FetchContent)

# Set the version of zlib you want to use
set(ZLIB_VERSION "1.3")

# Fetch zlib using FetchContent
FetchContent_Declare(
  zlib
  GIT_REPOSITORY https://github.com/madler/zlib
  GIT_TAG v1.3
)

FetchContent_GetProperties(zlib)
if(zlib_POPULATED)
	message(STATUS "Skipping zlib download")
	return()
endif()

FetchContent_Populate(zlib)

execute_process(
    COMMAND ${CMAKE_COMMAND} -A Win32 -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
    WORKING_DIRECTORY ${zlib_SOURCE_DIR}
)
execute_process(
    COMMAND ${CMAKE_COMMAND} --build . --config Release
    WORKING_DIRECTORY ${zlib_SOURCE_DIR}
)
execute_process(
    COMMAND ${CMAKE_COMMAND} --install . --config Release
    WORKING_DIRECTORY ${zlib_SOURCE_DIR}
)
