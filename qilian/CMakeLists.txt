cmake_minimum_required(VERSION 3.16...3.25)

set(APP_NAME qilian)
message("Configuring ${APP_NAME}...")

set(APP_SRC_FILES "")
set(APP_INC_FILES "")

# Pull in our source files
include(src/sources.cmake)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${APP_NAME})

project(${APP_NAME}
	VERSION 0.0.1
	DESCRIPTION "A sandbox application for testing feature development"
	LANGUAGES CXX
)

add_executable(${APP_NAME}
	${APP_SRC_FILES}
	${APP_INC_FILES}
)

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
		PRIVATE <map> <memory> <string> <vector> <utility> <yuki/types.hpp>
	)
endif()

target_link_libraries(${APP_NAME}
	PRIVATE project_options project_warnings erika yuki
)

message("Finished ${APP_NAME}")
