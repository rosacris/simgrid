cmake_minimum_required(VERSION 2.6)

set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/_rpc_simulator.c 
                            ${CMAKE_CURRENT_BINARY_DIR}/_rpc_client.c 
			    			${CMAKE_CURRENT_BINARY_DIR}/_rpc_forwarder.c 
			    			${CMAKE_CURRENT_BINARY_DIR}/_rpc_server.c
    						PROPERTIES GENERATED true)

set(EXECUTABLE_OUTPUT_PATH "${CMAKE_CURRENT_BINARY_DIR}")

add_executable(rpc_simulator ${CMAKE_CURRENT_BINARY_DIR}/_rpc_simulator.c rpc.c)
add_executable(rpc_client    ${CMAKE_CURRENT_BINARY_DIR}/_rpc_client.c    rpc.c)
add_executable(rpc_forwarder ${CMAKE_CURRENT_BINARY_DIR}/_rpc_forwarder.c rpc.c)
add_executable(rpc_server    ${CMAKE_CURRENT_BINARY_DIR}/_rpc_server.c    rpc.c)

add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/_rpc_simulator.c
         ${CMAKE_CURRENT_BINARY_DIR}/_rpc_client.c
         ${CMAKE_CURRENT_BINARY_DIR}/_rpc_server.c
         ${CMAKE_CURRENT_BINARY_DIR}/_rpc_forwarder.c
  DEPENDS gras_stub_generator ${CMAKE_CURRENT_SOURCE_DIR}/rpc.xml
  COMMAND ${CMAKE_BINARY_DIR}/bin/gras_stub_generator rpc ${CMAKE_CURRENT_SOURCE_DIR}/rpc.xml
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

### Add definitions for compile
if(NOT WIN32)
target_link_libraries(rpc_simulator simgrid pthread m )
target_link_libraries(rpc_client gras pthread m )
target_link_libraries(rpc_forwarder gras pthread m )
target_link_libraries(rpc_server gras pthread m )
else(NOT WIN32)
target_link_libraries(rpc_simulator simgrid)
target_link_libraries(rpc_client gras)
target_link_libraries(rpc_forwarder gras)
target_link_libraries(rpc_server gras)
endif(NOT WIN32)

