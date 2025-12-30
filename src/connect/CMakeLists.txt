

add_library(connect connect.cc)
target_include_directories(connect PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(connect PUBLIC -lflatbuffers -lenet)