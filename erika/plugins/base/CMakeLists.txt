cmake_minimum_required(VERSION 3.16...3.25)

set(LIB_NAME erika_plugins_base)
message("      (renderer) Configuring ${LIB_NAME}...")

project(${LIB_NAME}
    VERSION 0.0.1
    DESCRIPTION "Interfaces for creating Erika plugins."
    LANGUAGES CXX
)

add_library(${LIB_NAME} STATIC)

add_subdirectory(src)

message("${CMAKE_CURRENT_SOURCE_DIR}/include")
target_include_directories(${LIB_NAME} PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${ERIKA_BASE_PLUGIN_INC}/base>
    $<INSTALL_INTERFACE:include/base>
)
target_include_directories(${LIB_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

set_target_properties(${LIB_NAME} PROPERTIES
    LINK_WHAT_YOU_USE ON
    CXX_EXTENSIONS OFF
)

if(ENABLE_PCH)
    target_precompile_headers(${LIB_NAME}
        PRIVATE <string> <memory> <vector>
        PRIVATE <yuki/types.hpp>
    )
endif()

target_link_libraries(${LIB_NAME}
    PRIVATE project_options project_warnings yuki
    PUBLIC glm
)

# target_compile_definitions(${LIB_NAME} PRIVATE ERIKA_EXPORT)
message("      (renderer) Finished ${LIB_NAME}")
