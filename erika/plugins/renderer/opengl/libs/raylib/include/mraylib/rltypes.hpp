/**********************************************************************************************
 *
 *   raylib v4.6-dev - A simple and easy-to-use library to enjoy videogames programming (www.raylib.com)
 *
 *   FEATURES:
 *       - NO external dependencies, all required libraries included with raylib
 *       - Multiplatform: Windows, Linux, FreeBSD, OpenBSD, NetBSD, DragonFly,
 *                        MacOS, Haiku, Android, Raspberry Pi, DRM native, HTML5.
 *       - Written in plain C code (C99) in PascalCase/camelCase notation
 *       - Hardware accelerated with OpenGL (1.1, 2.1, 3.3, 4.3 or ES2 - choose at compile)
 *       - Unique OpenGL abstraction layer (usable as standalone module): [rlgl]
 *       - Multiple Fonts formats supported (TTF, XNA fonts, AngelCode fonts)
 *       - Outstanding texture formats support, including compressed formats (DXT, ETC, ASTC)
 *       - Full 3d support for 3d Shapes, Models, Billboards, Heightmaps and more!
 *       - Flexible Materials system, supporting classic maps and PBR maps
 *       - Animated 3D models supported (skeletal bones animation) (IQM)
 *       - Shaders support, including Model shaders and Postprocessing shaders
 *       - Powerful math module for Vector, Matrix and Quaternion operations: [raymath]
 *       - Audio loading and playing with streaming support (WAV, OGG, MP3, FLAC, XM, MOD)
 *       - VR stereo rendering with configurable HMD device parameters
 *       - Bindings to multiple programming languages available!
 *
 *   NOTES:
 *       - One default Font is loaded on InitWindow()->LoadFontDefault() [core, text]
 *       - One default Texture2D is loaded on rlglInit(), 1x1 white pixel R8G8B8A8 [rlgl] (OpenGL 3.3 or ES2)
 *       - One default Shader is loaded on rlglInit()->rlLoadShaderDefault() [rlgl] (OpenGL 3.3 or ES2)
 *       - One default RenderBatch is loaded on rlglInit()->rlLoadRenderBatch() [rlgl] (OpenGL 3.3 or ES2)
 *
 *   DEPENDENCIES (included):
 *       [rcore] rglfw (Camilla Löwy - github.com/glfw/glfw) for window/context management and input (PLATFORM_DESKTOP)
 *       [rlgl] glad (David Herberth - github.com/Dav1dde/glad) for OpenGL 3.3 extensions loading (PLATFORM_DESKTOP)
 *       [raudio] miniaudio (David Reid - github.com/mackron/miniaudio) for audio device/context management
 *
 *   OPTIONAL DEPENDENCIES (included):
 *       [rcore] msf_gif (Miles Fogle) for GIF recording
 *       [rcore] sinfl (Micha Mettke) for DEFLATE decompression algorithm
 *       [rcore] sdefl (Micha Mettke) for DEFLATE compression algorithm
 *       [rtextures] stb_image (Sean Barret) for images loading (BMP, TGA, PNG, JPEG, HDR...)
 *       [rtextures] stb_image_write (Sean Barret) for image writing (BMP, TGA, PNG, JPG)
 *       [rtextures] stb_image_resize (Sean Barret) for image resizing algorithms
 *       [rtext] stb_truetype (Sean Barret) for ttf fonts loading
 *       [rtext] stb_rect_pack (Sean Barret) for rectangles packing
 *       [rmodels] par_shapes (Philip Rideout) for parametric 3d shapes generation
 *       [rmodels] tinyobj_loader_c (Syoyo Fujita) for models loading (OBJ, MTL)
 *       [rmodels] cgltf (Johannes Kuhlmann) for models loading (glTF)
 *       [rmodels] Model3D (bzt) for models loading (M3D, https://bztsrc.gitlab.io/model3d)
 *       [raudio] dr_wav (David Reid) for WAV audio file loading
 *       [raudio] dr_flac (David Reid) for FLAC audio file loading
 *       [raudio] dr_mp3 (David Reid) for MP3 audio file loading
 *       [raudio] stb_vorbis (Sean Barret) for OGG audio loading
 *       [raudio] jar_xm (Joshua Reisenauer) for XM audio module loading
 *       [raudio] jar_mod (Joshua Reisenauer) for MOD audio module loading
 *
 *
 *   LICENSE: zlib/libpng
 *
 *   raylib is licensed under an unmodified zlib/libpng license, which is an OSI-certified,
 *   BSD-like license that allows static linking with closed source software:
 *
 *   Copyright (c) 2013-2023 Ramon Santamaria (@raysan5)
 *
 *   This software is provided "as-is", without any express or implied warranty. In no event
 *   will the authors be held liable for any damages arising from the use of this software.
 *
 *   Permission is granted to anyone to use this software for any purpose, including commercial
 *   applications, and to alter it and redistribute it freely, subject to the following restrictions:
 *
 *     1. The origin of this software must not be misrepresented; you must not claim that you
 *     wrote the original software. If you use this software in a product, an acknowledgment
 *     in the product documentation would be appreciated but is not required.
 *
 *     2. Altered source versions must be plainly marked as such, and must not be misrepresented
 *     as being the original software.
 *
 *     3. This notice may not be removed or altered from any source distribution.
 *
 **********************************************************************************************/

