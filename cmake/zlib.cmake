include_guard()
include(FetchContent)

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

if(EXISTS ${CMAKE_INSTALL_PREFIX}/lib/zlibstatic.lib)
	message(STATUS "Skipping zlib build")
	return()
endif()

set(OPTIONS
    "-A" "Win32"
    "-DBUILD_SHARED_LIBS=OFF"
    "-DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}"
    "-DCMAKE_MSVC_RUNTIME_LIBRARY=${CMAKE_MSVC_RUNTIME_LIBRARY}"
)
	
message("Building zlib with OPTIONS:\n${OPTIONS}")

execute_process(
    COMMAND ${CMAKE_COMMAND} ${OPTIONS} .
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

message(STATUS "Finished zlib build")
