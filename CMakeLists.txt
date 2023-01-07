cmake_minimum_required(VERSION 3.1)
project(MIMUW-FORK)

set(CMAKE_CXX_STANDARD "17")
set(CMAKE_C_STANDARD "11")
set(CMAKE_C_FLAGS "-g -Wall -Wextra -Wpedantic -Wno-sign-compare")

include_directories(.)

add_executable(executor executor.cpp)
target_link_libraries(executor pthread)
