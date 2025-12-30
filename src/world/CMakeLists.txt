
add_library(world world.cc)
target_include_directories(world PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(world PUBLIC -lflatbuffers)