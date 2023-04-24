cmake_minimum_required(VERSION 3.16...3.25)

project(ascension_root)

add_subdirectory(charlotte)
include(helpers)

set_standard_project_settings()

add_library(project_options INTERFACE)
target_compile_features(project_options INTERFACE cxx_std_17)

add_library(project_warnings INTERFACE)
set_compiler_warnings(project_warnings)

enable_static_analysis()

option(ENABLE_PCH "Enable Precompiled Headers" ON)

# Libraries
add_subdirectory(libs)
add_subdirectory(erika)
add_subdirectory(yuki)

# Applications
add_subdirectory(ascension)
add_subdirectory(qilian)
