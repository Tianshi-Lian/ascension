/**********************************************************************************************
 *
 *   rtext - Basic functions to load fonts and draw text
 *
 *   CONFIGURATION:
 *
 *   #define SUPPORT_MODULE_RTEXT
 *       rtext module is included in the build
 *
 *   #define SUPPORT_FILEFORMAT_FNT
 *   #define SUPPORT_FILEFORMAT_TTF
 *       Selected desired fileformats to be supported for loading. Some of those formats are
 *       supported by default, to remove support, just comment unrequired #define in this module
 *
 *   #define SUPPORT_DEFAULT_FONT
 *       Load default raylib font on initialization to be used by DrawText() and MeasureText().
 *       If no default font loaded, DrawTextEx() and MeasureTextEx() are required.
 *
 *   #define TEXTSPLIT_MAX_TEXT_BUFFER_LENGTH
 *       TextSplit() function static buffer max size
 *
 *   #define MAX_TEXTSPLIT_COUNT
 *       TextSplit() function static substrings pointers array (pointing to static buffer)
 *
 *
 *   DEPENDENCIES:
 *       stb_truetype  - Load TTF file and rasterize characters data
 *       stb_rect_pack - Rectangles packing algorithms, required for font atlas generation
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

// Check if config flags have been externally provided on compilation line
#if !defined(EXTERNAL_CONFIG_FLAGS)
#include "config.hpp" // Defines module configuration flags
#endif

#include "rltypes.hpp" // Declares module functions

#if defined(SUPPORT_MODULE_RTEXT)

#if defined(SUPPORT_DEFAULT_FONT)
RLAPI void
rlLoadFontDefault(void);
RLAPI void
rlUnloadFontDefault(void);
#endif

// Font loading/unloading functions
RLAPI Font
GetFontDefault(void); // Get the default Font
RLAPI Font
LoadFont(const char* fileName); // Load font from file into GPU memory (VRAM)
RLAPI Font
LoadFontEx(const char* fileName, int fontSize, int* fontChars, int glyphCount); // Load font from file with extended parameters,
                                                                                // use NULL for fontChars and 0 for glyphCount
                                                                                // to load the default character set
RLAPI Font
LoadFontFromImage(Image image, Color key, int firstChar); // Load font from Image (XNA style)
RLAPI Font
LoadFontFromMemory(
    const char* fileType,
    const unsigned char* fileData,
    int dataSize,
    int fontSize,
    int* fontChars,
    int glyphCount
); // Load font from memory buffer, fileType refers to extension: i.e. '.ttf'
RLAPI bool
IsFontReady(Font font); // Check if a font is ready
RLAPI GlyphInfo*
LoadFontData(
    const unsigned char* fileData,
    int dataSize,
    int fontSize,
    int* fontChars,
    int glyphCount,
    int type
); // Load font data for further use
RLAPI Image
GenImageFontAtlas(
    const GlyphInfo* chars,
    Rectangle** recs,
    int glyphCount,
    int fontSize,
    int padding,
    int packMethod
); // Generate image font atlas using chars info
RLAPI void
UnloadFontData(GlyphInfo* chars, int glyphCount); // Unload font chars info data (RAM)
RLAPI void
UnloadFont(Font font); // Unload font from GPU memory (VRAM)
RLAPI bool
ExportFontAsCode(Font font, const char* fileName); // Export font as code file, returns true on success

// Text drawing functions
RLAPI void
DrawFPS(int posX, int posY); // Draw current FPS
RLAPI void
DrawText(const char* text, int posX, int posY, int fontSize, Color color); // Draw text (using default font)
RLAPI void
DrawTextEx(
    Font font,
    const char* text,
    Vector2 position,
    float fontSize,
    float spacing,
    Color tint
); // Draw text using font and additional parameters
RLAPI void
DrawTextPro(
    Font font,
    const char* text,
    Vector2 position,
    Vector2 origin,
    float rotation,
    float fontSize,
    float spacing,
    Color tint
); // Draw text using Font and pro parameters (rotation)
RLAPI void
DrawTextCodepoint(Font font, int codepoint, Vector2 position, float fontSize, Color tint); // Draw one character (codepoint)
RLAPI void
DrawTextCodepoints(
    Font font,
    const int* codepoints,
    int count,
    Vector2 position,
    float fontSize,
    float spacing,
    Color tint
); // Draw multiple character (codepoint)

// Text font info functions
RLAPI int
MeasureText(const char* text, int fontSize); // Measure string width for default font
RLAPI Vector2
MeasureTextEx(Font font, const char* text, float fontSize, float spacing); // Measure string size for Font
RLAPI int
GetGlyphIndex(
    Font font,
    int codepoint
); // Get glyph index position in font for a codepoint (unicode character), fallback to '?' if not found
RLAPI GlyphInfo
GetGlyphInfo(
    Font font,
    int codepoint
); // Get glyph font info data for a codepoint (unicode character), fallback to '?' if not found
RLAPI Rectangle
GetGlyphAtlasRec(
    Font font,
    int codepoint
); // Get glyph rectangle in font atlas for a codepoint (unicode character), fallback to '?' if not found

// Text codepoints management functions (unicode characters)
RLAPI char*
LoadUTF8(const int* codepoints, int length); // Load UTF-8 text encoded from codepoints array
RLAPI void
UnloadUTF8(char* text); // Unload UTF-8 text encoded from codepoints array
RLAPI int*
LoadCodepoints(
    const char* text,
    int* count
); // Load all codepoints from a UTF-8 text string, codepoints count returned by parameter
RLAPI void
UnloadCodepoints(int* codepoints); // Unload codepoints data from memory
RLAPI int
GetCodepointCount(const char* text); // Get total number of codepoints in a UTF-8 encoded string
RLAPI int
GetCodepoint(
    const char* text,
    int* codepointSize
); // Get next codepoint in a UTF-8 encoded string, 0x3f('?') is returned on failure
RLAPI int
GetCodepointNext(
    const char* text,
    int* codepointSize
); // Get next codepoint in a UTF-8 encoded string, 0x3f('?') is returned on failure
RLAPI int
GetCodepointPrevious(
    const char* text,
    int* codepointSize
); // Get previous codepoint in a UTF-8 encoded string, 0x3f('?') is returned on failure
RLAPI const char*
CodepointToUTF8(
    int codepoint,
    int* utf8Size
); // Encode one codepoint into UTF-8 byte array (array length returned as parameter)

// Text strings management functions (no UTF-8 strings, only byte chars)
// NOTE: Some strings allocate memory internally for returned strings, just be careful!
RLAPI int
TextCopy(char* dst, const char* src); // Copy one string to another, returns bytes copied
RLAPI bool
TextIsEqual(const char* text1, const char* text2); // Check if two text string are equal
RLAPI unsigned int
TextLength(const char* text); // Get text length, checks for '\0' ending
RLAPI const char*
TextFormat(const char* text, ...); // Text formatting with variables (sprintf() style)
RLAPI const char*
TextSubtext(const char* text, int position, int length); // Get a piece of a text string
RLAPI char*
TextReplace(char* text, const char* replace, const char* by); // Replace text string (WARNING: memory must be freed!)
RLAPI char*
TextInsert(const char* text, const char* insert, int position); // Insert text in a position (WARNING: memory must be freed!)
RLAPI const char*
TextJoin(const char** textList, int count, const char* delimiter); // Join text strings with delimiter
RLAPI const char**
TextSplit(const char* text, char delimiter, int* count); // Split text into multiple strings
RLAPI void
TextAppend(char* text, const char* append, int* position); // Append text at specific position and move cursor!
RLAPI int
TextFindIndex(const char* text, const char* find); // Find first text occurrence within a string
RLAPI const char*
TextToUpper(const char* text); // Get upper case version of provided string
RLAPI const char*
TextToLower(const char* text); // Get lower case version of provided string
RLAPI const char*
TextToPascal(const char* text); // Get Pascal case notation version of provided string
RLAPI int
TextToInteger(const char* text); // Get integer value from text (negative values not supported)

#endif // SUPPORT_MODULE_RTEXT
