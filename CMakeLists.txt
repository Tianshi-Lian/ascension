cmake_minimum_required(VERSION 3.16...3.25)

set(APP_PROJECT_NAME ascension)
set(APP_LIBS_DIR libs)

project(${APP_PROJECT_NAME}
    VERSION 0.0.1
    DESCRIPTION "Description"
    LANGUAGES CXX
)
include(cmake/standard_project_settings.cmake)

add_library(project_options INTERFACE)
target_compile_features(project_options INTERFACE cxx_std_17)

add_library(project_warnings INTERFACE)
include(cmake/compiler_warnings.cmake)
set_project_warnings(project_warnings)

include(cmake/static_analysis.cmake)

option(ENABLE_PCH "Enable Precompiled Headers" ON)

if(ENABLE_PCH)
    target_precompile_headers(project_options INTERFACE <map> <memory> <string> <vector> <utility>)
endif()

# Libraries
add_subdirectory(libs/erika)
add_subdirectory(libs/yuki)

# Application
add_subdirectory(ascension)
