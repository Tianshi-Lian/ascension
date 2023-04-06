/**********************************************************************************************
 *
 *   raylib configuration flags
 *
 *   This file defines all the configuration flags for the different raylib modules
 *
 *   LICENSE: zlib/libpng
 *
 *   Copyright (c) 2018-2023 Ahmad Fatoum & Ramon Santamaria (@raysan5)
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

#ifndef CONFIG_H
#define CONFIG_H

//------------------------------------------------------------------------------------
// Module selection - Some modules could be avoided
// Mandatory modules: rcore, rlgl, utils
//------------------------------------------------------------------------------------
#define SUPPORT_MODULE_RSHAPES 1
#define SUPPORT_MODULE_RTEXTURES 1
#define SUPPORT_MODULE_RTEXT 1 // WARNING: It requires SUPPORT_MODULE_RTEXTURES to load sprite font textures

//------------------------------------------------------------------------------------
// Module: rlgl - Configuration Flags
//------------------------------------------------------------------------------------
// Use OpenGL 4.3
#define GRAPHICS_API_OPENGL_43 1

//------------------------------------------------------------------------------------
// Module: rshapes - Configuration Flags
//------------------------------------------------------------------------------------
// Use QUADS instead of TRIANGLES for drawing when possible
// Some lines-based shapes could still use lines
#define SUPPORT_QUADS_DRAW_MODE 1

//------------------------------------------------------------------------------------
// Module: rtextures - Configuration Flags
//------------------------------------------------------------------------------------
// Select the desired fileformats to be supported for image data loading
#define SUPPORT_FILEFORMAT_PNG 1
// #define SUPPORT_FILEFORMAT_BMP          1
// #define SUPPORT_FILEFORMAT_TGA          1
// #define SUPPORT_FILEFORMAT_JPG          1
#define SUPPORT_FILEFORMAT_GIF 1
#define SUPPORT_FILEFORMAT_QOI 1
// #define SUPPORT_FILEFORMAT_PSD          1
#define SUPPORT_FILEFORMAT_DDS 1
#define SUPPORT_FILEFORMAT_HDR 1
// #define SUPPORT_FILEFORMAT_PIC          1
// #define SUPPORT_FILEFORMAT_PNM          1
// #define SUPPORT_FILEFORMAT_KTX          1
// #define SUPPORT_FILEFORMAT_ASTC         1
// #define SUPPORT_FILEFORMAT_PKM          1
// #define SUPPORT_FILEFORMAT_PVR          1

// Support image export functionality (.png, .bmp, .tga, .jpg, .qoi)
#define SUPPORT_IMAGE_EXPORT 1
// Support procedural image generation functionality (gradient, spot, perlin-noise, cellular)
#define SUPPORT_IMAGE_GENERATION 1
// Support multiple image editing functions to scale, adjust colors, flip, draw on images, crop...
// If not defined, still some functions are supported: ImageFormat(), ImageCrop(), ImageToPOT()
#define SUPPORT_IMAGE_MANIPULATION 1

//------------------------------------------------------------------------------------
// Module: rtext - Configuration Flags
//------------------------------------------------------------------------------------
// Default font is loaded on window initialization to be available for the user to render simple text
// NOTE: If enabled, uses external module functions to load default raylib font
#define SUPPORT_DEFAULT_FONT 1
// Selected desired font fileformats to be supported for loading
#define SUPPORT_FILEFORMAT_FNT 1
#define SUPPORT_FILEFORMAT_TTF 1

// Support text management functions
// If not defined, still some functions are supported: TextLength(), TextFormat()
#define SUPPORT_TEXT_MANIPULATION 1

// rtext: Configuration values
//------------------------------------------------------------------------------------
#define MAX_TEXT_BUFFER_LENGTH                                                                                                 \
    1024                        // Size of internal static buffers used on some functions:
                                // TextFormat(), TextSubtext(), TextToUpper(), TextToLower(), TextToPascal(), TextSplit()
#define MAX_TEXTSPLIT_COUNT 128 // Maximum number of substrings to split: TextSplit()

#endif // CONFIG_H
