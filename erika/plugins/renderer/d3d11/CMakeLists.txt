cmake_minimum_required(VERSION 3.16...3.25)

set(LIB_NAME d3d11)
message("      (renderer) Configuring ${LIB_NAME}...")

set(LIB_SRC_FILES
	src/d3d11.cpp
)

set(LIB_INC_FILES
	${ERIKA_BASE_PLUGIN_INC}/${LIB_NAME}/d3d11.hpp
)

project(${LIB_NAME}
	VERSION 0.0.1
	DESCRIPTION "An Erika rendering plugin for D3D11"
	LANGUAGES CXX
)

add_library(${LIB_NAME} SHARED
	${LIB_SRC_FILES}
	${LIB_INC_FILES}
)

target_compile_definitions(${LIB_NAME} PRIVATE ERIKA_EXPORT)

target_include_directories(${LIB_NAME} PRIVATE
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${ERIKA_BASE_PLUGIN_INC}/${LIB_NAME}>
	$<INSTALL_INTERFACE:include/${LIB_NAME}>
)
target_include_directories(${LIB_NAME} PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	$<INSTALL_INTERFACE:include>
)

set_target_properties(${LIB_NAME} PROPERTIES
	LINK_WHAT_YOU_USE ON
	CXX_EXTENSIONS OFF
)

if(ENABLE_PCH)
	target_precompile_headers(${LIB_NAME}
		PRIVATE <string> <memory>
		PRIVATE <yuki/types.hpp>
	)
endif()

target_link_libraries(${LIB_NAME}
	PRIVATE project_options project_warnings erika yuki
)

message("      (renderer) Finished ${LIB_NAME}")
