cmake_minimum_required(VERSION 3.16...3.25)

set(LIB_NAME erika_plugins)
message("  (erika) Configuring ${LIB_NAME}...")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/erika/plugins/bin)

add_library(${LIB_NAME} INTERFACE)

# We use this to ensure all users (applications) can refer to plugins as if they are part
# of erika, even though they "aren't" e.g.: #include "erika/plugins/renderer/d3d11.hpp"
set(ERIKA_BASE_PLUGIN_INC include/erika/plugins)

# TODO: Add renderer/CMakeLists.txt and make sure ../renderer/.. is added to public target includes
add_subdirectory(renderer/d3d11)

target_link_libraries(${LIB_NAME} INTERFACE d3d11)

message("  (erika) Finished ${LIB_NAME}")
