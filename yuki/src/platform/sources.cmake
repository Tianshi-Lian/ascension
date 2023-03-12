include_guard()

set(LIB_SRC_FILES
	${LIB_SRC_FILES}
	src/platform/platform.cpp
	src/platform/platform_linux.cpp
	src/platform/platform_win32.cpp
)

set(LIB_INC_FILES
	${LIB_INC_FILES}
	include/${LIB_NAME}/platform/platform.hpp
)
