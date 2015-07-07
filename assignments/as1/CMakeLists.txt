cmake_minimum_required (VERSION 2.8.11)
project (C_REFRESHER)

include_directories (${CMAKE_CURRENT_DIRECTORY}/include)
set (CMAKE_C_FLAGS "${CMAKE_CPP_FLAGS} -g -std=c99 -Wall")
add_executable (runner src/runner.c src/c_refresher.c)
