include_guard()

set(LIB_SRC_FILES
	${LIB_SRC_FILES}
	src/debug/instrumentor.cpp
	src/debug/logger.cpp
)

set(LIB_INC_FILES
	${LIB_INC_FILES}
	include/${LIB_NAME}/debug/instrumentor.hpp
	include/${LIB_NAME}/debug/logger.hpp
)
