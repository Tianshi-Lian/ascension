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

#pragma GCC system_header

#ifndef RAYLIB_H
#define RAYLIB_H

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

#ifndef RLAPI
#define RLAPI // Functions defined as 'extern' by default (implicit specifiers)
#endif

// Support TRACELOG macros
#ifndef TRACELOG
#define TRACELOG(level, ...) (void)0
#define TRACELOGD(...) (void)0
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
typedef struct Vector2 {
    float x;
    float y;
} Vector2;

// Vector3 type
typedef struct Vector3 {
    float x;
    float y;
    float z;
} Vector3;

// Vector4 type
typedef struct Vector4 {
    float x;
    float y;
    float z;
    float w;
} Vector4;

// Quaternion type
typedef Vector4 Quaternion;

// Matrix type (OpenGL style 4x4 - right handed, column major)
typedef struct Matrix {
    float m0, m4, m8, m12;  // Matrix first row (4 components)
    float m1, m5, m9, m13;  // Matrix second row (4 components)
    float m2, m6, m10, m14; // Matrix third row (4 components)
    float m3, m7, m11, m15; // Matrix fourth row (4 components)
} Matrix;

// NOTE: Helper types to be used instead of array return types for *ToFloat functions
typedef struct float3 {
    float v[3];
} float3;

typedef struct float16 {
    float v[16];
} float16;

// Color, 4 components, R8G8B8A8 (32bit)
typedef struct Color {
    unsigned char r; // Color red value
    unsigned char g; // Color green value
    unsigned char b; // Color blue value
    unsigned char a; // Color alpha value
} Color;

// Rectangle, 4 components
typedef struct Rectangle {
    float x;      // Rectangle top-left corner position x
    float y;      // Rectangle top-left corner position y
    float width;  // Rectangle width
    float height; // Rectangle height
} Rectangle;

// Image, pixel data stored in CPU memory (RAM)
typedef struct Image {
    void* data;  // Image raw data
    int width;   // Image base width
    int height;  // Image base height
    int mipmaps; // Mipmap levels, 1 by default
    int format;  // Data format (PixelFormat type)
} Image;

// Texture, tex data stored in GPU memory (VRAM)
typedef struct Texture {
    unsigned int id; // OpenGL texture id
    int width;       // Texture base width
    int height;      // Texture base height
    int mipmaps;     // Mipmap levels, 1 by default
    int format;      // Data format (PixelFormat type)
} Texture;

// Texture2D, same as Texture
typedef Texture Texture2D;

// TextureCubemap, same as Texture
typedef Texture TextureCubemap;

// RenderTexture, fbo for texture rendering
typedef struct RenderTexture {
    unsigned int id; // OpenGL framebuffer object id
    Texture texture; // Color buffer attachment texture
    Texture depth;   // Depth buffer attachment texture
} RenderTexture;

// RenderTexture2D, same as RenderTexture
typedef RenderTexture RenderTexture2D;

// NPatchInfo, n-patch layout info
typedef struct NPatchInfo {
    Rectangle source; // Texture source rectangle
    int left;         // Left border offset
    int top;          // Top border offset
    int right;        // Right border offset
    int bottom;       // Bottom border offset
    int layout;       // Layout of the n-patch: 3x3, 1x3 or 3x1
} NPatchInfo;

// GlyphInfo, font characters glyphs info
typedef struct GlyphInfo {
    int value;    // Character value (Unicode)
    int offsetX;  // Character offset X when drawing
    int offsetY;  // Character offset Y when drawing
    int advanceX; // Character advance position X
    Image image;  // Character image data
} GlyphInfo;

// Font, font texture and GlyphInfo array data
typedef struct Font {
    int baseSize;      // Base size (default chars height)
    int glyphCount;    // Number of glyph characters
    int glyphPadding;  // Padding around the glyph characters
    Texture2D texture; // Texture atlas containing the glyphs
    Rectangle* recs;   // Rectangles in texture for the glyphs
    GlyphInfo* glyphs; // Glyphs info data
} Font;

// Shader
typedef struct Shader {
    unsigned int id; // Shader program id
    int* locs;       // Shader locations array (RL_MAX_SHADER_LOCATIONS)
} Shader;

// MaterialMap
typedef struct MaterialMap {
    Texture2D texture; // Material map texture
    Color color;       // Material map color
    float value;       // Material map value
} MaterialMap;

