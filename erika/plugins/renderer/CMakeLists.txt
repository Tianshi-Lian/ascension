cmake_minimum_required(VERSION 3.16...3.25)

set(LIB_NAME erika_renderer_plugins)

# TODO: Move to using LoadLibrary (and linux equivalent) to load shared libs from sub-folders
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/erika/plugins/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/erika/plugins/bin")

add_library(${LIB_NAME} INTERFACE)

# We use this to ensure all users (applications) can refer to plugins as if they are part
# of erika, even though they "aren't" e.g.: #include "erika/plugins/renderer/d3d11.hpp"
set(ERIKA_BASE_PLUGIN_INC include/erika/plugins/renderer)

add_subdirectory(d3d11)

target_link_libraries(${LIB_NAME} INTERFACE d3d11)

message("  (erika) Finished ${LIB_NAME}")
