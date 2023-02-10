include_guard()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/dist)
set(LIB_NAME erika)

add_library(${LIB_NAME}
    src/erika.cpp
    include/${LIB_NAME}/erika.h
)

target_link_libraries(${LIB_NAME}
    PRIVATE project_options project_warnings yuki
)

target_include_directories(${LIB_NAME} PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/${LIB_NAME}>
    $<INSTALL_INTERFACE:include/${LIB_NAME}>
)
target_include_directories(${LIB_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

set_target_properties(${LIB_NAME} PROPERTIES
    LINK_WHAT_YOU_USE ON
)
