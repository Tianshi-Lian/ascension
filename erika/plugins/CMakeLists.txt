cmake_minimum_required(VERSION 3.16...3.25)

message("  (erika) Configuring erika plugin system...")

set(ERIKA_BASE_PLUGIN_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/bin)
set(ERIKA_BASE_INT_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/ints)

# We use this to ensure all users (applications) can refer to plugins as if they are part
# of erika, even though they "aren't" e.g.: #include "erika/plugins/renderer/d3d11.hpp"
# This can be overriden by subdirs.
set(ERIKA_BASE_PLUGIN_INC include/erika/plugins)

add_subdirectory(base)
add_subdirectory(renderer)

message("  (erika) Finished erika plugin system")
