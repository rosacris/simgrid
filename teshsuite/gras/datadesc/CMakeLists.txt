cmake_minimum_required(VERSION 2.6)

set(EXECUTABLE_OUTPUT_PATH "${CMAKE_CURRENT_BINARY_DIR}")

add_executable(datadesc_usage datadesc_usage.c datadesc_structs.c)

### Add definitions for compile
if(NOT WIN32)
target_link_libraries(datadesc_usage gras m pthread)
elseif(NOT WIN32)
target_link_libraries(datadesc_usage gras)
endif(NOT WIN32)