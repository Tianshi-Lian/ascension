set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/dist)

set(APP_NAME ascension)
add_executable(${APP_NAME}
    src/main.cpp
)

target_link_libraries(${APP_NAME}
    PRIVATE project_options project_warnings erika yuki
)

target_include_directories(${APP_NAME} PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/${APP_NAME}>
    $<INSTALL_INTERFACE:include/${APP_NAME}>
)

set_target_properties(${APP_NAME} PROPERTIES
    LINK_WHAT_YOU_USE ON
)
