cmake_minimum_required(VERSION 3.16...3.25)

set(APP_NAME ascension)
message("Configuring ${APP_NAME}...")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/ints)
set(CMAKE_PDB_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/ints)

project(${APP_NAME}
	VERSION 0.0.1
	DESCRIPTION "A game about ascending through the six planes of heaven"
	LANGUAGES CXX
)

add_executable(${APP_NAME})

add_subdirectory(include/ascension)
add_subdirectory(src)
add_subdirectory(libs)

set_app_ito(${APP_NAME})

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
		PRIVATE <map> <memory> <string> <vector> <utility>
		PRIVATE <core/types.hpp>
	)
endif()

target_link_libraries(${APP_NAME}
	PRIVATE project_options project_warnings yuki
	PRIVATE SDL2::SDL2main SDL2::SDL2-static libglew_static freetype pugixml-static
)

target_compile_definitions(${APP_NAME} PUBLIC $<$<CONFIG:Debug>:AS_DEBUG>)

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

message("Finished ${APP_NAME}")
