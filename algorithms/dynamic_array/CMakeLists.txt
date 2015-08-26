cmake_minimum_required (VERSION 2.8.11)
project (DynamicArray)

include_directories (${CMAKE_CURRENT_DIRECTORY}/include)
set (CMAKE_C_FLAGS "${CMAKE_CPP_FLAGS} -g -std=c99 -Wall -Werror")
add_executable (check test/tester.c)
