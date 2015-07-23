cmake_minimum_required (VERSION 2.8.11)
project (DynamicArray)

include_directories (${CMAKE_CURRENT_DIRECTORY}/include)
set (CMAKE_C_FLAGS "${CMAKE_CPP_FLAGS} -g -std=c99 -Wall")
# TODO: don't put debug symbols in compiled library code! (?)
add_executable (test test/tester.c)
