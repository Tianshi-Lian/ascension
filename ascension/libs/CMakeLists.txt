set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

# SDL
# https://github.com/libsdl-org/SDL
set(SDL_LIBC ON CACHE BOOL "" FORCE)
set(SDL_TEST OFF CACHE BOOL "" FORCE)
set(SDL_CCACHE ON CACHE BOOL "" FORCE)
set(SDL_STATIC_PIC ON CACHE BOOL "" FORCE)
add_subdirectory(sdl)

# GLM
# https://github.com/g-truc/glm
if(${CMAKE_SYSTEM_NAME} MATCHES "Android")
    add_compile_definitions(GLM_LANG_STL11_FORCED)
endif()

target_include_directories(${APP_NAME} SYSTEM PRIVATE "glm")
