cmake_minimum_required(VERSION 2.6)

set(EXECUTABLE_OUTPUT_PATH "${CMAKE_CURRENT_BINARY_DIR}")

add_executable(test_intra_all2all test_intra_all2all.c)
add_executable(test_intra_independent_comm test_intra_independent_comm.c)
add_executable(test_intra_scatter test_intra_scatter.c)

### Add definitions for compile
if(NOT WIN32)
target_link_libraries(test_intra_all2all simgrid m pthread )
target_link_libraries(test_intra_independent_comm simgrid m pthread )
target_link_libraries(test_intra_scatter simgrid m pthread )
else(NOT WIN32)
target_link_libraries(test_intra_all2all simgrid)
target_link_libraries(test_intra_independent_comm simgrid)
target_link_libraries(test_intra_scatter simgrid)
endif(NOT WIN32)