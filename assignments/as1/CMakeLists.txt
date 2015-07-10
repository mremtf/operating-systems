cmake_minimum_required (VERSION 2.8.11)
project (C_REFRESHER)

include_directories (${CMAKE_CURRENT_DIRECTORY}/include)
set (CMAKE_C_FLAGS "${CMAKE_CPP_FLAGS} -g -std=c99 -Wall")
set (CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules
								${CMAKE_MODULE_PATH})

add_executable (matlab src/runner.c src/c_refresher.c)
target_link_libraries(matlab readline)