// Material, includes shader and maps
typedef struct Material {
    Shader shader;     // Material shader
    MaterialMap* maps; // Material maps array (MAX_MATERIAL_MAPS)
    float params[4];   // Material generic parameters (if required)
} Material;

// Transform, vertex transformation data
typedef struct Transform {
    Vector3 translation; // Translation
    Quaternion rotation; // Rotation
    Vector3 scale;       // Scale
} Transform;

// Bone, skeletal animation bone
typedef struct BoneInfo {
    char name[32]; // Bone name
    int parent;    // Bone parent
} BoneInfo;

// Ray, ray for raycasting
typedef struct Ray {
    Vector3 position;  // Ray position (origin)
    Vector3 direction; // Ray direction
} Ray;

// RayCollision, ray hit information
typedef struct RayCollision {
    bool hit;       // Did the ray hit something?
    float distance; // Distance to the nearest hit
    Vector3 point;  // Point of the nearest hit
    Vector3 normal; // Surface normal of hit
} RayCollision;

// BoundingBox
typedef struct BoundingBox {
    Vector3 min; // Minimum vertex box-corner
    Vector3 max; // Maximum vertex box-corner
} BoundingBox;

// VrDeviceInfo, Head-Mounted-Display device parameters
typedef struct VrDeviceInfo {
    int hResolution;               // Horizontal resolution in pixels
    int vResolution;               // Vertical resolution in pixels
    float hScreenSize;             // Horizontal size in meters
    float vScreenSize;             // Vertical size in meters
    float vScreenCenter;           // Screen center in meters
    float eyeToScreenDistance;     // Distance between eye and display in meters
    float lensSeparationDistance;  // Lens separation distance in meters
    float interpupillaryDistance;  // IPD (distance between pupils) in meters
    float lensDistortionValues[4]; // Lens distortion constant parameters
    float chromaAbCorrection[4];   // Chromatic aberration correction parameters
} VrDeviceInfo;

// VrStereoConfig, VR stereo rendering configuration for simulator
typedef struct VrStereoConfig {
    Matrix projection[2];       // VR projection matrices (per eye)
    Matrix viewOffset[2];       // VR view offset matrices (per eye)
    float leftLensCenter[2];    // VR left lens center
    float rightLensCenter[2];   // VR right lens center
    float leftScreenCenter[2];  // VR left screen center
    float rightScreenCenter[2]; // VR right screen center
    float scale[2];             // VR distortion scale
    float scaleIn[2];           // VR distortion scale in
} VrStereoConfig;

// File path list
typedef struct FilePathList {
    unsigned int capacity; // Filepaths max entries
    unsigned int count;    // Filepaths entries count
    char** paths;          // Filepaths entries
} FilePathList;

//----------------------------------------------------------------------------------
// Enumerators Definition
//----------------------------------------------------------------------------------

// Shader location index
typedef enum {
    SHADER_LOC_VERTEX_POSITION = 0, // Shader location: vertex attribute: position
    SHADER_LOC_VERTEX_TEXCOORD01,   // Shader location: vertex attribute: texcoord01
    SHADER_LOC_VERTEX_TEXCOORD02,   // Shader location: vertex attribute: texcoord02
    SHADER_LOC_VERTEX_NORMAL,       // Shader location: vertex attribute: normal
    SHADER_LOC_VERTEX_TANGENT,      // Shader location: vertex attribute: tangent
    SHADER_LOC_VERTEX_COLOR,        // Shader location: vertex attribute: color
    SHADER_LOC_MATRIX_MVP,          // Shader location: matrix uniform: model-view-projection
    SHADER_LOC_MATRIX_VIEW,         // Shader location: matrix uniform: view (camera transform)
    SHADER_LOC_MATRIX_PROJECTION,   // Shader location: matrix uniform: projection
    SHADER_LOC_MATRIX_MODEL,        // Shader location: matrix uniform: model (transform)
    SHADER_LOC_MATRIX_NORMAL,       // Shader location: matrix uniform: normal
    SHADER_LOC_VECTOR_VIEW,         // Shader location: vector uniform: view
    SHADER_LOC_COLOR_DIFFUSE,       // Shader location: vector uniform: diffuse color
    SHADER_LOC_COLOR_SPECULAR,      // Shader location: vector uniform: specular color
    SHADER_LOC_COLOR_AMBIENT,       // Shader location: vector uniform: ambient color
    SHADER_LOC_MAP_ALBEDO,          // Shader location: sampler2d texture: albedo (same as: SHADER_LOC_MAP_DIFFUSE)
    SHADER_LOC_MAP_METALNESS,       // Shader location: sampler2d texture: metalness (same as: SHADER_LOC_MAP_SPECULAR)
    SHADER_LOC_MAP_NORMAL,          // Shader location: sampler2d texture: normal
    SHADER_LOC_MAP_ROUGHNESS,       // Shader location: sampler2d texture: roughness
    SHADER_LOC_MAP_OCCLUSION,       // Shader location: sampler2d texture: occlusion
    SHADER_LOC_MAP_EMISSION,        // Shader location: sampler2d texture: emission
    SHADER_LOC_MAP_HEIGHT,          // Shader location: sampler2d texture: height
    SHADER_LOC_MAP_CUBEMAP,         // Shader location: samplerCube texture: cubemap
    SHADER_LOC_MAP_IRRADIANCE,      // Shader location: samplerCube texture: irradiance
    SHADER_LOC_MAP_PREFILTER,       // Shader location: samplerCube texture: prefilter
    SHADER_LOC_MAP_BRDF             // Shader location: sampler2d texture: brdf
} ShaderLocationIndex;