#ifndef RAYLIB_H
#define RAYLIB_H

#include <array>

#define RAYLIB_VERSION_MAJOR 4
#define RAYLIB_VERSION_MINOR 6
#define RAYLIB_VERSION_PATCH 0
#define RAYLIB_VERSION "4.6-dev"

// Function specifiers in case library is build/used as a shared library (Windows)
// NOTE: Microsoft specifiers to tell compiler that symbols are imported/exported from a .dll
#if defined(_WIN32)
#if defined(BUILD_LIBTYPE_SHARED)
#if defined(__TINYC__)
#define __declspec(x) __attribute__((x))
#endif
#define RLAPI __declspec(dllexport) // We are building the library as a Win32 shared library (.dll)
#elif defined(USE_LIBTYPE_SHARED)
#define RLAPI __declspec(dllimport) // We are using the library as a Win32 shared library (.dll)
#endif
#endif

//----------------------------------------------------------------------------------
// Some basic Defines
//----------------------------------------------------------------------------------
#ifndef PI
#define PI 3.14159265358979323846f
#endif
#ifndef DEG2RAD
#define DEG2RAD (PI / 180.0f)
#endif
#ifndef RAD2DEG
#define RAD2DEG (180.0f / PI)
#endif

#ifndef EPSILON
#define EPSILON 0.000001f
#endif

// Get float vector for Matrix
#ifndef MatrixToFloat
#define MatrixToFloat(mat) (MatrixToFloatV(mat).v)
#endif

// Get float vector for Vector3
#ifndef Vector3ToFloat
#define Vector3ToFloat(vec) (Vector3ToFloatV(vec).v)
#endif

// Support TRACELOG macros
#ifndef TRACELOG
#define TRACELOG(level, ...) (void)0
#define TRACELOGD(...) (void)0
#endif

// Allow custom memory allocators
// NOTE: Require recompiling raylib sources
#ifndef RL_MALLOC
#define RL_MALLOC(sz) malloc(sz)
#endif
#ifndef RL_CALLOC
#define RL_CALLOC(n, sz) calloc(n, sz)
#endif
#ifndef RL_REALLOC
#define RL_REALLOC(ptr, sz) realloc(ptr, sz)
#endif
#ifndef RL_FREE
#define RL_FREE(ptr) free(ptr)
#endif

// NOTE: MSVC C++ compiler does not support compound literals (C99 feature)
// Plain structures in C++ (without constructors) can be initialized with { }
#if defined(__cplusplus)
#define CLITERAL(type) type
#else
#define CLITERAL(type) (type)
#endif

// NOTE: We set some defines with some data types declared by raylib
// Other modules (raymath, rlgl) also require some of those types, so,
// to be able to use those other modules as standalone (not depending on raylib)
// this defines are very useful for internal check and avoid type (re)definitions
#define RL_COLOR_TYPE
#define RL_RECTANGLE_TYPE
#define RL_VECTOR2_TYPE
#define RL_VECTOR3_TYPE
#define RL_VECTOR4_TYPE
#define RL_QUATERNION_TYPE
#define RL_MATRIX_TYPE

// Some Basic Colors
// NOTE: Custom raylib color palette for amazing visuals on WHITE background
#define LIGHTGRAY                                                                                                              \
    CLITERAL(Color)                                                                                                            \
    {                                                                                                                          \
        200, 200, 200, 255                                                                                                     \
    } // Light Gray
