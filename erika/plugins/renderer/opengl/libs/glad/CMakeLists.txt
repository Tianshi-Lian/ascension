cmake_minimum_required(VERSION 3.16...3.25)

set(LIB_NAME glad)
message("        (opengl) Configuring ${LIB_NAME}...")

set(LIB_SRC_FILES
	"src/gl.c"
	"src/wgl.c"
)
set(LIB_INC_FILES
	"include/${LIB_NAME}/gl.h"
	"include/${LIB_NAME}/wgl.h"
)

project(${LIB_NAME}
	VERSION "2.0.4"
	HOMEPAGE_URL "https://github.com/Dav1dde/glad"
	DESCRIPTION "
		Multi-Language Vulkan/GL/GLES/EGL/GLX/WGL Loader-Generator based on the official specs.
		Generated from: https://gen.glad.sh/
		Permalink: https://tinyurl.com/yc7uth4y"
	LANGUAGES CXX
)

add_library(${LIB_NAME}
	${LIB_SRC_FILES}
	${LIB_INC_FILES}
)
target_compile_features(${LIB_NAME} INTERFACE cxx_std_17)

target_include_directories(${LIB_NAME} SYSTEM PRIVATE
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/${LIB_NAME}>
	$<INSTALL_INTERFACE:include/${LIB_NAME}>
)
target_include_directories(${LIB_NAME} SYSTEM PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	$<INSTALL_INTERFACE:include>
)

set_target_properties(${LIB_NAME} PROPERTIES
	CXX_EXTENSIONS OFF
	COMPILE_FLAGS "${CMAKE_CXX_FLAGS} -fPIC"
)

message("        (opengl) Finished ${LIB_NAME}")