#define SHADER_LOC_MAP_DIFFUSE SHADER_LOC_MAP_ALBEDO
#define SHADER_LOC_MAP_SPECULAR SHADER_LOC_MAP_METALNESS

// Shader uniform data type
typedef enum {
    SHADER_UNIFORM_FLOAT = 0, // Shader uniform type: float
    SHADER_UNIFORM_VEC2,      // Shader uniform type: vec2 (2 float)
    SHADER_UNIFORM_VEC3,      // Shader uniform type: vec3 (3 float)
    SHADER_UNIFORM_VEC4,      // Shader uniform type: vec4 (4 float)
    SHADER_UNIFORM_INT,       // Shader uniform type: int
    SHADER_UNIFORM_IVEC2,     // Shader uniform type: ivec2 (2 int)
    SHADER_UNIFORM_IVEC3,     // Shader uniform type: ivec3 (3 int)
    SHADER_UNIFORM_IVEC4,     // Shader uniform type: ivec4 (4 int)
    SHADER_UNIFORM_SAMPLER2D  // Shader uniform type: sampler2d
} ShaderUniformDataType;

// Shader attribute data types
typedef enum {
    SHADER_ATTRIB_FLOAT = 0, // Shader attribute type: float
    SHADER_ATTRIB_VEC2,      // Shader attribute type: vec2 (2 float)
    SHADER_ATTRIB_VEC3,      // Shader attribute type: vec3 (3 float)
    SHADER_ATTRIB_VEC4       // Shader attribute type: vec4 (4 float)
} ShaderAttributeDataType;

// Pixel formats
// NOTE: Support depends on OpenGL version and platform
typedef enum {
    PIXELFORMAT_UNCOMPRESSED_GRAYSCALE = 1, // 8 bit per pixel (no alpha)
    PIXELFORMAT_UNCOMPRESSED_GRAY_ALPHA,    // 8*2 bpp (2 channels)
    PIXELFORMAT_UNCOMPRESSED_R5G6B5,        // 16 bpp
    PIXELFORMAT_UNCOMPRESSED_R8G8B8,        // 24 bpp
    PIXELFORMAT_UNCOMPRESSED_R5G5B5A1,      // 16 bpp (1 bit alpha)
    PIXELFORMAT_UNCOMPRESSED_R4G4B4A4,      // 16 bpp (4 bit alpha)
    PIXELFORMAT_UNCOMPRESSED_R8G8B8A8,      // 32 bpp
    PIXELFORMAT_UNCOMPRESSED_R32,           // 32 bpp (1 channel - float)
    PIXELFORMAT_UNCOMPRESSED_R32G32B32,     // 32*3 bpp (3 channels - float)
    PIXELFORMAT_UNCOMPRESSED_R32G32B32A32,  // 32*4 bpp (4 channels - float)
    PIXELFORMAT_COMPRESSED_DXT1_RGB,        // 4 bpp (no alpha)
    PIXELFORMAT_COMPRESSED_DXT1_RGBA,       // 4 bpp (1 bit alpha)
    PIXELFORMAT_COMPRESSED_DXT3_RGBA,       // 8 bpp
    PIXELFORMAT_COMPRESSED_DXT5_RGBA,       // 8 bpp
    PIXELFORMAT_COMPRESSED_ETC1_RGB,        // 4 bpp
    PIXELFORMAT_COMPRESSED_ETC2_RGB,        // 4 bpp
    PIXELFORMAT_COMPRESSED_ETC2_EAC_RGBA,   // 8 bpp
    PIXELFORMAT_COMPRESSED_PVRT_RGB,        // 4 bpp
    PIXELFORMAT_COMPRESSED_PVRT_RGBA,       // 4 bpp
    PIXELFORMAT_COMPRESSED_ASTC_4x4_RGBA,   // 8 bpp
    PIXELFORMAT_COMPRESSED_ASTC_8x8_RGBA    // 2 bpp
} PixelFormat;