#define GRAY                                                                                                                   \
    CLITERAL(Color)                                                                                                            \
    {                                                                                                                          \
        130, 130, 130, 255                                                                                                     \
    } // Gray
#define DARKGRAY                                                                                                               \
    CLITERAL(Color)                                                                                                            \
    {                                                                                                                          \
        80, 80, 80, 255                                                                                                        \
    } // Dark Gray
#define YELLOW                                                                                                                 \
    CLITERAL(Color)                                                                                                            \
    {                                                                                                                          \
        253, 249, 0, 255                                                                                                       \
    } // Yellow
#define GOLD                                                                                                                   \
    CLITERAL(Color)                                                                                                            \
    {                                                                                                                          \
        255, 203, 0, 255                                                                                                       \
    } // Gold
#define ORANGE                                                                                                                 \
    CLITERAL(Color)                                                                                                            \
    {                                                                                                                          \
        255, 161, 0, 255                                                                                                       \
    } // Orange
#define PINK                                                                                                                   \
    CLITERAL(Color)                                                                                                            \
    {                                                                                                                          \
        255, 109, 194, 255                                                                                                     \
    } // Pink
#define RED                                                                                                                    \
    CLITERAL(Color)                                                                                                            \
    {                                                                                                                          \
        230, 41, 55, 255                                                                                                       \
    } // Red
#define MAROON                                                                                                                 \
    CLITERAL(Color)                                                                                                            \
    {                                                                                                                          \
        190, 33, 55, 255                                                                                                       \
    } // Maroon
#define GREEN                                                                                                                  \
    CLITERAL(Color)                                                                                                            \
    {                                                                                                                          \
        0, 228, 48, 255                                                                                                        \
    } // Green
#define LIME                                                                                                                   \
    CLITERAL(Color)                                                                                                            \
    {                                                                                                                          \
        0, 158, 47, 255                                                                                                        \
    } // Lime
#define DARKGREEN                                                                                                              \
    CLITERAL(Color)                                                                                                            \
    {                                                                                                                          \
        0, 117, 44, 255                                                                                                        \
    } // Dark Green
#define SKYBLUE                                                                                                                \
    CLITERAL(Color)                                                                                                            \
    {                                                                                                                          \
        102, 191, 255, 255                                                                                                     \
    } // Sky Blue
#define BLUE                                                                                                                   \
    CLITERAL(Color)                                                                                                            \
    {                                                                                                                          \
        0, 121, 241, 255                                                                                                       \
    } // Blue
#define DARKBLUE                                                                                                               \
    CLITERAL(Color)                                                                                                            \
    {                                                                                                                          \
        0, 82, 172, 255                                                                                                        \
    } // Dark Blue
#define PURPLE                                                                                                                 \
    CLITERAL(Color)                                                                                                            \
    {                                                                                                                          \
        200, 122, 255, 255                                                                                                     \
    } // Purple
#define VIOLET                                                                                                                 \
    CLITERAL(Color)                                                                                                            \
    {                                                                                                                          \
        135, 60, 190, 255                                                                                                      \
    } // Violet
#define DARKPURPLE                                                                                                             \
    CLITERAL(Color)                                                                                                            \
    {                                                                                                                          \
        112, 31, 126, 255                                                                                                      \
    } // Dark Purple
#define BEIGE                                                                                                                  \
    CLITERAL(Color)                                                                                                            \
    {                                                                                                                          \
        211, 176, 131, 255                                                                                                     \
    } // Beige
#define BROWN                                                                                                                  \
    CLITERAL(Color)                                                                                                            \
    {                                                                                                                          \
        127, 106, 79, 255                                                                                                      \
    } // Brown
#define DARKBROWN                                                                                                              \
    CLITERAL(Color)                                                                                                            \
    {                                                                                                                          \
        76, 63, 47, 255                                                                                                        \
    } // Dark Brown

#define WHITE                                                                                                                  \
    CLITERAL(Color)                                                                                                            \
    {                                                                                                                          \
        255, 255, 255, 255                                                                                                     \
    } // White
#define BLACK                                                                                                                  \
    CLITERAL(Color)                                                                                                            \
    {                                                                                                                          \
        0, 0, 0, 255                                                                                                           \
    } // Black
