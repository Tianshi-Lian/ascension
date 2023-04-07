cmake_minimum_required(VERSION 3.18...3.25)
cmake_policy(SET CMP0075 NEW)
cmake_policy(SET CMP0091 NEW)

set(APP_NAME ascension)
message("Configuring ${APP_NAME}...")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)

project(${APP_NAME}
	VERSION 0.0.1
	DESCRIPTION "A game about ascending through the six planes of heaven"
	LANGUAGES CXX
)

add_executable(${APP_NAME})

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
	set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT ${CMAKE_PROJECT_NAME})
	set_property(TARGET ${APP_NAME} PROPERTY WIN32_EXECUTABLE true)
endif()

target_include_directories(${APP_NAME} PRIVATE
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/${APP_NAME}>
	$<INSTALL_INTERFACE:include/${APP_NAME}>
)

set_target_properties(${APP_NAME} PROPERTIES
	LINK_WHAT_YOU_USE ON
	CXX_EXTENSIONS OFF
)

if(ENABLE_PCH)
	target_precompile_headers(${APP_NAME}
		PRIVATE <algorithm> <array> <chrono> <cmath> <cstring> <exception> <fstream> <functional> <iostream>
		PRIVATE <map> <memory> <numeric> <random> <sstream> <stdexcept> <string> <unordered_map> <unordered_set>
		PRIVATE <vector>
	)
endif()

target_link_libraries(${APP_NAME}
	PRIVATE project_options project_warnings SDL2::SDL2main SDL2::SDL2-static
)

install(
	TARGETS ${APP_NAME} DESTINATION ${CMAKE_BINARY_DIR}/dist
)
install(
	DIRECTORY ${PROJECT_SOURCE_DIR}/assets
	DESTINATION ${CMAKE_BINARY_DIR}/dist
)
install(
	DIRECTORY DESTINATION ${CMAKE_BINARY_DIR}/dist
)

add_subdirectory("src")
add_subdirectory("libs")

message("Finished ${APP_NAME}")
