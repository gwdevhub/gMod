include_guard()
include(FetchContent)

FetchContent_Declare(
    minhook
    GIT_REPOSITORY https://github.com/TsudaKageyu/minhook
    GIT_TAG master)
FetchContent_GetProperties(minhook)

if (NOT minhook_POPULATED)
    FetchContent_Populate(minhook)
endif()

add_subdirectory(${minhook_SOURCE_DIR} EXCLUDE_FROM_ALL)

set_target_properties(minhook PROPERTIES FOLDER "Dependencies/")
