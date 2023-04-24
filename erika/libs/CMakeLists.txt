set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

# GLM
# https://github.com/g-truc/glm
target_include_directories(${LIB_NAME} SYSTEM PUBLIC "glm")
