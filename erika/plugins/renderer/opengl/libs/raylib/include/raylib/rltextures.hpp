/**********************************************************************************************
 *
 *   rtextures - Basic functions to load and draw textures
 *
 *   CONFIGURATION:
 *
 *   #define SUPPORT_MODULE_RTEXTURES
 *       rtextures module is included in the build
 *
 *   #define SUPPORT_FILEFORMAT_BMP
 *   #define SUPPORT_FILEFORMAT_PNG
 *   #define SUPPORT_FILEFORMAT_TGA
 *   #define SUPPORT_FILEFORMAT_JPG
 *   #define SUPPORT_FILEFORMAT_GIF
 *   #define SUPPORT_FILEFORMAT_QOI
 *   #define SUPPORT_FILEFORMAT_PSD
 *   #define SUPPORT_FILEFORMAT_HDR
 *   #define SUPPORT_FILEFORMAT_PIC
 *   #define SUPPORT_FILEFORMAT_PNM
 *   #define SUPPORT_FILEFORMAT_DDS
 *   #define SUPPORT_FILEFORMAT_PKM
 *   #define SUPPORT_FILEFORMAT_KTX
 *   #define SUPPORT_FILEFORMAT_PVR
 *   #define SUPPORT_FILEFORMAT_ASTC
 *       Select desired fileformats to be supported for image data loading. Some of those formats are
 *       supported by default, to remove support, just comment unrequired #define in this module
 *
 *   #define SUPPORT_IMAGE_EXPORT
 *       Support image export in multiple file formats
 *
 *   #define SUPPORT_IMAGE_MANIPULATION
 *       Support multiple image editing functions to scale, adjust colors, flip, draw on images, crop...
 *       If not defined only some image editing functions supported: ImageFormat(), ImageAlphaMask(), ImageResize*()
 *
 *   #define SUPPORT_IMAGE_GENERATION
 *       Support procedural image generation functionality (gradient, spot, perlin-noise, cellular)
 *
 *   DEPENDENCIES:
 *       stb_image        - Multiple image formats loading (JPEG, PNG, BMP, TGA, PSD, GIF, PIC)
 *                          NOTE: stb_image has been slightly modified to support Android platform.
 *       stb_image_resize - Multiple image resize algorithms
 *
 *
 *   LICENSE: zlib/libpng
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

#ifndef RAYTEXTURES_H
#define RAYTEXTURES_H

// Check if config flags have been externally provided on compilation line
#if !defined(EXTERNAL_CONFIG_FLAGS)
#include "config.hpp" // Defines module configuration flags
#endif

#include "rltypes.hpp" // Declares module functions

#if defined(SUPPORT_MODULE_RTEXTURES)

// Support only desired texture formats on stb_image
#if !defined(SUPPORT_FILEFORMAT_BMP)
#define STBI_NO_BMP
#endif
#if !defined(SUPPORT_FILEFORMAT_PNG)
#define STBI_NO_PNG
#endif
#if !defined(SUPPORT_FILEFORMAT_TGA)
#define STBI_NO_TGA
#endif
#if !defined(SUPPORT_FILEFORMAT_JPG)
#define STBI_NO_JPEG // Image format .jpg and .jpeg
#endif
#if !defined(SUPPORT_FILEFORMAT_PSD)
#define STBI_NO_PSD
#endif
#if !defined(SUPPORT_FILEFORMAT_GIF)
#define STBI_NO_GIF
#endif
#if !defined(SUPPORT_FILEFORMAT_PIC)
#define STBI_NO_PIC
#endif
#if !defined(SUPPORT_FILEFORMAT_HDR)
#define STBI_NO_HDR
#endif
#if !defined(SUPPORT_FILEFORMAT_PNM)
#define STBI_NO_PNM
#endif

#if defined(SUPPORT_FILEFORMAT_DDS)
#define RL_GPUTEX_SUPPORT_DDS
#endif
#if defined(SUPPORT_FILEFORMAT_PKM)
#define RL_GPUTEX_SUPPORT_PKM
#endif
#if defined(SUPPORT_FILEFORMAT_KTX)
#define RL_GPUTEX_SUPPORT_KTX
#endif
#if defined(SUPPORT_FILEFORMAT_PVR)
#define RL_GPUTEX_SUPPORT_PVR
#endif
#if defined(SUPPORT_FILEFORMAT_ASTC)
#define RL_GPUTEX_SUPPORT_ASTC
#endif

// Image fileformats not supported by default
#if defined(__TINYC__)
#define STBI_NO_SIMD
#endif

//----------------------------------------------------------------------------------
// Defines and Macros
//----------------------------------------------------------------------------------
#ifndef PIXELFORMAT_UNCOMPRESSED_R5G5B5A1_ALPHA_THRESHOLD
#define PIXELFORMAT_UNCOMPRESSED_R5G5B5A1_ALPHA_THRESHOLD 50 // Threshold over 255 to set alpha as 0
#endif

#ifndef GAUSSIAN_BLUR_ITERATIONS
#define GAUSSIAN_BLUR_ITERATIONS 4 // Number of box blur iterations to approximate gaussian blur
#endif

//----------------------------------------------------------------------------------
// Module Functions Definition
//----------------------------------------------------------------------------------

// Image loading functions
// NOTE: These functions do not require GPU access
RLAPI Image
LoadImage(const char* fileName); // Load image from file into CPU memory (RAM)
RLAPI Image
LoadImageRaw(const char* fileName, int width, int height, int format, int headerSize); // Load image from RAW file data
RLAPI Image
LoadImageAnim(const char* fileName, int* frames); // Load image sequence from file (frames appended to image.data)
RLAPI Image
LoadImageFromMemory(
    const char* fileType,
    const unsigned char* fileData,
    int dataSize
); // Load image from memory buffer, fileType refers to extension: i.e. '.png'
RLAPI Image
LoadImageFromTexture(Texture2D texture); // Load image from GPU texture data
RLAPI Image
LoadImageFromScreen(void); // Load image from screen buffer and (screenshot)
RLAPI bool
IsImageReady(Image image); // Check if an image is ready
RLAPI void
UnloadImage(Image image); // Unload image from CPU memory (RAM)
RLAPI bool
ExportImage(Image image, const char* fileName); // Export image data to file, returns true on success
RLAPI bool
ExportImageAsCode(
    Image image,
    const char* fileName
); // Export image as code file defining an array of bytes, returns true on success

// Image generation functions
RLAPI Image
GenImageColor(int width, int height, Color color); // Generate image: plain color
RLAPI Image
GenImageGradientV(int width, int height, Color top, Color bottom); // Generate image: vertical gradient
RLAPI Image
GenImageGradientH(int width, int height, Color left, Color right); // Generate image: horizontal gradient
RLAPI Image
GenImageGradientRadial(int width, int height, float density, Color inner, Color outer); // Generate image: radial gradient
RLAPI Image
GenImageChecked(int width, int height, int checksX, int checksY, Color col1, Color col2); // Generate image: checked
RLAPI Image
GenImageWhiteNoise(int width, int height, float factor); // Generate image: white noise
RLAPI Image
GenImagePerlinNoise(int width, int height, int offsetX, int offsetY, float scale); // Generate image: perlin noise
RLAPI Image
GenImageCellular(int width, int height, int tileSize); // Generate image: cellular algorithm, bigger tileSize means bigger cells
RLAPI Image
GenImageText(int width, int height, const char* text); // Generate image: grayscale image from text data

// Image manipulation functions
RLAPI Image
ImageCopy(Image image); // Create an image duplicate (useful for transformations)
RLAPI Image
ImageFromImage(Image image, Rectangle rec); // Create an image from another image piece
RLAPI Image
ImageText(const char* text, int fontSize, Color color); // Create an image from text (default font)
RLAPI Image
ImageTextEx(Font font, const char* text, float fontSize, float spacing, Color tint); // Create an image from text (custom sprite
                                                                                     // font)
RLAPI void
ImageFormat(Image* image, int newFormat); // Convert image data to desired format
RLAPI void
ImageToPOT(Image* image, Color fill); // Convert image to POT (power-of-two)
RLAPI void
ImageCrop(Image* image, Rectangle crop); // Crop an image to a defined rectangle
RLAPI void
ImageAlphaCrop(Image* image, float threshold); // Crop image depending on alpha value
RLAPI void
ImageAlphaClear(Image* image, Color color, float threshold); // Clear alpha channel to desired color
RLAPI void
ImageAlphaMask(Image* image, Image alphaMask); // Apply alpha mask to image
RLAPI void
ImageAlphaPremultiply(Image* image); // Premultiply alpha channel
RLAPI void
ImageBlurGaussian(Image* image, int blurSize); // Apply Gaussian blur using a box blur approximation
RLAPI void
ImageResize(Image* image, int newWidth, int newHeight); // Resize image (Bicubic scaling algorithm)
RLAPI void
ImageResizeNN(Image* image, int newWidth, int newHeight); // Resize image (Nearest-Neighbor scaling algorithm)
RLAPI void
ImageResizeCanvas(Image* image, int newWidth, int newHeight, int offsetX, int offsetY, Color fill); // Resize canvas and fill
                                                                                                    // with color
RLAPI void
ImageMipmaps(Image* image); // Compute all mipmap levels for a provided image
RLAPI void
ImageDither(Image* image, int rBpp, int gBpp, int bBpp, int aBpp); // Dither image data to 16bpp or lower (Floyd-Steinberg
                                                                   // dithering)
RLAPI void
ImageFlipVertical(Image* image); // Flip image vertically
RLAPI void
ImageFlipHorizontal(Image* image); // Flip image horizontally
RLAPI void
ImageRotateCW(Image* image); // Rotate image clockwise 90deg
RLAPI void
ImageRotateCCW(Image* image); // Rotate image counter-clockwise 90deg
RLAPI void
ImageColorTint(Image* image, Color color); // Modify image color: tint
RLAPI void
ImageColorInvert(Image* image); // Modify image color: invert
RLAPI void
ImageColorGrayscale(Image* image); // Modify image color: grayscale
RLAPI void
ImageColorContrast(Image* image, float contrast); // Modify image color: contrast (-100 to 100)
RLAPI void
ImageColorBrightness(Image* image, int brightness); // Modify image color: brightness (-255 to 255)
RLAPI void
ImageColorReplace(Image* image, Color color, Color replace); // Modify image color: replace color
RLAPI Color*
LoadImageColors(Image image); // Load color data from image as a Color array (RGBA - 32bit)
RLAPI Color*
LoadImagePalette(
    Image image,
    int maxPaletteSize,
    int* colorCount
); // Load colors palette from image as a Color array (RGBA - 32bit)
RLAPI void
UnloadImageColors(Color* colors); // Unload color data loaded with LoadImageColors()
RLAPI void
UnloadImagePalette(Color* colors); // Unload colors palette loaded with LoadImagePalette()
RLAPI Rectangle
GetImageAlphaBorder(Image image, float threshold); // Get image alpha border rectangle
RLAPI Color
GetImageColor(Image image, int x, int y); // Get image pixel color at (x, y) position

// Image drawing functions
// NOTE: Image software-rendering functions (CPU)
RLAPI void
ImageClearBackground(Image* dst, Color color); // Clear image background with given color
RLAPI void
ImageDrawPixel(Image* dst, int posX, int posY, Color color); // Draw pixel within an image
RLAPI void
ImageDrawPixelV(Image* dst, Vector2 position, Color color); // Draw pixel within an image (Vector version)
RLAPI void
ImageDrawLine(Image* dst, int startPosX, int startPosY, int endPosX, int endPosY, Color color); // Draw line within an image
RLAPI void
ImageDrawLineV(Image* dst, Vector2 start, Vector2 end, Color color); // Draw line within an image (Vector version)
RLAPI void
ImageDrawCircle(Image* dst, int centerX, int centerY, int radius, Color color); // Draw a filled circle within an image
RLAPI void
ImageDrawCircleV(Image* dst, Vector2 center, int radius, Color color); // Draw a filled circle within an image (Vector version)
RLAPI void
ImageDrawCircleLines(Image* dst, int centerX, int centerY, int radius, Color color); // Draw circle outline within an image
RLAPI void
ImageDrawCircleLinesV(Image* dst, Vector2 center, int radius, Color color); // Draw circle outline within an image (Vector
                                                                            // version)
RLAPI void
ImageDrawRectangle(Image* dst, int posX, int posY, int width, int height, Color color); // Draw rectangle within an image
RLAPI void
ImageDrawRectangleV(Image* dst, Vector2 position, Vector2 size, Color color); // Draw rectangle within an image (Vector version)
RLAPI void
ImageDrawRectangleRec(Image* dst, Rectangle rec, Color color); // Draw rectangle within an image
RLAPI void
ImageDrawRectangleLines(Image* dst, Rectangle rec, int thick, Color color); // Draw rectangle lines within an image
RLAPI void
ImageDraw(Image* dst, Image src, Rectangle srcRec, Rectangle dstRec, Color tint); // Draw a source image within a destination
                                                                                  // image (tint applied to source)
RLAPI void
ImageDrawText(Image* dst, const char* text, int posX, int posY, int fontSize, Color color); // Draw text (using default font)
                                                                                            // within an image (destination)
RLAPI void
ImageDrawTextEx(
    Image* dst,
    Font font,
    const char* text,
    Vector2 position,
    float fontSize,
    float spacing,
    Color tint
); // Draw text (custom sprite font) within an image (destination)

// Texture loading functions
// NOTE: These functions require GPU access
RLAPI Texture2D
LoadTexture(const char* fileName); // Load texture from file into GPU memory (VRAM)
RLAPI Texture2D
LoadTextureFromImage(Image image); // Load texture from image data
RLAPI TextureCubemap
LoadTextureCubemap(Image image, int layout); // Load cubemap from image, multiple image cubemap layouts supported
RLAPI RenderTexture2D
LoadRenderTexture(int width, int height); // Load texture for rendering (framebuffer)
RLAPI bool
IsTextureReady(Texture2D texture); // Check if a texture is ready
RLAPI void
UnloadTexture(Texture2D texture); // Unload texture from GPU memory (VRAM)
RLAPI bool
IsRenderTextureReady(RenderTexture2D target); // Check if a render texture is ready
RLAPI void
UnloadRenderTexture(RenderTexture2D target); // Unload render texture from GPU memory (VRAM)
RLAPI void
UpdateTexture(Texture2D texture, const void* pixels); // Update GPU texture with new data
RLAPI void
UpdateTextureRec(Texture2D texture, Rectangle rec, const void* pixels); // Update GPU texture rectangle with new data

// Texture configuration functions
RLAPI void
GenTextureMipmaps(Texture2D* texture); // Generate GPU mipmaps for a texture
RLAPI void
SetTextureFilter(Texture2D texture, int filter); // Set texture scaling filter mode
RLAPI void
SetTextureWrap(Texture2D texture, int wrap); // Set texture wrapping mode

// Texture drawing functions
RLAPI void
DrawTexture(Texture2D texture, int posX, int posY, Color tint); // Draw a Texture2D
RLAPI void
DrawTextureV(Texture2D texture, Vector2 position, Color tint); // Draw a Texture2D with position defined as Vector2
RLAPI void
DrawTextureEx(Texture2D texture, Vector2 position, float rotation, float scale, Color tint); // Draw a Texture2D with extended
                                                                                             // parameters
RLAPI void
DrawTextureRec(Texture2D texture, Rectangle source, Vector2 position, Color tint); // Draw a part of a texture defined by a
                                                                                   // rectangle
RLAPI void
DrawTexturePro(
    Texture2D texture,
    Rectangle source,
    Rectangle dest,
    Vector2 origin,
    float rotation,
    Color tint
); // Draw a part of a texture defined by a rectangle with 'pro' parameters
RLAPI void
DrawTextureNPatch(
    Texture2D texture,
    NPatchInfo nPatchInfo,
    Rectangle dest,
    Vector2 origin,
    float rotation,
    Color tint
); // Draws a texture (or part of it) that stretches or shrinks nicely

// Color/pixel related functions
RLAPI Color
Fade(Color color, float alpha); // Get color with alpha applied, alpha goes from 0.0f to 1.0f
RLAPI int
ColorToInt(Color color); // Get hexadecimal value for a Color
RLAPI Vector4
ColorNormalize(Color color); // Get Color normalized as float [0..1]
RLAPI Color
ColorFromNormalized(Vector4 normalized); // Get Color from normalized values [0..1]
RLAPI Vector3
ColorToHSV(Color color); // Get HSV values for a Color, hue [0..360], saturation/value [0..1]
RLAPI Color
ColorFromHSV(float hue, float saturation, float value); // Get a Color from HSV values, hue [0..360], saturation/value [0..1]
RLAPI Color
ColorTint(Color color, Color tint); // Get color multiplied with another color
RLAPI Color
ColorBrightness(Color color, float factor); // Get color with brightness correction, brightness factor goes from -1.0f to 1.0f
RLAPI Color
ColorContrast(Color color, float contrast); // Get color with contrast correction, contrast values between -1.0f and 1.0f
RLAPI Color
ColorAlpha(Color color, float alpha); // Get color with alpha applied, alpha goes from 0.0f to 1.0f
RLAPI Color
ColorAlphaBlend(Color dst, Color src, Color tint); // Get src alpha-blended into dst color with tint
RLAPI Color
GetColor(unsigned int hexValue); // Get Color structure from hexadecimal value
RLAPI Color
GetPixelColor(void* srcPtr, int format); // Get Color from a source pixel pointer of certain format
RLAPI void
SetPixelColor(void* dstPtr, Color color, int format); // Set color formatted into destination pixel pointer
RLAPI int
GetPixelDataSize(int width, int height, int format); // Get pixel data size in bytes for certain format

#endif // SUPPORT_MODULE_RTEXTURES

#endif