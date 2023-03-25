cmake_minimum_required(VERSION 3.16...3.25)

set(LIB_NAME erika_renderer_plugins)

set_lib_output_directories(${ERIKA_BASE_PLUGIN_OUTPUT_DIR} renderer)
message(STATUS ${CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG})

add_library(${LIB_NAME} INTERFACE)

# We use this to ensure all users (applications) can refer to plugins as if they are part
# of erika, even though they "aren't" e.g.: #include "erika/plugins/renderer/d3d11.hpp"
set(ERIKA_BASE_PLUGIN_INC ${ERIKA_BASE_PLUGIN_INC}/renderer)

add_subdirectory(d3d11)
add_subdirectory(opengl)

target_link_libraries(${LIB_NAME} INTERFACE d3d11 opengl)

message("  (erika) Finished ${LIB_NAME}")
