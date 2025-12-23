
cmake_minimum_required(VERSION 3.10)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# 指定vcpkg 路径
set(CMAKE_TOOLCHAIN_FILE "~/.vcpkg/scripts/buildsystems/vcpkg.cmake")

project(enet_test)

find_package(EnTT CONFIG REQUIRED)

add_executable(test entt_test.cc)

target_link_libraries(test PRIVATE EnTT::EnTT)
target_include_directories(test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../comm)