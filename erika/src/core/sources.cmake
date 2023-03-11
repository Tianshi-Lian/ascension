include_guard()

set(LIB_SRC_FILES
	${LIB_SRC_FILES}
	src/core/game.cpp
	src/core/engine.cpp
)

set(LIB_INC_FILES
	${LIB_INC_FILES}
	include/${LIB_NAME}/core/game.hpp
	include/${LIB_NAME}/core/engine.hpp
	include/${LIB_NAME}/core/entry.hpp
)
