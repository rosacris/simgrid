cmake_minimum_required(VERSION 2.6)

set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/_bandwidth_simulator.c
							${CMAKE_CURRENT_BINARY_DIR}/_bandwidth_maestro.c
							${CMAKE_CURRENT_BINARY_DIR}/_bandwidth_sensor.c
							PROPERTIES GENERATED true)

set(EXECUTABLE_OUTPUT_PATH "${CMAKE_CURRENT_BINARY_DIR}")

add_executable(bandwidth_simulator 	${CMAKE_CURRENT_BINARY_DIR}/_bandwidth_simulator.c
									${CMAKE_CURRENT_SOURCE_DIR}/bandwidth.c)
add_executable(bandwidth_maestro 	${CMAKE_CURRENT_BINARY_DIR}/_bandwidth_maestro.c
									${CMAKE_CURRENT_SOURCE_DIR}/bandwidth.c)
add_executable(bandwidth_sensor ${CMAKE_CURRENT_BINARY_DIR}/_bandwidth_sensor.c
								${CMAKE_CURRENT_SOURCE_DIR}/bandwidth.c)

add_custom_command(
  					OUTPUT 	${CMAKE_CURRENT_BINARY_DIR}/_bandwidth_simulator.c
							${CMAKE_CURRENT_BINARY_DIR}/_bandwidth_maestro.c
							${CMAKE_CURRENT_BINARY_DIR}/_bandwidth_sensor.c
  					DEPENDS gras_stub_generator ${CMAKE_CURRENT_SOURCE_DIR}/bandwidth.xml
  					COMMAND ${CMAKE_BINARY_DIR}/bin/gras_stub_generator bandwidth ${CMAKE_CURRENT_SOURCE_DIR}/bandwidth.xml
					WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
					)
					
### Add definitions for compile
if(NOT WIN32)
target_link_libraries(bandwidth_simulator simgrid pthread m )
target_link_libraries(bandwidth_maestro gras pthread m )
target_link_libraries(bandwidth_sensor gras pthread m )
else(NOT WIN32)
target_link_libraries(bandwidth_simulator simgrid)
target_link_libraries(bandwidth_maestro gras)
target_link_libraries(bandwidth_sensor gras)
endif(NOT WIN32)