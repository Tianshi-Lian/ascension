include_guard()

set(LIB_SRC_FILES
	${LIB_SRC_FILES}
	src/plugins/plugin_manager.cpp
	src/plugins/renderer.cpp
)

set(LIB_INC_FILES
	${LIB_INC_FILES}
	include/${LIB_NAME}/plugins/plugin_manager.hpp
	include/${LIB_NAME}/plugins/plugin_types.hpp
	include/${LIB_NAME}/plugins/renderer.hpp
)
