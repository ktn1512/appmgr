cmake_minimum_required(VERSION 3.16)

project(appmgr)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_EXTENSIONS OFF)

add_library(lib

    ./src/appmgr.cpp

)

target_include_directories(lib PUBLIC

    ${CMAKE_CURRENT_SOURCE_DIR}/inc

)

add_executable(appmgr

    ./src/main.cpp

)

target_link_libraries(appmgr lib)
