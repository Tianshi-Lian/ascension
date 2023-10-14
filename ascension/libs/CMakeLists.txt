set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

# SDL
# https://github.com/libsdl-org/SDL
set(SDL_LIBC ON CACHE BOOL "" FORCE)
set(SDL_TEST OFF CACHE BOOL "" FORCE)
set(SDL_CCACHE OFF CACHE BOOL "" FORCE)
set(SDL_STATIC_PIC ON CACHE BOOL "" FORCE)
add_subdirectory(sdl EXCLUDE_FROM_ALL)

# json5
add_library(json5 INTERFACE)
target_include_directories("json5/include" SYSTEM INTERFACE "json5")
