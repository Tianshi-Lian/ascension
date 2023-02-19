cmake_minimum_required(VERSION 3.16...3.25)

set(LIB_NAME magic_enum)
message("Configuring ${LIB_NAME}...")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/dist)

project(${LIB_NAME}
	VERSION "0.8.2"
	HOMEPAGE_URL "https://github.com/Neargye/magic_enum"
	DESCRIPTION "A library that provides static reflection for enums, work with any enum type without any macro or boilerplate code."
	LANGUAGES CXX
)

add_library(${LIB_NAME} INTERFACE)
target_compile_features(${LIB_NAME} INTERFACE cxx_std_17)

target_include_directories(${LIB_NAME} INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

message("Finished ${LIB_NAME}")
