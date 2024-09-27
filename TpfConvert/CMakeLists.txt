list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/TpfConvert/cmake")

find_package(DirectX REQUIRED)

add_executable(TpfConvert)

# Find all .ixx files in the TpfConvert folder
file(GLOB TPF_SOURCES "src/*.ixx")

# Add the .ixx files to the TpfConvert target
target_sources(TpfConvert PRIVATE ${TPF_SOURCES})

set_target_properties(TpfConvert PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED ON
)

target_compile_definitions(TpfConvert PRIVATE
    "NOMINMAX"
    "_WIN32_WINNT=_WIN32_WINNT_WIN7"
    "WIN32_LEAN_AND_MEAN"
    "VC_EXTRALEAN"
)

target_include_directories(TpfConvert PRIVATE ${DirectX_INCLUDE_DIRS})

target_link_libraries(TpfConvert PRIVATE
    libzippp
    ${DirectX_LIBRARIES}             
)
