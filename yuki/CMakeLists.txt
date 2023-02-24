cmake_minimum_required(VERSION 3.16...3.25)

set(LIB_NAME yuki)
message("Configuring ${LIB_NAME}...")

set(LIB_SRC_FILES
	src/yuki.cpp
	src/debug/logger.cpp
)

set(LIB_INC_FILES
	include/${LIB_NAME}/yuki.hpp
	include/${LIB_NAME}/debug/logger.hpp
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/dist)
include(helpers)

project(${LIB_NAME}
    VERSION 0.0.1
    DESCRIPTION "A core library for C++ development, graphics & games"
    LANGUAGES CXX
)
set_standard_project_settings()

if(NOT TARGET project_options)
    add_library(project_options INTERFACE)
    target_compile_features(project_options INTERFACE cxx_std_17)
endif()

if(NOT TARGET project_warnings)
    add_library(project_warnings INTERFACE)
    set_compiler_warnings(project_warnings)
endif()

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
)

if(ENABLE_PCH)
    target_precompile_headers(${LIB_NAME}
		PRIVATE <string> include/${LIB_NAME}/types.hpp
	)
endif()

add_subdirectory(libs/magic_enum)

target_link_libraries(${LIB_NAME}
    PRIVATE project_options project_warnings pthread
	PUBLIC magic_enum
)

message("Finished ${LIB_NAME}")
