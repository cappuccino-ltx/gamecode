

cmake_minimum_required(VERSION 3.10)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(game)

include_directories(${CMAKE_SOURCE_DIR}/src/comm)
include_directories(${CMAKE_SOURCE_DIR}/src/flat)
include_directories(${CMAKE_SOURCE_DIR}/src/entity)

add_subdirectory(${CMAKE_SOURCE_DIR}/src/flat)
add_subdirectory(${CMAKE_SOURCE_DIR}/src/connect)
add_subdirectory(${CMAKE_SOURCE_DIR}/src/world)
add_subdirectory(${CMAKE_SOURCE_DIR}/src/server)