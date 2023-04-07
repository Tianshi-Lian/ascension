cmake_minimum_required(VERSION 3.16...3.25)

set(LIB_NAME mraylib)
message("        (opengl) Configuring ${LIB_NAME}...")

set(LIB_SRC_FILES
	"src/rlgl.cpp"
	"src/rlshapes.cpp"
	"src/rltext.cpp"
	"src/rltextures.cpp"
	"src/rlutils.cpp"
)
set(LIB_INC_FILES
	"include/${LIB_NAME}/rltypes.hpp"
	"include/${LIB_NAME}/rlgl.hpp"
	"include/${LIB_NAME}/rlmath.hpp"
	"include/${LIB_NAME}/rlshapes.hpp"
	"include/${LIB_NAME}/rltext.hpp"
	"include/${LIB_NAME}/rltextures.hpp"
	"include/${LIB_NAME}/rlutils.hpp"
)

project(${LIB_NAME}
	VERSION "4.5.0"
	HOMEPAGE_URL "https://github.com/raysan5/raylib/blob/master/src/rlgl.h"
	DESCRIPTION "A multi-OpenGL abstraction layer with an immediate-mode style API"
	LANGUAGES CXX
)

add_library(${LIB_NAME}
	${LIB_SRC_FILES}
	${LIB_INC_FILES}
)
target_compile_features(${LIB_NAME} INTERFACE cxx_std_17)

target_include_directories(${LIB_NAME} SYSTEM PRIVATE
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/libs>
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

target_link_libraries(${LIB_NAME}
	PRIVATE glad
)

message("        (opengl) Finished ${LIB_NAME}")
