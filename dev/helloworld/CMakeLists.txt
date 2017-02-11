project(helloworld)
cmake_minimum_required(VERSION 2.8)

set(_srcs
    src/main.cpp
)

add_executable(${PROJECT_NAME} ${_srcs})
