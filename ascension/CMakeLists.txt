set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/dist)

set(APP_SRC_FILES
	src/main.cpp
	src/ascension.cpp
)

# This isn't needed but makes IDEs function nicer
set(APP_INC_FILES
	include/${APP_NAME}/ascension.hpp
)

add_executable(${APP_NAME}
	${APP_SRC_FILES}
	${APP_INC_FILES}
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
	CXX_EXTENSIONS OFF
)

option(ENABLE_PCH "Enable Precompiled Headers" ON)

if(ENABLE_PCH)
	target_precompile_headers(project_options INTERFACE <map> <memory> <string> <vector> <utility> <yuki/types.hpp>)
endif()
