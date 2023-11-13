include_guard()
include(FetchContent)

# Fetch mbedtls using FetchContent
FetchContent_Declare(
  mbedtls
  GIT_REPOSITORY https://github.com/Mbed-TLS/mbedtls
  GIT_TAG v3.5.1
)

FetchContent_GetProperties(mbedtls)

if(mbedtls_POPULATED)
	message(STATUS "Skipping mbedtls download")
	return()
endif()

FetchContent_Populate(mbedtls)

if(EXISTS ${CMAKE_INSTALL_PREFIX}/lib/mbedtls.lib)
	message(STATUS "Skipping mbedtls build")
	return()
endif()


execute_process(
    COMMAND ${CMAKE_COMMAND} -A Win32 -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
    WORKING_DIRECTORY ${mbedtls_SOURCE_DIR}
)
execute_process(
    COMMAND ${CMAKE_COMMAND} --build . --config Release
    WORKING_DIRECTORY ${mbedtls_SOURCE_DIR}
)
execute_process(
    COMMAND ${CMAKE_COMMAND} --install . --config Release
    WORKING_DIRECTORY ${mbedtls_SOURCE_DIR}
)

message(STATUS "Finished mbedtls build")