#define BLANK                                                                                                                  \
    CLITERAL(Color)                                                                                                            \
    {                                                                                                                          \
        0, 0, 0, 0                                                                                                             \
    } // Blank (Transparent)
#define MAGENTA                                                                                                                \
    CLITERAL(Color)                                                                                                            \
    {                                                                                                                          \
        255, 0, 255, 255                                                                                                       \
    } // Magenta
#define RAYWHITE                                                                                                               \
    CLITERAL(Color)                                                                                                            \
    {                                                                                                                          \
        245, 245, 245, 255                                                                                                     \
    } // My own White (raylib logo)

// Cubemap layouts
enum rlCubemapLayout {
    RL_CUBEMAP_LAYOUT_AUTO_DETECT = 0,     // Automatically detect layout type
    RL_CUBEMAP_LAYOUT_LINE_VERTICAL,       // Layout is defined by a vertical line with faces
    RL_CUBEMAP_LAYOUT_LINE_HORIZONTAL,     // Layout is defined by a horizontal line with faces
    RL_CUBEMAP_LAYOUT_CROSS_THREE_BY_FOUR, // Layout is defined by a 3x4 cross with cubemap faces
    RL_CUBEMAP_LAYOUT_CROSS_FOUR_BY_THREE, // Layout is defined by a 4x3 cross with cubemap faces
    RL_CUBEMAP_LAYOUT_PANORAMA             // Layout is defined by a panorama image (equirrectangular map)
};

// N-patch layout
enum rlNPatchLayout {
    RL_NPATCH_NINE_PATCH = 0,        // Npatch layout: 3x3 tiles
    RL_NPATCH_THREE_PATCH_VERTICAL,  // Npatch layout: 1x3 tiles
    RL_NPATCH_THREE_PATCH_HORIZONTAL // Npatch layout: 3x1 tiles
};

//----------------------------------------------------------------------------------
// Structures Definition
//----------------------------------------------------------------------------------
// Boolean type
#if (defined(__STDC__) && __STDC_VERSION__ >= 199901L) || (defined(_MSC_VER) && _MSC_VER >= 1800)
#include <stdbool.h>
#elif !defined(__cplusplus) && !defined(bool)
typedef enum bool {
    false = 0,
    true = !false
} bool;
#define RL_BOOL_TYPE
#endif

// Vector2 type
struct Vector2 {
    float x;
    float y;
};

// Vector3 type
struct Vector3 {
    float x;
    float y;
    float z;
};

// Vector4 type
struct Vector4 {
    float x;
    float y;
    float z;
    float w;
};

// Quaternion type
using Quaternion = Vector4;

// Matrix type (OpenGL style 4x4 - right handed, column major)
struct Matrix {
    float m0, m4, m8, m12;  // Matrix first row (4 components)
    float m1, m5, m9, m13;  // Matrix second row (4 components)
    float m2, m6, m10, m14; // Matrix third row (4 components)
    float m3, m7, m11, m15; // Matrix fourth row (4 components)
};

// NOTE: Helper types to be used instead of array return types for *ToFloat functions
struct float3 {
    std::array<float, 3> v;
};

struct float16 {
    std::array<float, 16> v; // NOLINT
};

// Color, 4 components, R8G8B8A8 (32bit)
struct Color {
    unsigned char r; // Color red value
    unsigned char g; // Color green value
    unsigned char b; // Color blue value
    unsigned char a; // Color alpha value
};

// Rectangle, 4 components
struct Rectangle {
    float x;      // Rectangle top-left corner position x
    float y;      // Rectangle top-left corner position y
    float width;  // Rectangle width
    float height; // Rectangle height
};

// Image, pixel data stored in CPU memory (RAM)
struct Image {
    void* data;  // Image raw data
    int width;   // Image base width
    int height;  // Image base height
    int mipmaps; // Mipmap levels, 1 by default
    int format;  // Data format (PixelFormat type)
};

// Texture, tex data stored in GPU memory (VRAM)
struct Texture {
    unsigned int id; // OpenGL texture id
    int width;       // Texture base width
    int height;      // Texture base height
    int mipmaps;     // Mipmap levels, 1 by default
    int format;      // Data format (PixelFormat type)
};

// Texture2D, same as Texture
using Texture2D = Texture;

// TextureCubemap, same as Texture
using TextureCubemap = Texture;

