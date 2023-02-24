cmake_minimum_required(VERSION 3.16...3.25)

set(APP_NAME ascension)

add_subdirectory(sophia)
include(helpers)

message("Configuring ${APP_NAME}...")
project(${APP_NAME}
    VERSION 0.0.1
    DESCRIPTION "Description"
    LANGUAGES CXX
)
set_standard_project_settings()

add_library(project_options INTERFACE)
target_compile_features(project_options INTERFACE cxx_std_17)

add_library(project_warnings INTERFACE)
set_compiler_warnings(project_warnings)

enable_static_analysis()

# Libraries
add_subdirectory(erika)
add_subdirectory(yuki)

# Application
add_subdirectory(ascension)

message("Finished ${APP_NAME}")
