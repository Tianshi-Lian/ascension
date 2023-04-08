set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

# SDL
# https://github.com/libsdl-org/SDL
set(SDL_LIBC ON CACHE BOOL "" FORCE)
set(SDL_TEST OFF CACHE BOOL "" FORCE)
set(SDL_CCACHE OFF CACHE BOOL "" FORCE)
set(SDL_STATIC_PIC ON CACHE BOOL "" FORCE)
add_subdirectory(sdl EXCLUDE_FROM_ALL)

# GLM
# https://github.com/g-truc/glm
target_include_directories(${APP_NAME} SYSTEM PRIVATE "glm")

# glew-cmake
# https://github.com/Perlmint/glew-cmake
set(glew-cmake_BUILD_SHARED OFF CACHE BOOL "" FORCE)
set(ONLY_LIBS ON CACHE BOOL "" FORCE)
set(USE_GLU ON CACHE BOOL "" FORCE)
set(CMAKE_INSTALL_LIBDIR ${CMAKE_BINARY_DIR}/dist CACHE STRING "" FORCE)
add_subdirectory(glew EXCLUDE_FROM_ALL)

# FreeType
# https://github.com/freetype/freetype
# TODO: look into getting support for some of these.
set(FT_DISABLE_HARFBUZZ ON CACHE BOOL "" FORCE)
set(FT_DISABLE_PNG ON CACHE BOOL "" FORCE)
set(FT_DISABLE_ZLIB ON CACHE BOOL "" FORCE)
set(FT_DISABLE_BZIP2 ON CACHE BOOL "" FORCE)
set(FT_DISABLE_BROTLI ON CACHE BOOL "" FORCE)
add_subdirectory(freetype EXCLUDE_FROM_ALL)

# pugixml
# https://github.com/zeux/pugixml
add_subdirectory(pugixml EXCLUDE_FROM_ALL)

# stb
# https://github.com/nothings/stb
target_include_directories(${APP_NAME} SYSTEM PRIVATE "stb")
