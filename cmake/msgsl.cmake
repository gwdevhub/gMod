cmake_minimum_required(VERSION 3.14)

include(FetchContent)

FetchContent_Declare(GSL
    GIT_REPOSITORY "https://github.com/microsoft/GSL"
    GIT_TAG "v4.0.0"
    GIT_SHALLOW ON
)

FetchContent_MakeAvailable(GSL)
