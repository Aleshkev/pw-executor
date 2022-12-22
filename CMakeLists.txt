cmake_minimum_required(VERSION 3.1)
project(MIMUW-FORK)

set(CMAKE_CXX_STANDARD "17")
set(CMAKE_C_STANDARD "11")
set(CMAKE_C_FLAGS "-g -Wall -Wextra -Wpedantic -Wno-sign-compare")

include_directories(.)

function(add_example_program target_name)
    add_executable(${target_name} "${target_name}.cpp")
    #  target_link_libraries(${target_name} err)
    target_link_libraries(${target_name} pthread)
endfunction()

#add_library(err err.cpp)
#add_library(utils utils.cpp)
#target_link_libraries(utils err)
#add_example_program(executor)
#target_link_libraries(executor utils)

#add_library(threads threads.cpp)
add_executable(executor executor.cpp)
target_link_libraries(executor pthread)
