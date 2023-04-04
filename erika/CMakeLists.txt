cmake_minimum_required(VERSION 3.16...3.25)

set(LIB_NAME erika)
message("Configuring ${LIB_NAME}...")

set(LIB_SRC_FILES "")
set(LIB_INC_FILES "")

# Pull in our source files
include(src/sources.cmake)

project(${LIB_NAME}
	VERSION 0.0.1
	DESCRIPTION "A 2D game engine written in C++17"
	LANGUAGES CXX
)

add_library(${LIB_NAME}
	${LIB_SRC_FILES}
	${LIB_INC_FILES}
)

target_include_directories(${LIB_NAME} PRIVATE
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/${LIB_NAME}>
	$<INSTALL_INTERFACE:include/${LIB_NAME}>
)
target_include_directories(${LIB_NAME} PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	$<INSTALL_INTERFACE:include>
)

set_target_properties(${LIB_NAME} PROPERTIES
	LINK_WHAT_YOU_USE ON
	CXX_EXTENSIONS OFF
	COMPILE_FLAGS "${CMAKE_CXX_FLAGS} -fPIC"
)

if(ENABLE_PCH)
	target_precompile_headers(${LIB_NAME}
		PRIVATE <string> <memory>
		PRIVATE <yuki/types.hpp>
		PRIVATE <core/defines.hpp>
	)
endif()

target_link_libraries(${LIB_NAME}
	PRIVATE project_options project_warnings yuki
)

# Erika plugins
add_subdirectory(plugins)

message("Finished ${LIB_NAME}")
