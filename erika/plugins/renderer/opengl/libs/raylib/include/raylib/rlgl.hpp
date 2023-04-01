/**********************************************************************************************
 *
 *   rlgl v4.5 - A multi-OpenGL abstraction layer with an immediate-mode style API
 *
 *   An abstraction layer for multiple OpenGL versions (1.1, 2.1, 3.3 Core, 4.3 Core, ES 2.0)
 *   that provides a pseudo-OpenGL 1.1 immediate-mode style API (rlVertex_val, rlTranslate, rlRotate...)
 *
 *   When choosing an OpenGL backend different than OpenGL 1.1, some internal buffer are
 *   initialized on rlglInit() to accumulate vertex data.
 *
 *   When an internal state change is required all the stored vertex data is renderer in batch,
 *   additionally_val, rlDrawRenderBatchActive() could be called to force flushing of the batch.
 *
 *   Some additional resources are also loaded for convenience, here the complete list:
 *      - Default batch (RLGL.defaultBatch): RenderBatch system to accumulate vertex data
 *      - Default texture (RLGL.defaultTexture_id): 1x1 white pixel R8G8B8A8
 *      - Default shader (RLGL.State.defaultShader_id, RLGL.State.defaultShaderLocs)
 *
 *   Internal buffer (and additional resources) must be manually unloaded calling rlglClose().
 *
 *
 *   CONFIGURATION:
 *
 *   #define GRAPHICS_API_OPENGL_11
 *   #define GRAPHICS_API_OPENGL_21
 *   #define GRAPHICS_API_OPENGL_33
 *   #define GRAPHICS_API_OPENGL_43
 *   #define GRAPHICS_API_OPENGL_ES2
 *       Use selected OpenGL graphics backend, should be supported by platform
 *       Those preprocessor defines are only used on rlgl module, if OpenGL version is
 *       required by any other module, use rlGetVersion() to check it
 *
 *   #define RLGL_IMPLEMENTATION
 *       Generates the implementation of the library into the included file.
 *       If not defined, the library is in header only mode and can be included in other headers
 *       or source files without problems. But only ONE file should hold the implementation.
 *
 *   #define RLGL_RENDER_TEXTURES_HINT
 *       Enable framebuffer objects (fbo) support (enabled by default)
 *       Some GPUs could not support them despite the OpenGL version
 *
 *   #define RLGL_SHOW_GL_DETAILS_INFO
 *       Show OpenGL extensions and capabilities detailed logs on init
 *
 *   #define RLGL_ENABLE_OPENGL_DEBUG_CONTEXT
 *       Enable debug context (only available on OpenGL 4.3)
 *
 *   rlgl capabilities could be customized just defining some internal
 *   values before library inclusion (default values listed):
 *
 *   #define RL_DEFAULT_BATCH_BUFFER_ELEMENTS   8192    // Default internal render batch elements limits
 *   #define RL_DEFAULT_BATCH_BUFFERS              1    // Default number of batch buffers (multi-buffering)
 *   #define RL_DEFAULT_BATCH_DRAWCALLS          256    // Default number of batch draw calls (by state changes: mode, texture)
 *   #define RL_DEFAULT_BATCH_MAX_TEXTURE_UNITS    4    // Maximum number of textures units that can be activated on batch
 *drawing (SetShaderValueTexture())
 *
 *   #define RL_MAX_MATRIX_STACK_SIZE             32    // Maximum size of internal Matrix stack
 *   #define RL_MAX_SHADER_LOCATIONS              32    // Maximum number of shader locations supported
 *   #define RL_CULL_DISTANCE_NEAR              0.01    // Default projection matrix near cull distance
 *   #define RL_CULL_DISTANCE_FAR             1000.0    // Default projection matrix far cull distance
 *
 *   When loading a shader, the following vertex attribute and uniform
 *   location names are tried to be set automatically:
 *
 *   #define RL_DEFAULT_SHADER_ATTRIB_NAME_POSITION     "vertexPosition"    // Bound by default to shader location: 0
 *   #define RL_DEFAULT_SHADER_ATTRIB_NAME_TEXCOORD     "vertexTexCoord"    // Bound by default to shader location: 1
 *   #define RL_DEFAULT_SHADER_ATTRIB_NAME_NORMAL       "vertexNormal"      // Bound by default to shader location: 2
 *   #define RL_DEFAULT_SHADER_ATTRIB_NAME_COLOR        "vertexColor"       // Bound by default to shader location: 3
 *   #define RL_DEFAULT_SHADER_ATTRIB_NAME_TANGENT      "vertexTangent"     // Bound by default to shader location: 4
 *   #define RL_DEFAULT_SHADER_UNIFORM_NAME_MVP         "mvp"               // model-view-projection matrix
 *   #define RL_DEFAULT_SHADER_UNIFORM_NAME_VIEW        "matView"           // view matrix
 *   #define RL_DEFAULT_SHADER_UNIFORM_NAME_PROJECTION  "matProjection"     // projection matrix
 *   #define RL_DEFAULT_SHADER_UNIFORM_NAME_MODEL       "matModel"          // model matrix
 *   #define RL_DEFAULT_SHADER_UNIFORM_NAME_NORMAL      "matNormal"         // normal matrix (transpose(inverse(matModelView))
 *   #define RL_DEFAULT_SHADER_UNIFORM_NAME_COLOR       "colDiffuse"        // color diffuse (base tint color, multiplied by
 *texture color) #define RL_DEFAULT_SHADER_SAMPLER2D_NAME_TEXTURE0  "texture0"          // texture0 (texture slot active 0)
 *   #define RL_DEFAULT_SHADER_SAMPLER2D_NAME_TEXTURE1  "texture1"          // texture1 (texture slot active 1)
 *   #define RL_DEFAULT_SHADER_SAMPLER2D_NAME_TEXTURE2  "texture2"          // texture2 (texture slot active 2)
 *
 *   DEPENDENCIES:
 *
 *      - OpenGL libraries (depending on platform and OpenGL version selected)
 *      - GLAD OpenGL extensions loading library (only for OpenGL 3.3 Core, 4.3 Core)
 *
 *
 *   LICENSE: zlib/libpng
 *
 *   Copyright (c) 2014-2023 Ramon Santamaria (@raysan5)
 *
 *   This software is provided "as-is", without any express or implied warranty. In no event
 *   will the authors be held liable for any damages arising from the use of this software.
 *
 *   Permission is granted to anyone to use this software for any purpose, including commercial
 *   applications, and to alter it and redistribute it freely_val, subject to the following restrictions:
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

#ifndef RLGL_H
#define RLGL_H

#include <array>

// Check if config flags have been externally provided on compilation line
#if !defined(EXTERNAL_CONFIG_FLAGS)
#include "config.hpp" // Defines module configuration flags
#endif

#include "rltypes.hpp"

// Security check in case no GRAPHICS_API_OPENGL_* defined
#if !defined(GRAPHICS_API_OPENGL_11) && !defined(GRAPHICS_API_OPENGL_21) && !defined(GRAPHICS_API_OPENGL_33) &&                \
    !defined(GRAPHICS_API_OPENGL_43) && !defined(GRAPHICS_API_OPENGL_ES2)
#define GRAPHICS_API_OPENGL_33
#endif

// Security check in case multiple GRAPHICS_API_OPENGL_* defined
#if defined(GRAPHICS_API_OPENGL_11)
#if defined(GRAPHICS_API_OPENGL_21)
#undef GRAPHICS_API_OPENGL_21
#endif
#if defined(GRAPHICS_API_OPENGL_33)
#undef GRAPHICS_API_OPENGL_33
#endif
#if defined(GRAPHICS_API_OPENGL_43)
#undef GRAPHICS_API_OPENGL_43
#endif
#if defined(GRAPHICS_API_OPENGL_ES2)
#undef GRAPHICS_API_OPENGL_ES2
#endif
#endif

// OpenGL 2.1 uses most of OpenGL 3.3 Core functionality
// WARNING: Specific parts are checked with #if defines
#if defined(GRAPHICS_API_OPENGL_21)
#define GRAPHICS_API_OPENGL_33
#endif

// OpenGL 4.3 uses OpenGL 3.3 Core functionality
#if defined(GRAPHICS_API_OPENGL_43)
#define GRAPHICS_API_OPENGL_33
#endif

// Primitive assembly draw modes
#define RL_LINES 0x0001     // GL_LINES
#define RL_TRIANGLES 0x0004 // GL_TRIANGLES
#define RL_QUADS 0x0007     // GL_QUADS

// Texture parameters (equivalent to OpenGL defines)
#define RL_TEXTURE_WRAP_S 0x2802     // GL_TEXTURE_WRAP_S
#define RL_TEXTURE_WRAP_T 0x2803     // GL_TEXTURE_WRAP_T
#define RL_TEXTURE_MAG_FILTER 0x2800 // GL_TEXTURE_MAG_FILTER
#define RL_TEXTURE_MIN_FILTER 0x2801 // GL_TEXTURE_MIN_FILTER

#define RL_TEXTURE_FILTER_NEAREST 0x2600            // GL_NEAREST
#define RL_TEXTURE_FILTER_LINEAR 0x2601             // GL_LINEAR
#define RL_TEXTURE_FILTER_MIP_NEAREST 0x2700        // GL_NEAREST_MIPMAP_NEAREST
#define RL_TEXTURE_FILTER_NEAREST_MIP_LINEAR 0x2702 // GL_NEAREST_MIPMAP_LINEAR
#define RL_TEXTURE_FILTER_LINEAR_MIP_NEAREST 0x2701 // GL_LINEAR_MIPMAP_NEAREST
#define RL_TEXTURE_FILTER_MIP_LINEAR 0x2703         // GL_LINEAR_MIPMAP_LINEAR
#define RL_TEXTURE_FILTER_ANISOTROPIC 0x3000        // Anisotropic filter (custom identifier)
#define RL_TEXTURE_MIPMAP_BIAS_RATIO 0x4000         // Texture mipmap bias, percentage ratio (custom identifier)

#define RL_TEXTURE_WRAP_REPEAT 0x2901        // GL_REPEAT
#define RL_TEXTURE_WRAP_CLAMP 0x812F         // GL_CLAMP_TO_EDGE
#define RL_TEXTURE_WRAP_MIRROR_REPEAT 0x8370 // GL_MIRRORED_REPEAT
#define RL_TEXTURE_WRAP_MIRROR_CLAMP 0x8742  // GL_MIRROR_CLAMP_EXT

// Matrix modes (equivalent to OpenGL)
#define RL_MODELVIEW 0x1700  // GL_MODELVIEW
#define RL_PROJECTION 0x1701 // GL_PROJECTION
#define RL_TEXTURE 0x1702    // GL_TEXTURE

// GL buffer usage hint
#define RL_STREAM_DRAW 0x88E0  // GL_STREAM_DRAW
#define RL_STREAM_READ 0x88E1  // GL_STREAM_READ
#define RL_STREAM_COPY 0x88E2  // GL_STREAM_COPY
#define RL_STATIC_DRAW 0x88E4  // GL_STATIC_DRAW
#define RL_STATIC_READ 0x88E5  // GL_STATIC_READ
#define RL_STATIC_COPY 0x88E6  // GL_STATIC_COPY
#define RL_DYNAMIC_DRAW 0x88E8 // GL_DYNAMIC_DRAW
#define RL_DYNAMIC_READ 0x88E9 // GL_DYNAMIC_READ
#define RL_DYNAMIC_COPY 0x88EA // GL_DYNAMIC_COPY

// GL Shader type
#define RL_FRAGMENT_SHADER 0x8B30 // GL_FRAGMENT_SHADER
#define RL_VERTEX_SHADER 0x8B31   // GL_VERTEX_SHADER
#define RL_COMPUTE_SHADER 0x91B9  // GL_COMPUTE_SHADER

// GL blending factors
#define RL_ZERO 0                          // GL_ZERO
#define RL_ONE 1                           // GL_ONE
#define RL_SRC_COLOR 0x0300                // GL_SRC_COLOR
#define RL_ONE_MINUS_SRC_COLOR 0x0301      // GL_ONE_MINUS_SRC_COLOR
#define RL_SRC_ALPHA 0x0302                // GL_SRC_ALPHA
#define RL_ONE_MINUS_SRC_ALPHA 0x0303      // GL_ONE_MINUS_SRC_ALPHA
#define RL_DST_ALPHA 0x0304                // GL_DST_ALPHA
#define RL_ONE_MINUS_DST_ALPHA 0x0305      // GL_ONE_MINUS_DST_ALPHA
#define RL_DST_COLOR 0x0306                // GL_DST_COLOR
#define RL_ONE_MINUS_DST_COLOR 0x0307      // GL_ONE_MINUS_DST_COLOR
#define RL_SRC_ALPHA_SATURATE 0x0308       // GL_SRC_ALPHA_SATURATE
#define RL_CONSTANT_COLOR 0x8001           // GL_CONSTANT_COLOR
#define RL_ONE_MINUS_CONSTANT_COLOR 0x8002 // GL_ONE_MINUS_CONSTANT_COLOR
#define RL_CONSTANT_ALPHA 0x8003           // GL_CONSTANT_ALPHA
#define RL_ONE_MINUS_CONSTANT_ALPHA 0x8004 // GL_ONE_MINUS_CONSTANT_ALPHA

// GL blending functions/equations
#define RL_FUNC_ADD 0x8006              // GL_FUNC_ADD
#define RL_MIN 0x8007                   // GL_MIN
#define RL_MAX 0x8008                   // GL_MAX
#define RL_FUNC_SUBTRACT 0x800A         // GL_FUNC_SUBTRACT
#define RL_FUNC_REVERSE_SUBTRACT 0x800B // GL_FUNC_REVERSE_SUBTRACT
#define RL_BLEND_EQUATION 0x8009        // GL_BLEND_EQUATION
#define RL_BLEND_EQUATION_RGB 0x8009    // GL_BLEND_EQUATION_RGB   // (Same as BLEND_EQUATION)
#define RL_BLEND_EQUATION_ALPHA 0x883D  // GL_BLEND_EQUATION_ALPHA
#define RL_BLEND_DST_RGB 0x80C8         // GL_BLEND_DST_RGB
#define RL_BLEND_SRC_RGB 0x80C9         // GL_BLEND_SRC_RGB
#define RL_BLEND_DST_ALPHA 0x80CA       // GL_BLEND_DST_ALPHA
#define RL_BLEND_SRC_ALPHA 0x80CB       // GL_BLEND_SRC_ALPHA
#define RL_BLEND_COLOR 0x8005           // GL_BLEND_COLOR

#define RL_SHADER_LOC_MAP_DIFFUSE RL_SHADER_LOC_MAP_ALBEDO
#define RL_SHADER_LOC_MAP_SPECULAR RL_SHADER_LOC_MAP_METALNESS

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
#if (defined(__STDC__) && __STDC_VERSION__ >= 199901L) || (defined(_MSC_VER) && _MSC_VER >= 1800)
#include <stdbool.h>
#elif !defined(__cplusplus) && !defined(bool) && !defined(RL_BOOL_TYPE)
// Boolean type
typedef enum bool {
    false = 0,
    true = !false
} bool;
#endif

// Dynamic vertex buffers (position + texcoords + colors + indices arrays)
using rlVertexBuffer = struct rlVertexBuffer {
    int elementCount; // Number of elements in the buffer (QUADS)

    float* vertices;       // Vertex position (XYZ - 3 components per vertex) (shader-location = 0)
    float* texcoords;      // Vertex texture coordinates (UV - 2 components per vertex) (shader-location = 1)
    unsigned char* colors; // Vertex colors (RGBA - 4 components per vertex) (shader-location = 3)
#if defined(GRAPHICS_API_OPENGL_11) || defined(GRAPHICS_API_OPENGL_33)
    unsigned int* indices; // Vertex indices (in case vertex data comes indexed) (6 indices per quad)
#endif
#if defined(GRAPHICS_API_OPENGL_ES2)
    unsigned short* indices; // Vertex indices (in case vertex data comes indexed) (6 indices per quad)
#endif
    unsigned int vaoId;                // OpenGL Vertex Array Object id
    std::array<unsigned int, 4> vboId; // OpenGL Vertex Buffer Objects id (4 types of vertex data)
};

// Draw call type
// NOTE: Only texture changes register a new draw, other state-change-related elements are not
// used at this moment (vaoId, shader_id, matrices), raylib just forces a batch draw call if any
// of those state-change happens (this is done in core module)
struct rlDrawCall {
    int mode;            // Drawing mode: LINES, TRIANGLES, QUADS
    int vertexCount;     // Number of vertex of the draw
    int vertexAlignment; // Number of vertex required for index alignment (LINES, TRIANGLES)
    // unsigned int vaoId;       // Vertex array id to be used on the draw -> Using RLGL.currentBatch->vertexBuffer.vaoId
    // unsigned int shader_id;    // Shader id to be used on the draw -> Using RLGL.currentShader_id
    unsigned int texture_id; // Texture id to be used on the draw -> Use to create new draw call if changes

    // Matrix projection;        // Projection matrix for this draw -> Using RLGL.projection by default
    // Matrix modelview;         // Modelview matrix for this draw -> Using RLGL.modelview by default
};

// rlRenderBatch type
struct rlRenderBatch {
    int bufferCount;              // Number of vertex buffers (multi-buffering support)
    int currentBuffer;            // Current buffer tracking in case of multi-buffering
    rlVertexBuffer* vertexBuffer; // Dynamic buffer(s) for vertex data

    rlDrawCall* draws;  // Draw calls array_val, depends on texture_id
    int drawCounter;    // Draw calls counter
    float currentDepth; // Current depth value for next draw
};

// OpenGL version
enum rlGlVersion {
    RL_OPENGL_11 = 1, // OpenGL 1.1
    RL_OPENGL_21,     // OpenGL 2.1 (GLSL 120)
    RL_OPENGL_33,     // OpenGL 3.3 (GLSL 330)
    RL_OPENGL_43,     // OpenGL 4.3 (using GLSL 330)
    RL_OPENGL_ES_20   // OpenGL ES 2.0 (GLSL 100)
};

// Texture pixel formats
// NOTE: Support depends on OpenGL version
enum rlPixelFormat {
    RL_PIXELFORMAT_UNCOMPRESSED_GRAYSCALE = 1, // 8 bit per pixel (no alpha)
    RL_PIXELFORMAT_UNCOMPRESSED_GRAY_ALPHA,    // 8*2 bpp (2 channels)
    RL_PIXELFORMAT_UNCOMPRESSED_R5G6B5,        // 16 bpp
    RL_PIXELFORMAT_UNCOMPRESSED_R8G8B8,        // 24 bpp
    RL_PIXELFORMAT_UNCOMPRESSED_R5G5B5A1,      // 16 bpp (1 bit alpha)
    RL_PIXELFORMAT_UNCOMPRESSED_R4G4B4A4,      // 16 bpp (4 bit alpha)
    RL_PIXELFORMAT_UNCOMPRESSED_R8G8B8A8,      // 32 bpp
    RL_PIXELFORMAT_UNCOMPRESSED_R32,           // 32 bpp (1 channel - float)
    RL_PIXELFORMAT_UNCOMPRESSED_R32G32B32,     // 32*3 bpp (3 channels - float)
    RL_PIXELFORMAT_UNCOMPRESSED_R32G32B32A32,  // 32*4 bpp (4 channels - float)
    RL_PIXELFORMAT_COMPRESSED_DXT1_RGB,        // 4 bpp (no alpha)
    RL_PIXELFORMAT_COMPRESSED_DXT1_RGBA,       // 4 bpp (1 bit alpha)
    RL_PIXELFORMAT_COMPRESSED_DXT3_RGBA,       // 8 bpp
    RL_PIXELFORMAT_COMPRESSED_DXT5_RGBA,       // 8 bpp
    RL_PIXELFORMAT_COMPRESSED_ETC1_RGB,        // 4 bpp
    RL_PIXELFORMAT_COMPRESSED_ETC2_RGB,        // 4 bpp
    RL_PIXELFORMAT_COMPRESSED_ETC2_EAC_RGBA,   // 8 bpp
    RL_PIXELFORMAT_COMPRESSED_PVRT_RGB,        // 4 bpp
    RL_PIXELFORMAT_COMPRESSED_PVRT_RGBA,       // 4 bpp
    RL_PIXELFORMAT_COMPRESSED_ASTC_4x4_RGBA,   // 8 bpp
    RL_PIXELFORMAT_COMPRESSED_ASTC_8x8_RGBA    // 2 bpp
};

// Texture parameters: filter mode
// NOTE 1: Filtering considers mipmaps if available in the texture
// NOTE 2: Filter is accordingly set for minification and magnification
enum rlTextureFilter {
    RL_TEXTURE_FILTER_POINT = 0,       // No filter, just pixel approximation
    RL_TEXTURE_FILTER_BILINEAR,        // Linear filtering
    RL_TEXTURE_FILTER_TRILINEAR,       // Trilinear filtering (linear with mipmaps)
    RL_TEXTURE_FILTER_ANISOTROPIC_4X,  // Anisotropic filtering 4x
    RL_TEXTURE_FILTER_ANISOTROPIC_8X,  // Anisotropic filtering 8x
    RL_TEXTURE_FILTER_ANISOTROPIC_16X, // Anisotropic filtering 16x
};

// Color blending modes (pre-defined)
enum rlBlendMode {
    RL_BLEND_ALPHA = 0,         // Blend textures considering alpha (default)
    RL_BLEND_ADDITIVE,          // Blend textures adding colors
    RL_BLEND_MULTIPLIED,        // Blend textures multiplying colors
    RL_BLEND_ADD_COLORS,        // Blend textures adding colors (alternative)
    RL_BLEND_SUBTRACT_COLORS,   // Blend textures subtracting colors (alternative)
    RL_BLEND_ALPHA_PREMULTIPLY, // Blend premultiplied textures considering alpha
    RL_BLEND_CUSTOM,            // Blend textures using custom src/dst factors (use rlSetBlendFactors())
    RL_BLEND_CUSTOM_SEPARATE    // Blend textures using custom src/dst factors (use rlSetBlendFactorsSeparate())
};

// Shader location point type
enum rlShaderLocationIndex {
    RL_SHADER_LOC_VERTEX_POSITION = 0, // Shader location: vertex attribute: position
    RL_SHADER_LOC_VERTEX_TEXCOORD01,   // Shader location: vertex attribute: texcoord01
    RL_SHADER_LOC_VERTEX_TEXCOORD02,   // Shader location: vertex attribute: texcoord02
    RL_SHADER_LOC_VERTEX_NORMAL,       // Shader location: vertex attribute: normal
    RL_SHADER_LOC_VERTEX_TANGENT,      // Shader location: vertex attribute: tangent
    RL_SHADER_LOC_VERTEX_COLOR,        // Shader location: vertex attribute: color
    RL_SHADER_LOC_MATRIX_MVP,          // Shader location: matrix uniform: model-view-projection
    RL_SHADER_LOC_MATRIX_VIEW,         // Shader location: matrix uniform: view (camera transform)
    RL_SHADER_LOC_MATRIX_PROJECTION,   // Shader location: matrix uniform: projection
    RL_SHADER_LOC_MATRIX_MODEL,        // Shader location: matrix uniform: model (transform)
    RL_SHADER_LOC_MATRIX_NORMAL,       // Shader location: matrix uniform: normal
    RL_SHADER_LOC_VECTOR_VIEW,         // Shader location: vector uniform: view
    RL_SHADER_LOC_COLOR_DIFFUSE,       // Shader location: vector uniform: diffuse color
    RL_SHADER_LOC_COLOR_SPECULAR,      // Shader location: vector uniform: specular color
    RL_SHADER_LOC_COLOR_AMBIENT,       // Shader location: vector uniform: ambient color
    RL_SHADER_LOC_MAP_ALBEDO,          // Shader location: sampler2d texture: albedo (same as: RL_SHADER_LOC_MAP_DIFFUSE)
    RL_SHADER_LOC_MAP_METALNESS,       // Shader location: sampler2d texture: metalness (same as: RL_SHADER_LOC_MAP_SPECULAR)
    RL_SHADER_LOC_MAP_NORMAL,          // Shader location: sampler2d texture: normal
    RL_SHADER_LOC_MAP_ROUGHNESS,       // Shader location: sampler2d texture: roughness
    RL_SHADER_LOC_MAP_OCCLUSION,       // Shader location: sampler2d texture: occlusion
    RL_SHADER_LOC_MAP_EMISSION,        // Shader location: sampler2d texture: emission
    RL_SHADER_LOC_MAP_HEIGHT,          // Shader location: sampler2d texture: height
    RL_SHADER_LOC_MAP_CUBEMAP,         // Shader location: samplerCube texture: cubemap
    RL_SHADER_LOC_MAP_IRRADIANCE,      // Shader location: samplerCube texture: irradiance
    RL_SHADER_LOC_MAP_PREFILTER,       // Shader location: samplerCube texture: prefilter
    RL_SHADER_LOC_MAP_BRDF             // Shader location: sampler2d texture: brdf
};

// Shader uniform data type
enum rlShaderUniformDataType {
    RL_SHADER_UNIFORM_FLOAT = 0, // Shader uniform type: float
    RL_SHADER_UNIFORM_VEC2,      // Shader uniform type: vec2 (2 float)
    RL_SHADER_UNIFORM_VEC3,      // Shader uniform type: vec3 (3 float)
    RL_SHADER_UNIFORM_VEC4,      // Shader uniform type: vec4 (4 float)
    RL_SHADER_UNIFORM_INT,       // Shader uniform type: int
    RL_SHADER_UNIFORM_IVEC2,     // Shader uniform type: ivec2 (2 int)
    RL_SHADER_UNIFORM_IVEC3,     // Shader uniform type: ivec3 (3 int)
    RL_SHADER_UNIFORM_IVEC4,     // Shader uniform type: ivec4 (4 int)
    RL_SHADER_UNIFORM_SAMPLER2D  // Shader uniform type: sampler2d
};

// Shader attribute data types
enum rlShaderAttributeDataType {
    RL_SHADER_ATTRIB_FLOAT = 0, // Shader attribute type: float
    RL_SHADER_ATTRIB_VEC2,      // Shader attribute type: vec2 (2 float)
    RL_SHADER_ATTRIB_VEC3,      // Shader attribute type: vec3 (3 float)
    RL_SHADER_ATTRIB_VEC4       // Shader attribute type: vec4 (4 float)
};

// Framebuffer attachment type
// NOTE: By default up to 8 color channels defined, but it can be more
enum rlFramebufferAttachType {
    RL_ATTACHMENT_COLOR_CHANNEL0 = 0, // Framebuffer attachment type: color 0
    RL_ATTACHMENT_COLOR_CHANNEL1,     // Framebuffer attachment type: color 1
    RL_ATTACHMENT_COLOR_CHANNEL2,     // Framebuffer attachment type: color 2
    RL_ATTACHMENT_COLOR_CHANNEL3,     // Framebuffer attachment type: color 3
    RL_ATTACHMENT_COLOR_CHANNEL4,     // Framebuffer attachment type: color 4
    RL_ATTACHMENT_COLOR_CHANNEL5,     // Framebuffer attachment type: color 5
    RL_ATTACHMENT_COLOR_CHANNEL6,     // Framebuffer attachment type: color 6
    RL_ATTACHMENT_COLOR_CHANNEL7,     // Framebuffer attachment type: color 7
    RL_ATTACHMENT_DEPTH = 100,        // Framebuffer attachment type: depth
    RL_ATTACHMENT_STENCIL = 200,      // Framebuffer attachment type: stencil
};

// Framebuffer texture attachment type
enum rlFramebufferAttachTextureType {
    RL_ATTACHMENT_CUBEMAP_POSITIVE_X = 0, // Framebuffer texture attachment type: cubemap, +X side
    RL_ATTACHMENT_CUBEMAP_NEGATIVE_X,     // Framebuffer texture attachment type: cubemap, -X side
    RL_ATTACHMENT_CUBEMAP_POSITIVE_Y,     // Framebuffer texture attachment type: cubemap, +Y side
    RL_ATTACHMENT_CUBEMAP_NEGATIVE_Y,     // Framebuffer texture attachment type: cubemap, -Y side
    RL_ATTACHMENT_CUBEMAP_POSITIVE_Z,     // Framebuffer texture attachment type: cubemap, +Z side
    RL_ATTACHMENT_CUBEMAP_NEGATIVE_Z,     // Framebuffer texture attachment type: cubemap, -Z side
    RL_ATTACHMENT_TEXTURE2D = 100,        // Framebuffer texture attachment type: texture2d
    RL_ATTACHMENT_RENDERBUFFER = 200,     // Framebuffer texture attachment type: renderbuffer
};

// Face culling mode
enum rlCullMode {
    RL_CULL_FACE_FRONT = 0,
    RL_CULL_FACE_BACK
};

//------------------------------------------------------------------------------------
// Functions Declaration - Matrix operations
//------------------------------------------------------------------------------------

void
rlMatrixMode(int mode); // Choose the current matrix to be transformed
void
rlPushMatrix(); // Push the current matrix to stack
void
rlPopMatrix(); // Pop latest inserted matrix from stack
void
rlLoadIdentity(); // Reset current matrix to identity matrix
void
rlTranslatef(float x_val, float y_val, float z_val); // Multiply the current matrix by a translation matrix
void
rlRotatef(float angle, float x_val, float y_val, float z_val); // Multiply the current matrix by a rotation matrix
void
rlScalef(float x_val, float y_val, float z_val); // Multiply the current matrix by a scaling matrix
void
rlMultMatrixf(const float* matf); // Multiply the current matrix by another matrix
void
rlFrustum(double left, double right, double bottom, double top, double znear, double zfar);
void
rlOrtho(double left, double right, double bottom, double top, double znear, double zfar);
void
rlViewport(int x_val, int y_val, int width, int height); // Set the viewport area

//------------------------------------------------------------------------------------
// Functions Declaration - Vertex level operations
//------------------------------------------------------------------------------------
void
rlBegin(int mode); // Initialize drawing mode (how to organize vertex)
void
rlEnd(); // Finish vertex providing
void
rlVertex2i(int x_val, int y_val); // Define one vertex (position) - 2 int
void
rlVertex2f(float x_val, float y_val); // Define one vertex (position) - 2 float
void
rlVertex3f(float x_val, float y_val, float z_val); // Define one vertex (position) - 3 float
void
rlTexCoord2f(float x_val, float y_val); // Define one vertex (texture coordinate) - 2 float
void
rlNormal3f(float x_val, float y_val, float z_val); // Define one vertex (normal) - 3 float
void
rlColor4ub(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha); // Define one vertex (color) - 4
                                                                                             // byte
void
rlColor3f(float x_val, float y_val, float z_val); // Define one vertex (color) - 3 float
void
rlColor4f(float x_val, float y_val, float z_val, float w_val); // Define one vertex (color) - 4 float

//------------------------------------------------------------------------------------
// Functions Declaration - OpenGL style functions (common to 1.1, 3.3+, ES2)
// NOTE: This functions are used to completely abstract raylib code from OpenGL layer,
// some of them are direct wrappers over OpenGL calls, some others are custom
//------------------------------------------------------------------------------------

// Vertex buffers state
bool
rlEnableVertexArray(unsigned int vaoId); // Enable vertex array (VAO, if supported)
void
rlDisableVertexArray(); // Disable vertex array (VAO, if supported)
void
rlEnableVertexBuffer(unsigned int buffer_id); // Enable vertex buffer (VBO)
void
rlDisableVertexBuffer(); // Disable vertex buffer (VBO)
void
rlEnableVertexBufferElement(unsigned int buffer_id); // Enable vertex buffer element (VBO element)
void
rlDisableVertexBufferElement(); // Disable vertex buffer element (VBO element)
void
rlEnableVertexAttribute(unsigned int index); // Enable vertex attribute index
void
rlDisableVertexAttribute(unsigned int index); // Disable vertex attribute index
#if defined(GRAPHICS_API_OPENGL_11)
void
rlEnableStatePointer(int vertexAttribType, void* buffer); // Enable attribute state pointer
void
rlDisableStatePointer(int vertexAttribType); // Disable attribute state pointer
#endif

// Textures state
void
rlActiveTextureSlot(int slot); // Select and active a texture slot
void
rlEnableTexture(unsigned int texture_id); // Enable texture
void
rlDisableTexture(); // Disable texture
void
rlEnableTextureCubemap(unsigned int texture_id); // Enable texture cubemap
void
rlDisableTextureCubemap(); // Disable texture cubemap
void
rlTextureParameters(unsigned int texture_id, int param, int value); // Set texture parameters (filter, wrap)
void
rlCubemapParameters(unsigned int texture_id, int param, int value); // Set cubemap parameters (filter, wrap)

// Shader state
void
rlEnableShader(unsigned int shader_id); // Enable shader program
void
rlDisableShader(); // Disable shader program

// Framebuffer state
void
rlEnableFramebuffer(unsigned int buffer_id); // Enable render texture (fbo)
void
rlDisableFramebuffer(); // Disable render texture (fbo), return to default framebuffer
void
rlActiveDrawBuffers(int count); // Activate multiple draw color buffers

// General render state
void
rlEnableColorBlend(); // Enable color blending
void
rlDisableColorBlend(); // Disable color blending
void
rlEnableDepthTest(); // Enable depth test
void
rlDisableDepthTest(); // Disable depth test
void
rlEnableDepthMask(); // Enable depth write
void
rlDisableDepthMask(); // Disable depth write
void
rlEnableBackfaceCulling(); // Enable backface culling
void
rlDisableBackfaceCulling(); // Disable backface culling
void
rlSetCullFace(int mode); // Set face culling mode
void
rlEnableScissorTest(); // Enable scissor test
void
rlDisableScissorTest(); // Disable scissor test
void
rlScissor(int x_val, int y_val, int width, int height); // Scissor test
void
rlEnableWireMode(); // Enable wire mode
void
rlDisableWireMode(); // Disable wire mode
void
rlSetLineWidth(float width); // Set the line drawing width
float
rlGetLineWidth(); // Get the line drawing width
void
rlEnableSmoothLines(); // Enable line aliasing
void
rlDisableSmoothLines(); // Disable line aliasing
void
rlEnableStereoRender(); // Enable stereo rendering
void
rlDisableStereoRender(); // Disable stereo rendering
bool
rlIsStereoRenderEnabled(); // Check if stereo render is enabled

void
rlClearColor(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha); // Clear color buffer with color
void
rlClearScreenBuffers(); // Clear used screen buffers (color and depth)
void
rlCheckErrors(); // Check and log OpenGL error codes
void
rlSetBlendMode(int mode); // Set blending mode
void
rlSetBlendFactors(
    int glSrcFactor,
    int glDstFactor,
    int glEquation
); // Set blending mode factor and equation (using OpenGL factors)
void
rlSetBlendFactorsSeparate(
    int glSrcRGB,
    int glDstRGB,
    int glSrcAlpha,
    int glDstAlpha,
    int glEqRGB,
    int glEqAlpha
); // Set blending mode factors and equations separately (using OpenGL factors)

//------------------------------------------------------------------------------------
// Functions Declaration - rlgl functionality
//------------------------------------------------------------------------------------
// rlgl initialization functions
void
rlglInit(int width, int height); // Initialize rlgl (buffers, shaders, textures, states)
void
rlglClose(); // De-initialize rlgl (buffers, shaders, textures)
void
rlLoadExtensions(void* loader); // Load OpenGL extensions (loader function required)
int
rlGetVersion(); // Get current OpenGL version
void
rlSetFramebufferWidth(int width); // Set current framebuffer width
int
rlGetFramebufferWidth(); // Get default framebuffer width
void
rlSetFramebufferHeight(int height); // Set current framebuffer height
int
rlGetFramebufferHeight(); // Get default framebuffer height

unsigned int
rlGetTexture_idDefault(); // Get default texture id
unsigned int
rlGetShader_idDefault(); // Get default shader id
int*
rlGetShaderLocsDefault(); // Get default shader locations

// Render batch management
// NOTE: rlgl provides a default render batch to behave like OpenGL 1.1 immediate mode
// but this render batch API is exposed in case of custom batches are required
rlRenderBatch
rlLoadRenderBatch(int numBuffers, int bufferElements); // Load a render batch system
void
rlUnloadRenderBatch(rlRenderBatch batch); // Unload render batch system
void
rlDrawRenderBatch(rlRenderBatch* batch); // Draw render batch data (Update->Draw->Reset)
void
rlSetRenderBatchActive(rlRenderBatch* batch); // Set the active render batch for rlgl (NULL for default internal)
void
rlDrawRenderBatchActive(); // Update and draw internal render batch
bool
rlCheckRenderBatchLimit(int vCount); // Check internal buffer overflow for a given number of vertex

void
rlSetTexture(unsigned int texture_id); // Set current texture for render batch and check buffers limits

//------------------------------------------------------------------------------------------------------------------------

// Vertex buffers management
unsigned int
rlLoadVertexArray(); // Load vertex array (vao) if supported
unsigned int
rlLoadVertexBuffer(const void* buffer, int size, bool dynamic); // Load a vertex buffer attribute
unsigned int
rlLoadVertexBufferElement(const void* buffer, int size, bool dynamic); // Load a new attributes element buffer
void
rlUpdateVertexBuffer(unsigned int bufferId, const void* data, int dataSize, int offset); // Update GPU buffer with new data
void
rlUpdateVertexBufferElements(unsigned int buffer_id, const void* data, int dataSize, int offset); // Update vertex buffer
                                                                                                  // elements with new data
void
rlUnloadVertexArray(unsigned int vaoId);
void
rlUnloadVertexBuffer(unsigned int vboId);
void
rlSetVertexAttribute(unsigned int index_val, int compSize, int type, bool normalized, int stride, const void* pointer);
void
rlSetVertexAttributeDivisor(unsigned int index_val, int divisor);
void
rlSetVertexAttributeDefault(int locIndex_val, const void* value, int attribType, int count); // Set vertex attribute default
                                                                                             // value
void
rlDrawVertexArray(int offset, int count);
void
rlDrawVertexArrayElements(int offset, int count, const void* buffer);
void
rlDrawVertexArrayInstanced(int offset, int count, int instances);
void
rlDrawVertexArrayElementsInstanced(int offset, int count, const void* buffer, int instances);

// Textures management
unsigned int
rlLoadTexture(const void* data, int width, int height, int format, int mipmapCount); // Load texture in GPU
unsigned int
rlLoadTextureDepth(int width, int height, bool useRenderBuffer); // Load depth texture/renderbuffer (to be attached to fbo)
unsigned int
rlLoadTextureCubemap(const void* data, int size, int format); // Load texture cubemap
void
rlUpdateTexture(
    unsigned int texture_id,
    int offsetX_VAL,
    int offsetY_VAL,
    int width,
    int height,
    int format,
    const void* data
); // Update GPU texture with new data
void
rlGetGlTextureFormats(
    int format,
    unsigned int* glInternalFormat,
    unsigned int* glFormat,
    unsigned int* glType
); // Get OpenGL internal formats
const char*
rlGetPixelFormatName(unsigned int format); // Get name string for pixel format
void
rlUnloadTexture(unsigned int texture_id); // Unload texture from GPU memory
void
rlGenTextureMipmaps(unsigned int texture_id, int width, int height, int format, int* mipmaps); // Generate mipmap data for
                                                                                               // selected texture
void*
rlReadTexturePixels(unsigned int texture_id, int width, int height, int format); // Read texture pixel data
unsigned char*
rlReadScreenPixels(int width, int height); // Read screen pixel data (color buffer)

// Framebuffer management (fbo)
unsigned int
rlLoadFramebuffer(int width, int height); // Load an empty framebuffer
void
rlFramebufferAttach(
    unsigned int fboId,
    unsigned int texId,
    int attachType,
    int texType,
    int mipLevel
); // Attach texture/renderbuffer to a framebuffer
bool
rlFramebufferComplete(unsigned int buffer_id); // Verify framebuffer is complete
void
rlUnloadFramebuffer(unsigned int buffer_id); // Delete framebuffer from GPU

// Shaders management
unsigned int
rlLoadShaderCode(const char* vertex_src, const char* fragment_src); // Load shader from code strings
unsigned int
rlCompileShader(
    const char* shader_src,
    int type
); // Compile custom shader and return shader id (type: RL_VERTEX_SHADER, RL_FRAGMENT_SHADER, RL_COMPUTE_SHADER)
unsigned int
rlLoadShaderProgram(unsigned int vertex_shader_id, unsigned int fragment_shader_id); // Load custom shader program
void
rlUnloadShaderProgram(unsigned int shader_id); // Unload shader program
int
rlGetLocationUniform(unsigned int shader_id, const char* uniformName); // Get shader location uniform
int
rlGetLocationAttrib(unsigned int shader_id, const char* attribName); // Get shader location attribute
void
rlSetUniform(int locIndex_val, const void* value, int uniformType, int count); // Set shader value uniform
void
rlSetUniformMatrix(int locIndex_val, Matrix mat); // Set shader value matrix
void
rlSetUniformSampler(int locIndex_val, unsigned int texture_id); // Set shader value sampler
void
rlSetShader(unsigned int shader_id, int* locations); // Set shader currently active (id and locations)

// Compute shader management
unsigned int
rlLoadComputeShaderProgram(unsigned int shader_id); // Load compute shader program
void
rlComputeShaderDispatch(
    unsigned int groupX_VAL,
    unsigned int groupY_VAL,
    unsigned int groupZ
); // Dispatch compute shader (equivalent to *draw* for graphics pipeline)

// Shader buffer storage object management (ssbo)
unsigned int
rlLoadShaderBuffer(unsigned int size, const void* data, int usageHint); // Load shader storage buffer object (SSBO)
void
rlUnloadShaderBuffer(unsigned int ssboId); // Unload shader storage buffer object (SSBO)
void
rlUpdateShaderBuffer(unsigned int buffer_id, const void* data, unsigned int dataSize, unsigned int offset); // Update SSBO
                                                                                                            // buffer data
void
rlBindShaderBuffer(unsigned int buffer_id, unsigned int index); // Bind SSBO buffer
void
rlReadShaderBuffer(unsigned int buffer_id, void* dest, unsigned int count, unsigned int offset); // Read SSBO buffer data
                                                                                                 // (GPU->CPU)
void
rlCopyShaderBuffer(
    unsigned int destId,
    unsigned int srcId,
    unsigned int destOffset,
    unsigned int srcOffset,
    unsigned int count
); // Copy SSBO data between buffers
unsigned int
rlGetShaderBufferSize(unsigned int buffer_id); // Get SSBO buffer size

// Buffer management
void
rlBindImageTexture(unsigned int buffer_id, unsigned int index_val, int format, bool readonly_val); // Bind image texture

// Matrix state management
Matrix
rlGetMatrixModelview(); // Get internal modelview matrix
Matrix
rlGetMatrixProjection(); // Get internal projection matrix
Matrix
rlGetMatrixTransform(); // Get internal accumulated transform matrix
Matrix
rlGetMatrixProjectionStereo(int eye); // Get internal projection matrix for stereo render (selected eye)
Matrix
rlGetMatrixViewOffsetStereo(int eye); // Get internal view offset matrix for stereo render (selected eye)
void
rlSetMatrixProjection(Matrix proj); // Set a custom projection matrix (replaces internal projection matrix)
void
rlSetMatrixModelview(Matrix view); // Set a custom modelview matrix (replaces internal modelview matrix)
void
rlSetMatrixProjectionStereo(Matrix right, Matrix left); // Set eyes projection matrices for stereo rendering
void
rlSetMatrixViewOffsetStereo(Matrix right, Matrix left); // Set eyes view offsets matrices for stereo rendering

// Quick and dirty cube/quad buffers load->draw->unload
void
rlLoadDrawCube(); // Load and draw a cube
void
rlLoadDrawQuad(); // Load and draw a quad

#endif // RLGL_H

/***********************************************************************************
 *
 *   RLGL IMPLEMENTATION
 *
 ************************************************************************************/

#if defined(RLGL_IMPLEMENTATION)

#endif // RLGL_IMPLEMENTATION
