# Set the version of miniz to download
set(MINIZ_VERSION 3.0.2)
set(MINIZ_SOURCE_DIR "${CMAKE_BINARY_DIR}/miniz-${MINIZ_VERSION}")

# Download miniz source code
file(DOWNLOAD "https://github.com/richgel999/miniz/releases/download/${MINIZ_VERSION}/miniz-${MINIZ_VERSION}.zip"
    "${CMAKE_BINARY_DIR}/miniz-${MINIZ_VERSION}.zip"
    SHOW_PROGRESS
)

# Extract miniz source code
file(MAKE_DIRECTORY ${MINIZ_SOURCE_DIR})
execute_process(
    COMMAND ${CMAKE_COMMAND} -E tar xzf "${CMAKE_BINARY_DIR}/miniz-${MINIZ_VERSION}.zip"
    WORKING_DIRECTORY ${MINIZ_SOURCE_DIR}
)

# Add miniz sources to the project
file(GLOB MINIZ_SOURCES
    "${MINIZ_SOURCE_DIR}/miniz.h"
    "${MINIZ_SOURCE_DIR}/miniz.c"
	)

# Create a library target for miniz
add_library(miniz STATIC ${MINIZ_SOURCES})

# Include directories for miniz
target_include_directories(miniz PUBLIC ${MINIZ_SOURCE_DIR})