// RenderTexture, fbo for texture rendering
struct RenderTexture {
    unsigned int id; // OpenGL framebuffer object id
    Texture texture; // Color buffer attachment texture
    Texture depth;   // Depth buffer attachment texture
};

// RenderTexture2D, same as RenderTexture
using RenderTexture2D = RenderTexture;

// NPatchInfo, n-patch layout info
struct NPatchInfo {
    Rectangle source; // Texture source rectangle
    int left;         // Left border offset
    int top;          // Top border offset
    int right;        // Right border offset
    int bottom;       // Bottom border offset
    int layout;       // Layout of the n-patch: 3x3, 1x3 or 3x1
};

// GlyphInfo, font characters glyphs info
struct GlyphInfo {
    int value;    // Character value (Unicode)
    int offsetX;  // Character offset X when drawing
    int offsetY;  // Character offset Y when drawing
    int advanceX; // Character advance position X
    Image image;  // Character image data
};

// Font, font texture and GlyphInfo array data
struct Font {
    int baseSize;      // Base size (default chars height)
    int glyphCount;    // Number of glyph characters
    int glyphPadding;  // Padding around the glyph characters
    Texture2D texture; // Texture atlas containing the glyphs
    Rectangle* recs;   // Rectangles in texture for the glyphs
    GlyphInfo* glyphs; // Glyphs info data
};

// Shader
struct Shader {
    unsigned int id; // Shader program id
    int* locs;       // Shader locations array (RL_MAX_SHADER_LOCATIONS)
};

// MaterialMap
struct MaterialMap {
    Texture2D texture; // Material map texture
    Color color;       // Material map color
    float value;       // Material map value
};

// Material, includes shader and maps
struct Material {
    Shader shader;               // Material shader
    MaterialMap* maps;           // Material maps array (MAX_MATERIAL_MAPS)
    std::array<float, 4> params; // Material generic parameters (if required)
};

// Transform, vertex transformation data
struct Transform {
    Vector3 translation; // Translation
    Quaternion rotation; // Rotation
    Vector3 scale;       // Scale
};

// Bone, skeletal animation bone
struct BoneInfo {
    std::array<char, 32> name; // NOLINT Bone name
    int parent;                // Bone parent
};

// Ray, ray for raycasting
struct Ray {
    Vector3 position;  // Ray position (origin)
    Vector3 direction; // Ray direction
};

// RayCollision, ray hit information
struct RayCollision {
    bool hit;       // Did the ray hit something?
    float distance; // Distance to the nearest hit
    Vector3 point;  // Point of the nearest hit
    Vector3 normal; // Surface normal of hit
};

// BoundingBox
struct BoundingBox {
    Vector3 min; // Minimum vertex box-corner
    Vector3 max; // Maximum vertex box-corner
};

// VrDeviceInfo, Head-Mounted-Display device parameters
struct VrDeviceInfo {
    int hResolution;                           // Horizontal resolution in pixels
    int vResolution;                           // Vertical resolution in pixels
    float hScreenSize;                         // Horizontal size in meters
    float vScreenSize;                         // Vertical size in meters
    float vScreenCenter;                       // Screen center in meters
    float eyeToScreenDistance;                 // Distance between eye and display in meters
    float lensSeparationDistance;              // Lens separation distance in meters
    float interpupillaryDistance;              // IPD (distance between pupils) in meters
    std::array<float, 4> lensDistortionValues; // Lens distortion constant parameters
    std::array<float, 4> chromaAbCorrection;   // Chromatic aberration correction parameters
};

// VrStereoConfig, VR stereo rendering configuration for simulator
struct VrStereoConfig {
    std::array<Matrix, 2> projection;       // VR projection matrices (per eye)
    std::array<Matrix, 2> viewOffset;       // VR view offset matrices (per eye)
    std::array<float, 2> leftLensCenter;    // VR left lens center
    std::array<float, 2> rightLensCenter;   // VR right lens center
    std::array<float, 2> leftScreenCenter;  // VR left screen center
    std::array<float, 2> rightScreenCenter; // VR right screen center
    std::array<float, 2> scale;             // VR distortion scale
    std::array<float, 2> scaleIn;           // VR distortion scale in
};

// File path list
struct FilePathList {
    unsigned int capacity; // Filepaths max entries
    unsigned int count;    // Filepaths entries count
    char** paths;          // Filepaths entries
};

#endif // RAYLIB_H
