/**********************************************************************************************
 *
 *   raylib.utils - Some common utility functions
 *
 *   CONFIGURATION:
 *
 *   #define SUPPORT_TRACELOG
 *       Show TraceLog() output messages
 *       NOTE: By default LOG_DEBUG traces not shown
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

#ifndef RAYUTILS_H
#define RAYUTILS_H

#if !defined(EXTERNAL_CONFIG_FLAGS)
#include "config.hpp" // Defines module configuration flags
#endif

// Files management functions
unsigned char*
LoadFileData(const char* fileName, unsigned int* bytesRead); // Load file data as byte array (read)
void
UnloadFileData(unsigned char* data); // Unload file data allocated by LoadFileData()
bool
SaveFileData(
    const char* fileName,
    void* data,
    unsigned int bytesToWrite
); // Save data to file from byte array (write), returns true on success
bool
ExportDataAsCode(
    const unsigned char* data,
    unsigned int size,
    const char* fileName
); // Export data to code (.h), returns true on success
char*
LoadFileText(const char* fileName); // Load text data from file (read), returns a '\0' terminated string
void
UnloadFileText(char* text); // Unload file text data allocated by LoadFileText()
bool
SaveFileText(
    const char* fileName,
    char* text
); // Save text data to file (write), string must be '\0' terminated, returns true on success
bool
FileExists(const char* fileName); // Check if file exists
bool
DirectoryExists(const char* dirPath); // Check if a directory path exists
bool
IsFileExtension(const char* fileName, const char* ext); // Check file extension (including point: .png, .wav)
int
GetFileLength(const char* fileName); // Get file length in bytes (NOTE: GetFileSize() conflicts with windows.h)
const char*
GetFileExtension(const char* fileName); // Get pointer to extension for a filename string (includes dot: '.png')
const char*
GetFileName(const char* filePath); // Get pointer to filename for a path string
const char*
GetFileNameWithoutExt(const char* filePath); // Get filename string without extension (uses static string)
const char*
GetDirectoryPath(const char* filePath); // Get full path for a given fileName with path (uses static string)
const char*
GetPrevDirectoryPath(const char* dirPath); // Get previous directory path for a given path (uses static string)
const char*
GetWorkingDirectory(void); // Get current working directory (uses static string)
// const char*
// GetApplicationDirectory(void); // Get the directory if the running application (uses static string)
bool
ChangeDirectory(const char* dir); // Change working directory, return true on success
bool
IsPathFile(const char* path); // Check if a given path is a file or a directory
long
GetFileModTime(const char* fileName); // Get file modification time (last write time)

int
GetRandomValue(int min, int max); // Get a random value between min and max (both included)
void
SetRandomSeed(unsigned int seed); // Set the seed for the random number generator

void*
MemAlloc(unsigned int size); // Internal memory allocator
void*
MemRealloc(void* ptr, unsigned int size); // Internal memory reallocator
void
MemFree(void* ptr); // Internal memory free

unsigned char*
CompressData(
    const unsigned char* data,
    int dataSize,
    int* compDataSize
); // Compress data (DEFLATE algorithm), memory must be MemFree()
unsigned char*
DecompressData(
    const unsigned char* compData,
    int compDataSize,
    int* dataSize
); // Decompress data (DEFLATE algorithm), memory must be MemFree()
char*
EncodeDataBase64(
    const unsigned char* data,
    int dataSize,
    int* outputSize
); // Encode data to Base64 string, memory must be MemFree()
unsigned char*
DecodeDataBase64(const unsigned char* data, int* outputSize); // Decode Base64 string data, memory must be MemFree()

// Callbacks to hook some internal functions
// WARNING: These callbacks are intended for advance users
typedef unsigned char* (*LoadFileDataCallback)(const char* fileName, unsigned int* bytesRead);     // FileIO: Load binary data
typedef bool (*SaveFileDataCallback)(const char* fileName, void* data, unsigned int bytesToWrite); // FileIO: Save binary data
typedef char* (*LoadFileTextCallback)(const char* fileName);                                       // FileIO: Load text data
typedef bool (*SaveFileTextCallback)(const char* fileName, char* text);                            // FileIO: Save text data

#endif
