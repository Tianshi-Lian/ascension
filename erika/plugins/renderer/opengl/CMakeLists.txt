cmake_minimum_required(VERSION 3.16...3.25)

set(LIB_NAME opengl)
message("      (renderer) Configuring ${LIB_NAME}...")

project(${LIB_NAME}
	VERSION 0.0.1
	DESCRIPTION "An Erika rendering plugin for OpenGL"
	LANGUAGES CXX
)

add_library(${LIB_NAME} MODULE)

add_subdirectory(src)

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

add_subdirectory(libs/glad)
add_subdirectory(libs/raylib)

target_link_libraries(${LIB_NAME}
	PRIVATE project_options project_warnings yuki erika glad mraylib
)

if(WIN32)
	target_link_libraries(${LIB_NAME}
		PRIVATE opengl32
	)
elseif(UNIX)
	target_link_libraries(${LIB_NAME}
		PRIVATE GL X11 xcb X11-xcb
	)
endif()

message("      (renderer) Finished ${LIB_NAME}")
