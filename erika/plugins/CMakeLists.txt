cmake_minimum_required(VERSION 3.16...3.25)

set(LIB_NAME erika_plugins)
message("  (erika) Configuring ${LIB_NAME}...")

add_library(${LIB_NAME} INTERFACE)

set(ERIKA_BASE_PLUGIN_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/bin)
set(ERIKA_BASE_INT_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/ints)

# We use this to ensure all users (applications) can refer to plugins as if they are part
# of erika, even though they "aren't" e.g.: #include "erika/plugins/renderer/d3d11.hpp"
# This can be overriden by subdirs.
set(ERIKA_BASE_PLUGIN_INC include/erika/plugins)

# Import & grab all our plugin options/types.
add_subdirectory(renderer)

add_dependencies(${LIB_NAME}
    erika
    erika_renderer_plugins
)

message("  (erika) Finished ${LIB_NAME}")