// Texture parameters: filter mode
// NOTE 1: Filtering considers mipmaps if available in the texture
// NOTE 2: Filter is accordingly set for minification and magnification
typedef enum {
    TEXTURE_FILTER_POINT = 0,       // No filter, just pixel approximation
    TEXTURE_FILTER_BILINEAR,        // Linear filtering
    TEXTURE_FILTER_TRILINEAR,       // Trilinear filtering (linear with mipmaps)
    TEXTURE_FILTER_ANISOTROPIC_4X,  // Anisotropic filtering 4x
    TEXTURE_FILTER_ANISOTROPIC_8X,  // Anisotropic filtering 8x
    TEXTURE_FILTER_ANISOTROPIC_16X, // Anisotropic filtering 16x
} TextureFilter;

// Texture parameters: wrap mode
typedef enum {
    TEXTURE_WRAP_REPEAT = 0,    // Repeats texture in tiled mode
    TEXTURE_WRAP_CLAMP,         // Clamps texture to edge pixel in tiled mode
    TEXTURE_WRAP_MIRROR_REPEAT, // Mirrors and repeats the texture in tiled mode
    TEXTURE_WRAP_MIRROR_CLAMP   // Mirrors and clamps to border the texture in tiled mode
} TextureWrap;

// Cubemap layouts
typedef enum {
    CUBEMAP_LAYOUT_AUTO_DETECT = 0,     // Automatically detect layout type
    CUBEMAP_LAYOUT_LINE_VERTICAL,       // Layout is defined by a vertical line with faces
    CUBEMAP_LAYOUT_LINE_HORIZONTAL,     // Layout is defined by a horizontal line with faces
    CUBEMAP_LAYOUT_CROSS_THREE_BY_FOUR, // Layout is defined by a 3x4 cross with cubemap faces
    CUBEMAP_LAYOUT_CROSS_FOUR_BY_THREE, // Layout is defined by a 4x3 cross with cubemap faces
    CUBEMAP_LAYOUT_PANORAMA             // Layout is defined by a panorama image (equirrectangular map)
} CubemapLayout;

// Font type, defines generation method
typedef enum {
    FONT_DEFAULT = 0, // Default font generation, anti-aliased
    FONT_BITMAP,      // Bitmap font generation, no anti-aliasing
    FONT_SDF          // SDF font generation, requires external shader
} FontType;

// Color blending modes (pre-defined)
typedef enum {
    BLEND_ALPHA = 0,         // Blend textures considering alpha (default)
    BLEND_ADDITIVE,          // Blend textures adding colors
    BLEND_MULTIPLIED,        // Blend textures multiplying colors
    BLEND_ADD_COLORS,        // Blend textures adding colors (alternative)
    BLEND_SUBTRACT_COLORS,   // Blend textures subtracting colors (alternative)
    BLEND_ALPHA_PREMULTIPLY, // Blend premultiplied textures considering alpha
    BLEND_CUSTOM,            // Blend textures using custom src/dst factors (use rlSetBlendFactors())
    BLEND_CUSTOM_SEPARATE    // Blend textures using custom rgb/alpha separate src/dst factors (use rlSetBlendFactorsSeparate())
} BlendMode;

// N-patch layout
typedef enum {
    NPATCH_NINE_PATCH = 0,        // Npatch layout: 3x3 tiles
    NPATCH_THREE_PATCH_VERTICAL,  // Npatch layout: 1x3 tiles
    NPATCH_THREE_PATCH_HORIZONTAL // Npatch layout: 3x1 tiles
} NPatchLayout;

#endif // RAYLIB_H
