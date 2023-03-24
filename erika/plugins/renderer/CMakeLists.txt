cmake_minimum_required(VERSION 3.16...3.25)

set(LIB_NAME erika_renderer_plugins)

# _LIBRARY_ is used by linux, whereas windows is _RUNTIME_.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${ERIKA_BASE_OUTPUT_DIR}/${CMAKE_BUILD_TYPE}/renderer)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${ERIKA_BASE_OUTPUT_DIR}/${CMAKE_BUILD_TYPE}/renderer)

foreach(CONFIG_TYPE IN LISTS CMAKE_CONFIGURATION_TYPES)
	string(TOUPPER ${CONFIG_TYPE} CONFIG_PROPERTY)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CONFIG_PROPERTY} ${ERIKA_BASE_OUTPUT_DIR}/${CONFIG_TYPE}/renderer)
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${CONFIG_PROPERTY} ${ERIKA_BASE_OUTPUT_DIR}/${CONFIG_TYPE}/renderer)
endforeach()

add_library(${LIB_NAME} INTERFACE)

# We use this to ensure all users (applications) can refer to plugins as if they are part
# of erika, even though they "aren't" e.g.: #include "erika/plugins/renderer/d3d11.hpp"
set(ERIKA_BASE_PLUGIN_INC ${ERIKA_BASE_PLUGIN_INC}/renderer)

add_subdirectory(d3d11)
add_subdirectory(opengl)

target_link_libraries(${LIB_NAME} INTERFACE d3d11 opengl)

message("  (erika) Finished ${LIB_NAME}")
