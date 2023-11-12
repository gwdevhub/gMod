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
if(NOT zlib_POPULATED)
  FetchContent_Populate(zlib)
  add_subdirectory(${zlib_SOURCE_DIR} ${zlib_BINARY_DIR})
endif()

set_target_properties(zlib PROPERTIES FOLDER "Dependencies/")
set_target_properties(zlibstatic PROPERTIES FOLDER "Dependencies/")