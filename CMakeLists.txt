cmake_minimum_required(VERSION 3.16...3.25)

set(LIB_NAME erika)
message("Configuring ${LIB_NAME}...")

set(LIB_SRC_FILES
	src/erika.cpp
)

set(LIB_INC_FILES
	include/${LIB_NAME}/erika.hpp
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/dist)

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/../sophia/CMakeLists.txt)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../sophia ${CMAKE_BINARY_DIR}/deps/${LIB_NAME}/sophia)
    message(STATUS "Found local sophia directory.")
else()
    include(FetchContent)
    FetchContent_Declare(sophia
        GIT_REPOSITORY git@github.com:r-ggraham/sophia.git
        GIT_TAG "origin/main"
    )
    FetchContent_MakeAvailable(sophia)
endif()
include(helpers)

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/../yuki/CMakeLists.txt)
	message(STATUS "Found local yuki directory.")
else()
    include(FetchContent)
    FetchContent_Declare(yuki
        GIT_REPOSITORY git@github.com:r-ggraham/yuki.git
        GIT_TAG "origin/main"
    )
    FetchContent_MakeAvailable(yuki)
endif()

project(${LIB_NAME}
    VERSION 0.0.1
    DESCRIPTION "A 2D game engine written in C++17"
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
    target_precompile_headers(${LIB_NAME} PUBLIC)
endif()

target_link_libraries(${LIB_NAME}
    PRIVATE project_options project_warnings yuki
)

message("Finished ${LIB_NAME}")
