

cmake_minimum_required(VERSION 3.10)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(game)

include_directories(${CMAKE_SOURCE_DIR}/src/comm)
include_directories(${CMAKE_SOURCE_DIR}/src/flat)

add_subdirectory(${CMAKE_SOURCE_DIR}/src/flat)
add_subdirectory(${CMAKE_SOURCE_DIR}/src/server)