// Check if config flags have been externally provided on compilation line
#include "rlutils.hpp"

#include "rlmath.hpp"
#include "rlshapes.hpp"
#include "rltext.hpp"
#include "rltextures.hpp"
#include "rltypes.hpp"

#if defined(PLATFORM_ANDROID)
#include <android/asset_manager.h> // Required for: Android assets manager: AAsset, AAssetManager_open(), ...
#include <android/log.h>           // Required for: Android log system: __android_log_vprint()
#include <errno.h>                 // Required for: Android error types
#endif

#include <cstdio>  // Required for: FILE, fopen(), fseek(), ftell(), fread(), fwrite(), fprintf(), vprintf(), fclose()
#include <cstdlib> // Required for: exit()
#include <cstring> // Required for: strcpy(), strcat()

#if defined(_WIN32)
#include <direct.h>    // Required for: _getch(), _chdir()
#define GETCWD _getcwd // NOTE: MSDN recommends not to use getcwd(), chdir()
#define CHDIR _chdir
#include <io.h> // Required for: _access() [Used in FileExists()]
#else
#include <unistd.h> // Required for: getch(), chdir() (POSIX), access()
#define GETCWD getcwd
#define CHDIR chdir
#endif

#if defined(_WIN32) && (defined(_MSC_VER) || defined(__TINYC__))
#define DIRENT_MALLOC RL_MALLOC
#define DIRENT_FREE RL_FREE

#include "libs/dirent.h" // Required for: DIR, opendir(), closedir() [Used in LoadDirectoryFiles()]
#else
#include <dirent.h> // Required for: DIR, opendir(), closedir() [Used in LoadDirectoryFiles()]
#endif

#ifndef MAX_FILEPATH_CAPACITY
#define MAX_FILEPATH_CAPACITY 8192 // Maximum capacity for filepath
#endif
#ifndef MAX_FILEPATH_LENGTH
#define MAX_FILEPATH_LENGTH 4096 // Maximum length for filepaths (Linux PATH_MAX default value)
#endif

#ifndef MAX_DECOMPRESSION_SIZE
#define MAX_DECOMPRESSION_SIZE 64 // Maximum size allocated for decompression in MB
#endif

// Flags operation macros
#define FLAG_SET(n, f) ((n) |= (f))
#define FLAG_CLEAR(n, f) ((n) &= ~(f))
#define FLAG_TOGGLE(n, f) ((n) ^= (f))
#define FLAG_CHECK(n, f) ((n) & (f))

#define _CRT_INTERNAL_NONSTDC_NAMES 1
#include <sys/stat.h> // Required for: stat(), S_ISREG [Used in GetFileModTime(), IsFilePath()]

typedef struct {
    int x;
    int y;
} Point;
typedef struct {
    unsigned int width;
    unsigned int height;
} Size;

// Core global state context data
typedef struct CoreData {
    struct {
#if defined(PLATFORM_RPI)
        EGL_DISPMANX_WINDOW_T handle; // Native window handle (graphic device)
#endif
#if defined(PLATFORM_ANDROID) || defined(PLATFORM_RPI) || defined(PLATFORM_DRM)
#if defined(PLATFORM_DRM)
        int fd;                         // File descriptor for /dev/dri/...
        drmModeConnector* connector;    // Direct Rendering Manager (DRM) mode connector
        drmModeCrtc* crtc;              // CRT Controller
        int modeIndex;                  // Index of the used mode of connector->modes
        struct gbm_device* gbmDevice;   // GBM device
        struct gbm_surface* gbmSurface; // GBM surface
        struct gbm_bo* prevBO;          // Previous GBM buffer object (during frame swapping)
        uint32_t prevFB;                // Previous GBM framebufer (during frame swapping)
#endif                                  // PLATFORM_DRM
        EGLDisplay device;              // Native display device (physical screen connection)
        EGLSurface surface;             // Surface to draw on, framebuffers (connected to context)
        EGLContext context;             // Graphic context, mode in which drawing can be done
        EGLConfig config;               // Graphic config
#endif
        const char* title;     // Window text title const pointer
        unsigned int flags;    // Configuration flags (bit based), keeps window state
        bool ready;            // Check if window has been initialized successfully
        bool fullscreen;       // Check if fullscreen mode is enabled
        bool shouldClose;      // Check if window set for closing
        bool resizedLastFrame; // Check if window has been resized last frame
        bool eventWaiting;     // Wait for events before ending frame

        Point position;     // Window position on screen (required on fullscreen toggle)
        Size display;       // Display width and height (monitor, device-screen, LCD, ...)
        Size screen;        // Screen width and height (used render area)
        Size currentFbo;    // Current render width and height (depends on active fbo)
        Size render;        // Framebuffer width and height (render area, including black bars if required)
        Point renderOffset; // Offset from render area (must be divided by 2)
        Matrix screenScale; // Matrix to scale screen (framebuffer rendering)

        char** dropFilepaths;       // Store dropped files paths pointers (provided by GLFW)
        unsigned int dropFileCount; // Count dropped files strings

    } Window;
#if defined(PLATFORM_ANDROID)
    struct {
        bool appEnabled;                    // Flag to detect if app is active ** = true
        struct android_app* app;            // Android activity
        struct android_poll_source* source; // Android events polling source
        bool contextRebindRequired;         // Used to know context rebind required
    } Android;
#endif
    struct {
        const char* basePath; // Base path for data storage
    } Storage;
    struct {
        double current;  // Current time measure
        double previous; // Previous time measure
        double update;   // Time measure for frame update
        double draw;     // Time measure for frame draw
        double frame;    // Time measure for one frame
        double target;   // Desired time for one frame, if 0 not applied
#if defined(PLATFORM_ANDROID) || defined(PLATFORM_RPI) || defined(PLATFORM_DRM)
        unsigned long long base; // Base time measure for hi-res timer
#endif
        unsigned int frameCounter; // Frame counter
    } Time;
} CoreData;

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
const char* raylib_version = RAYLIB_VERSION; // raylib version exported symbol, required for some bindings

static CoreData CORE = { 0 }; // Global CORE state context

#if defined(SUPPORT_SCREEN_CAPTURE)
static int screenshotCounter = 0; // Screenshots counter
#endif

#if defined(SUPPORT_GIF_RECORDING)
static int gifFrameCounter = 0;      // GIF frames counter
static bool gifRecording = false;    // GIF recording state
static MsfGifState gifState = { 0 }; // MSGIF context state
#endif

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
static LoadFileDataCallback loadFileData = NULL; // LoadFileData callback function pointer
static SaveFileDataCallback saveFileData = NULL; // SaveFileText callback function pointer
static LoadFileTextCallback loadFileText = NULL; // LoadFileText callback function pointer
static SaveFileTextCallback saveFileText = NULL; // SaveFileText callback function pointer

//----------------------------------------------------------------------------------
// Functions to set internal callbacks
//----------------------------------------------------------------------------------
void
SetLoadFileDataCallback(LoadFileDataCallback callback)
{
    loadFileData = callback;
} // Set custom file data loader
void
SetSaveFileDataCallback(SaveFileDataCallback callback)
{
    saveFileData = callback;
} // Set custom file data saver
void
SetLoadFileTextCallback(LoadFileTextCallback callback)
{
    loadFileText = callback;
} // Set custom file text loader
void
SetSaveFileTextCallback(SaveFileTextCallback callback)
{
    saveFileText = callback;
} // Set custom file text saver

#if defined(PLATFORM_ANDROID)
static AAssetManager* assetManager = NULL;  // Android assets manager pointer
static const char* internalDataPath = NULL; // Android internal data path
#endif

//----------------------------------------------------------------------------------
// Module specific Functions Declaration
//----------------------------------------------------------------------------------
#if defined(PLATFORM_ANDROID)
FILE*
funopen(
    const void* cookie,
    int (*readfn)(void*, char*, int),
    int (*writefn)(void*, const char*, int),
    fpos_t (*seekfn)(void*, fpos_t, int),
    int (*closefn)(void*)
);

static int
android_read(void* cookie, char* buf, int size);
static int
android_write(void* cookie, const char* buf, int size);
static fpos_t
android_seek(void* cookie, fpos_t offset, int whence);
static int
android_close(void* cookie);
#endif

static void
ScanDirectoryFiles(
    const char* basePath,
    FilePathList* list,
    const char* filter
); // Scan all files and directories in a base path
static void
ScanDirectoryFilesRecursively(
    const char* basePath,
    FilePathList* list,
    const char* filter
); // Scan all files and directories recursively from a base path

//----------------------------------------------------------------------------------
// Module Functions Definition - Utilities
//----------------------------------------------------------------------------------

// Internal memory allocator
// NOTE: Initializes to zero by default
void*
MemAlloc(unsigned int size)
{
    void* ptr = RL_CALLOC(size, 1);
    return ptr;
}

// Internal memory reallocator
void*
MemRealloc(void* ptr, unsigned int size)
{
    void* ret = RL_REALLOC(ptr, size);
    return ret;
}

// Internal memory free
void
MemFree(void* ptr)
{
    RL_FREE(ptr);
}

// Get a random value between min and max (both included)
// WARNING: Ranges higher than RAND_MAX will return invalid results
// More specifically, if (max - min) > INT_MAX there will be an overflow,
// and otherwise if (max - min) > RAND_MAX the random value will incorrectly never exceed a certain threshold
int
GetRandomValue(int min, int max)
{
    if (min > max) {
        int tmp = max;
        max = min;
        min = tmp;
    }

    if ((unsigned int)(max - min) > (unsigned int)RAND_MAX) {
        TRACELOG(LOG_WARNING, "Invalid GetRandomValue() arguments, range should not be higher than %i", RAND_MAX);
    }

    return (rand() % (abs(max - min) + 1) + min);
}

// Set the seed for the random number generator
void
SetRandomSeed(unsigned int seed)
{
    srand(seed);
}

// Check if the file exists
bool
FileExists(const char* fileName)
{
    bool result = false;

#if defined(_WIN32)
    if (_access(fileName, 0) != -1)
        result = true;
#else
    if (access(fileName, F_OK) != -1)
        result = true;
#endif

    // NOTE: Alternatively, stat() can be used instead of access()
    // #include <sys/stat.h>
    // struct stat statbuf;
    // if (stat(filename, &statbuf) == 0) result = true;

    return result;
}

// Check file extension
// NOTE: Extensions checking is not case-sensitive
bool
IsFileExtension(const char* fileName, const char* ext)
{
#define MAX_FILE_EXTENSION_SIZE 16

    bool result = false;
    const char* fileExt = GetFileExtension(fileName);

    if (fileExt != NULL) {
#if defined(SUPPORT_MODULE_RTEXT) && defined(SUPPORT_TEXT_MANIPULATION)
        int extCount = 0;
        const char** checkExts = TextSplit(ext, ';', &extCount); // WARNING: Module required: rtext

        char fileExtLower[MAX_FILE_EXTENSION_SIZE + 1] = { 0 };
        strncpy(fileExtLower, TextToLower(fileExt), MAX_FILE_EXTENSION_SIZE); // WARNING: Module required: rtext

        for (int i = 0; i < extCount; i++) {
            if (strcmp(fileExtLower, TextToLower(checkExts[i])) == 0) {
                result = true;
                break;
            }
        }
#else
        if (strcmp(fileExt, ext) == 0)
            result = true;
#endif
    }

    return result;
}

// Check if a directory path exists
bool
DirectoryExists(const char* dirPath)
{
    bool result = false;
    DIR* dir = opendir(dirPath);

    if (dir != NULL) {
        result = true;
        closedir(dir);
    }

    return result;
}

// Get file length in bytes
// NOTE: GetFileSize() conflicts with windows.h
int
GetFileLength(const char* fileName)
{
    int size = 0;

    FILE* file = fopen(fileName, "rb");

    if (file != NULL) {
        fseek(file, 0L, SEEK_END);
        size = (int)ftell(file);
        fclose(file);
    }

    return size;
}

// Get pointer to extension for a filename string (includes the dot: .png)
const char*
GetFileExtension(const char* fileName)
{
    const char* dot = strrchr(fileName, '.');

    if (!dot || dot == fileName)
        return NULL;

    return dot;
}

// String pointer reverse break: returns right-most occurrence of charset in s
static const char*
strprbrk(const char* s, const char* charset)
{
    const char* latestMatch = NULL;
    for (; s = strpbrk(s, charset), s != NULL; latestMatch = s++) {
    }
    return latestMatch;
}

// Get pointer to filename for a path string
const char*
GetFileName(const char* filePath)
{
    const char* fileName = NULL;
    if (filePath != NULL)
        fileName = strprbrk(filePath, "\\/");

    if (!fileName)
        return filePath;

    return fileName + 1;
}

// Get filename string without extension (uses static string)
const char*
GetFileNameWithoutExt(const char* filePath)
{
#define MAX_FILENAMEWITHOUTEXT_LENGTH 256

    static char fileName[MAX_FILENAMEWITHOUTEXT_LENGTH] = { 0 };
    memset(fileName, 0, MAX_FILENAMEWITHOUTEXT_LENGTH);

    if (filePath != NULL)
        strcpy(fileName, GetFileName(filePath)); // Get filename with extension

    int size = (int)strlen(fileName); // Get size in bytes

    for (int i = 0; (i < size) && (i < MAX_FILENAMEWITHOUTEXT_LENGTH); i++) {
        if (fileName[i] == '.') {
            // NOTE: We break on first '.' found
            fileName[i] = '\0';
            break;
        }
    }

    return fileName;
}

// Get directory for a given filePath
const char*
GetDirectoryPath(const char* filePath)
{
    /*
        // NOTE: Directory separator is different in Windows and other platforms,
        // fortunately, Windows also support the '/' separator, that's the one should be used
        #if defined(_WIN32)
            char separator = '\\';
        #else
            char separator = '/';
        #endif
    */
    const char* lastSlash = NULL;
    static char dirPath[MAX_FILEPATH_LENGTH] = { 0 };
    memset(dirPath, 0, MAX_FILEPATH_LENGTH);

    // In case provided path does not contain a root drive letter (C:\, D:\) nor leading path separator (\, /),
    // we add the current directory path to dirPath
    if (filePath[1] != ':' && filePath[0] != '\\' && filePath[0] != '/') {
        // For security, we set starting path to current directory,
        // obtained path will be concatenated to this
        dirPath[0] = '.';
        dirPath[1] = '/';
    }

    lastSlash = strprbrk(filePath, "\\/");
    if (lastSlash) {
        if (lastSlash == filePath) {
            // The last and only slash is the leading one: path is in a root directory
            dirPath[0] = filePath[0];
            dirPath[1] = '\0';
        }
        else {
            // NOTE: Be careful, strncpy() is not safe, it does not care about '\0'
            memcpy(
                dirPath + (filePath[1] != ':' && filePath[0] != '\\' && filePath[0] != '/' ? 2 : 0),
                filePath,
                strlen(filePath) - (strlen(lastSlash) - 1)
            );
            dirPath
                [strlen(filePath) - strlen(lastSlash) +
                 (filePath[1] != ':' && filePath[0] != '\\' && filePath[0] != '/' ? 2 : 0)] = '\0'; // Add '\0' manually
        }
    }

    return dirPath;
}

// Get previous directory path for a given path
const char*
GetPrevDirectoryPath(const char* dirPath)
{
    static char prevDirPath[MAX_FILEPATH_LENGTH] = { 0 };
    memset(prevDirPath, 0, MAX_FILEPATH_LENGTH);
    int pathLen = (int)strlen(dirPath);

    if (pathLen <= 3)
        strcpy(prevDirPath, dirPath);

    for (int i = (pathLen - 1); (i >= 0) && (pathLen > 3); i--) {
        if ((dirPath[i] == '\\') || (dirPath[i] == '/')) {
            // Check for root: "C:\" or "/"
            if (((i == 2) && (dirPath[1] == ':')) || (i == 0))
                i++;

            strncpy(prevDirPath, dirPath, i);
            break;
        }
    }

    return prevDirPath;
}

// Get current working directory
const char*
GetWorkingDirectory(void)
{
    static char currentDir[MAX_FILEPATH_LENGTH] = { 0 };
    memset(currentDir, 0, MAX_FILEPATH_LENGTH);

    char* path = GETCWD(currentDir, MAX_FILEPATH_LENGTH - 1);

    return path;
}

// Load directory filepaths
// NOTE: Base path is prepended to the scanned filepaths
// WARNING: Directory is scanned twice, first time to get files count
// No recursive scanning is done!
FilePathList
LoadDirectoryFiles(const char* dirPath)
{
    FilePathList files = { 0 };
    unsigned int fileCounter = 0;

    struct dirent* entity;
    DIR* dir = opendir(dirPath);

    if (dir != NULL) // It's a directory
    {
        // SCAN 1: Count files
        while ((entity = readdir(dir)) != NULL) {
            // NOTE: We skip '.' (current dir) and '..' (parent dir) filepaths
            if ((strcmp(entity->d_name, ".") != 0) && (strcmp(entity->d_name, "..") != 0))
                fileCounter++;
        }

        // Memory allocation for dirFileCount
        files.capacity = fileCounter;
        files.paths = (char**)RL_MALLOC(files.capacity * sizeof(char*));
        for (unsigned int i = 0; i < files.capacity; i++)
            files.paths[i] = (char*)RL_MALLOC(MAX_FILEPATH_LENGTH * sizeof(char));

        closedir(dir);

        // SCAN 2: Read filepaths
        // NOTE: Directory paths are also registered
        ScanDirectoryFiles(dirPath, &files, NULL);

        // Security check: read files.count should match fileCounter
        if (files.count != files.capacity)
            TRACELOG(LOG_WARNING, "FILEIO: Read files count do not match capacity allocated");
    }
    else
        TRACELOG(LOG_WARNING, "FILEIO: Failed to open requested directory"); // Maybe it's a file...

    return files;
}

// Load directory filepaths with extension filtering and recursive directory scan
// NOTE: On recursive loading we do not pre-scan for file count, we use MAX_FILEPATH_CAPACITY
FilePathList
LoadDirectoryFilesEx(const char* basePath, const char* filter, bool scanSubdirs)
{
    FilePathList files = { 0 };

    files.capacity = MAX_FILEPATH_CAPACITY;
    files.paths = (char**)RL_CALLOC(files.capacity, sizeof(char*));
    for (unsigned int i = 0; i < files.capacity; i++)
        files.paths[i] = (char*)RL_CALLOC(MAX_FILEPATH_LENGTH, sizeof(char));

    // WARNING: basePath is always prepended to scanned paths
    if (scanSubdirs)
        ScanDirectoryFilesRecursively(basePath, &files, filter);
    else
        ScanDirectoryFiles(basePath, &files, filter);

    return files;
}

// Unload directory filepaths
// WARNING: files.count is not reseted to 0 after unloading
void
UnloadDirectoryFiles(FilePathList files)
{
    for (unsigned int i = 0; i < files.capacity; i++)
        RL_FREE(files.paths[i]);

    RL_FREE(files.paths);
}

// Change working directory, returns true on success
bool
ChangeDirectory(const char* dir)
{
    bool result = CHDIR(dir);

    if (result != 0)
        TRACELOG(LOG_WARNING, "SYSTEM: Failed to change to directory: %s", dir);

    return (result == 0);
}

// Check if a given path point to a file
bool
IsPathFile(const char* path)
{
    struct stat pathStat = { 0 };
    stat(path, &pathStat);

    return S_ISREG(pathStat.st_mode);
}

// Check if a file has been dropped into window
bool
IsFileDropped(void)
{
    if (CORE.Window.dropFileCount > 0)
        return true;
    else
        return false;
}

// Load dropped filepaths
FilePathList
LoadDroppedFiles(void)
{
    FilePathList files = { 0 };

    files.count = CORE.Window.dropFileCount;
    files.paths = CORE.Window.dropFilepaths;

    return files;
}

// Unload dropped filepaths
void
UnloadDroppedFiles(FilePathList files)
{
    // WARNING: files pointers are the same as internal ones

    if (files.count > 0) {
        for (unsigned int i = 0; i < files.count; i++)
            RL_FREE(files.paths[i]);

        RL_FREE(files.paths);

        CORE.Window.dropFileCount = 0;
        CORE.Window.dropFilepaths = NULL;
    }
}

// Get file modification time (last write time)
long
GetFileModTime(const char* fileName)
{
    struct stat result = { 0 };

    if (stat(fileName, &result) == 0) {
        time_t mod = result.st_mtime;

        return (long)mod;
    }

    return 0;
}

// Load data from file into a buffer
unsigned char*
LoadFileData(const char* fileName, unsigned int* bytesRead)
{
    unsigned char* data = NULL;
    *bytesRead = 0;

    if (fileName != NULL) {
        if (loadFileData) {
            data = loadFileData(fileName, bytesRead);
            return data;
        }
#if defined(SUPPORT_STANDARD_FILEIO)
        FILE* file = fopen(fileName, "rb");

        if (file != NULL) {
            // WARNING: On binary streams SEEK_END could not be found,
            // using fseek() and ftell() could not work in some (rare) cases
            fseek(file, 0, SEEK_END);
            int size = ftell(file);
            fseek(file, 0, SEEK_SET);

            if (size > 0) {
                data = (unsigned char*)RL_MALLOC(size * sizeof(unsigned char));

                // NOTE: fread() returns number of read elements instead of bytes, so we read [1 byte, size elements]
                unsigned int count = (unsigned int)fread(data, sizeof(unsigned char), size, file);
                *bytesRead = count;

                if (count != size)
                    TRACELOG(LOG_WARNING, "FILEIO: [%s] File partially loaded", fileName);
                else
                    TRACELOG(LOG_INFO, "FILEIO: [%s] File loaded successfully", fileName);
            }
            else
                TRACELOG(LOG_WARNING, "FILEIO: [%s] Failed to read file", fileName);

            fclose(file);
        }
        else
            TRACELOG(LOG_WARNING, "FILEIO: [%s] Failed to open file", fileName);
#else
        TRACELOG(LOG_WARNING, "FILEIO: Standard file io not supported, use custom file callback");
#endif
    }
    else
        TRACELOG(LOG_WARNING, "FILEIO: File name provided is not valid");

    return data;
}

// Unload file data allocated by LoadFileData()
void
UnloadFileData(unsigned char* data)
{
    RL_FREE(data);
}

// Save data to file from buffer
bool
SaveFileData(const char* fileName, void* data, unsigned int bytesToWrite)
{
    bool success = false;

    if (fileName != NULL) {
        if (saveFileData) {
            return saveFileData(fileName, data, bytesToWrite);
        }
#if defined(SUPPORT_STANDARD_FILEIO)
        FILE* file = fopen(fileName, "wb");

        if (file != NULL) {
            unsigned int count = (unsigned int)fwrite(data, sizeof(unsigned char), bytesToWrite, file);

            if (count == 0)
                TRACELOG(LOG_WARNING, "FILEIO: [%s] Failed to write file", fileName);
            else if (count != bytesToWrite)
                TRACELOG(LOG_WARNING, "FILEIO: [%s] File partially written", fileName);
            else
                TRACELOG(LOG_INFO, "FILEIO: [%s] File saved successfully", fileName);

            int result = fclose(file);
            if (result == 0)
                success = true;
        }
        else
            TRACELOG(LOG_WARNING, "FILEIO: [%s] Failed to open file", fileName);
#else
        TRACELOG(LOG_WARNING, "FILEIO: Standard file io not supported, use custom file callback");
#endif
    }
    else
        TRACELOG(LOG_WARNING, "FILEIO: File name provided is not valid");

    return success;
}

// Export data to code (.h), returns true on success
bool
ExportDataAsCode(const unsigned char* data, unsigned int size, const char* fileName)
{
    bool success = false;

#ifndef TEXT_BYTES_PER_LINE
#define TEXT_BYTES_PER_LINE 20
#endif

    // NOTE: Text data buffer size is estimated considering raw data size in bytes
    // and requiring 6 char bytes for every byte: "0x00, "
    char* txtData = (char*)RL_CALLOC(size * 6 + 2000, sizeof(char));

    int byteCount = 0;
    byteCount += sprintf(
        txtData + byteCount, "////////////////////////////////////////////////////////////////////////////////////////\n"
    );
    byteCount += sprintf(
        txtData + byteCount, "//                                                                                    //\n"
    );
    byteCount += sprintf(
        txtData + byteCount, "// DataAsCode exporter v1.0 - Raw data exported as an array of bytes                  //\n"
    );
    byteCount += sprintf(
        txtData + byteCount, "//                                                                                    //\n"
    );
    byteCount += sprintf(
        txtData + byteCount, "// more info and bugs-report:  github.com/raysan5/raylib                              //\n"
    );
    byteCount += sprintf(
        txtData + byteCount, "// feedback and support:       ray[at]raylib.com                                      //\n"
    );
    byteCount += sprintf(
        txtData + byteCount, "//                                                                                    //\n"
    );
    byteCount += sprintf(
        txtData + byteCount, "// Copyright (c) 2022-2023 Ramon Santamaria (@raysan5)                                //\n"
    );
    byteCount += sprintf(
        txtData + byteCount, "//                                                                                    //\n"
    );
    byteCount += sprintf(
        txtData + byteCount, "////////////////////////////////////////////////////////////////////////////////////////\n\n"
    );

    // Get file name from path and convert variable name to uppercase
    char varFileName[256] = { 0 };
    strcpy(varFileName, GetFileNameWithoutExt(fileName));
    for (int i = 0; varFileName[i] != '\0'; i++)
        if ((varFileName[i] >= 'a') && (varFileName[i] <= 'z')) {
            varFileName[i] = varFileName[i] - 32;
        }

    byteCount += sprintf(txtData + byteCount, "static unsigned char %s_DATA[%i] = { ", varFileName, size);
    for (unsigned int i = 0; i < size - 1; i++)
        byteCount += sprintf(txtData + byteCount, ((i % TEXT_BYTES_PER_LINE == 0) ? "0x%x,\n" : "0x%x, "), data[i]);
    byteCount += sprintf(txtData + byteCount, "0x%x };\n", data[size - 1]);

    // NOTE: Text data size exported is determined by '\0' (NULL) character
    success = SaveFileText(fileName, txtData);

    RL_FREE(txtData);

    if (success != 0)
        TRACELOG(LOG_INFO, "FILEIO: [%s] Data as code exported successfully", fileName);
    else
        TRACELOG(LOG_WARNING, "FILEIO: [%s] Failed to export data as code", fileName);

    return success;
}

// Load text data from file, returns a '\0' terminated string
// NOTE: text chars array should be freed manually
char*
LoadFileText(const char* fileName)
{
    char* text = NULL;

    if (fileName != NULL) {
        if (loadFileText) {
            text = loadFileText(fileName);
            return text;
        }
#if defined(SUPPORT_STANDARD_FILEIO)
        FILE* file = fopen(fileName, "rt");

        if (file != NULL) {
            // WARNING: When reading a file as 'text' file,
            // text mode causes carriage return-linefeed translation...
            // ...but using fseek() should return correct byte-offset
            fseek(file, 0, SEEK_END);
            unsigned int size = (unsigned int)ftell(file);
            fseek(file, 0, SEEK_SET);

            if (size > 0) {
                text = (char*)RL_MALLOC((size + 1) * sizeof(char));
                unsigned int count = (unsigned int)fread(text, sizeof(char), size, file);

                // WARNING: \r\n is converted to \n on reading, so,
                // read bytes count gets reduced by the number of lines
                if (count < size)
                    text = RL_REALLOC(text, count + 1);

                // Zero-terminate the string
                text[count] = '\0';

                TRACELOG(LOG_INFO, "FILEIO: [%s] Text file loaded successfully", fileName);
            }
            else
                TRACELOG(LOG_WARNING, "FILEIO: [%s] Failed to read text file", fileName);

            fclose(file);
        }
        else
            TRACELOG(LOG_WARNING, "FILEIO: [%s] Failed to open text file", fileName);
#else
        TRACELOG(LOG_WARNING, "FILEIO: Standard file io not supported, use custom file callback");
#endif
    }
    else
        TRACELOG(LOG_WARNING, "FILEIO: File name provided is not valid");

    return text;
}

// Unload file text data allocated by LoadFileText()
void
UnloadFileText(char* text)
{
    RL_FREE(text);
}

// Save text data to file (write), string must be '\0' terminated
bool
SaveFileText(const char* fileName, char* text)
{
    bool success = false;

    if (fileName != NULL) {
        if (saveFileText) {
            return saveFileText(fileName, text);
        }
#if defined(SUPPORT_STANDARD_FILEIO)
        FILE* file = fopen(fileName, "wt");

        if (file != NULL) {
            int count = fprintf(file, "%s", text);

            if (count < 0)
                TRACELOG(LOG_WARNING, "FILEIO: [%s] Failed to write text file", fileName);
            else
                TRACELOG(LOG_INFO, "FILEIO: [%s] Text file saved successfully", fileName);

            int result = fclose(file);
            if (result == 0)
                success = true;
        }
        else
            TRACELOG(LOG_WARNING, "FILEIO: [%s] Failed to open text file", fileName);
#else
        TRACELOG(LOG_WARNING, "FILEIO: Standard file io not supported, use custom file callback");
#endif
    }
    else
        TRACELOG(LOG_WARNING, "FILEIO: File name provided is not valid");

    return success;
}

// Compress data (DEFLATE algorithm)
unsigned char*
CompressData(const unsigned char* data, int dataSize, int* compDataSize)
{
#define COMPRESSION_QUALITY_DEFLATE 8

    unsigned char* compData = NULL;

#if defined(SUPPORT_COMPRESSION_API)
    // Compress data and generate a valid DEFLATE stream
    struct sdefl sdefl = { 0 };
    int bounds = sdefl_bound(dataSize);
    compData = (unsigned char*)RL_CALLOC(bounds, 1);
    *compDataSize =
        sdeflate(&sdefl, compData, data, dataSize, COMPRESSION_QUALITY_DEFLATE); // Compression level 8, same as stbwi

    TRACELOG(LOG_INFO, "SYSTEM: Compress data: Original size: %i -> Comp. size: %i", dataSize, *compDataSize);
#endif

    return compData;
}

// Decompress data (DEFLATE algorithm)
unsigned char*
DecompressData(const unsigned char* compData, int compDataSize, int* dataSize)
{
    unsigned char* data = NULL;

#if defined(SUPPORT_COMPRESSION_API)
    // Decompress data from a valid DEFLATE stream
    data = (unsigned char*)RL_CALLOC(MAX_DECOMPRESSION_SIZE * 1024 * 1024, 1);
    int length = sinflate(data, MAX_DECOMPRESSION_SIZE * 1024 * 1024, compData, compDataSize);

    // WARNING: RL_REALLOC can make (and leave) data copies in memory, be careful with sensitive compressed data!
    // TODO: Use a different approach, create another buffer, copy data manually to it and wipe original buffer memory
    unsigned char* temp = (unsigned char*)RL_REALLOC(data, length);

    if (temp != NULL)
        data = temp;
    else
        TRACELOG(LOG_WARNING, "SYSTEM: Failed to re-allocate required decompression memory");

    *dataSize = length;

    TRACELOG(LOG_INFO, "SYSTEM: Decompress data: Comp. size: %i -> Original size: %i", compDataSize, *dataSize);
#endif

    return data;
}

// Encode data to Base64 string
char*
EncodeDataBase64(const unsigned char* data, int dataSize, int* outputSize)
{
    static const unsigned char base64encodeTable[] = { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
                                                       'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
                                                       'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
                                                       'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
                                                       '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/' };

    static const int modTable[] = { 0, 2, 1 };

    *outputSize = 4 * ((dataSize + 2) / 3);

    char* encodedData = (char*)RL_MALLOC(*outputSize);

    if (encodedData == NULL)
        return NULL;

    for (int i = 0, j = 0; i < dataSize;) {
        unsigned int octetA = (i < dataSize) ? (unsigned char)data[i++] : 0;
        unsigned int octetB = (i < dataSize) ? (unsigned char)data[i++] : 0;
        unsigned int octetC = (i < dataSize) ? (unsigned char)data[i++] : 0;

        unsigned int triple = (octetA << 0x10) + (octetB << 0x08) + octetC;

        encodedData[j++] = base64encodeTable[(triple >> 3 * 6) & 0x3F];
        encodedData[j++] = base64encodeTable[(triple >> 2 * 6) & 0x3F];
        encodedData[j++] = base64encodeTable[(triple >> 1 * 6) & 0x3F];
        encodedData[j++] = base64encodeTable[(triple >> 0 * 6) & 0x3F];
    }

    for (int i = 0; i < modTable[dataSize % 3]; i++)
        encodedData[*outputSize - 1 - i] = '='; // Padding character

    return encodedData;
}

// Decode Base64 string data
unsigned char*
DecodeDataBase64(const unsigned char* data, int* outputSize)
{
    static const unsigned char base64decodeTable[] = {
        0, 0, 0, 0, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,
        0, 0, 0, 0, 0,  0,  0,  0,  0,  0,  0,  0,  62, 0,  0,  0,  63, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 0,  0,  0, 0,
        0, 0, 0, 0, 1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 0, 0,
        0, 0, 0, 0, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51
    };

    // Get output size of Base64 input data
    int outSize = 0;
    for (int i = 0; data[4 * i] != 0; i++) {
        if (data[4 * i + 3] == '=') {
            if (data[4 * i + 2] == '=')
                outSize += 1;
            else
                outSize += 2;
        }
        else
            outSize += 3;
    }

    // Allocate memory to store decoded Base64 data
    unsigned char* decodedData = (unsigned char*)RL_MALLOC(outSize);

    for (int i = 0; i < outSize / 3; i++) {
        unsigned char a = base64decodeTable[(int)data[4 * i]];
        unsigned char b = base64decodeTable[(int)data[4 * i + 1]];
        unsigned char c = base64decodeTable[(int)data[4 * i + 2]];
        unsigned char d = base64decodeTable[(int)data[4 * i + 3]];

        decodedData[3 * i] = (a << 2) | (b >> 4);
        decodedData[3 * i + 1] = (b << 4) | (c >> 2);
        decodedData[3 * i + 2] = (c << 6) | d;
    }

    if (outSize % 3 == 1) {
        int n = outSize / 3;
        unsigned char a = base64decodeTable[(int)data[4 * n]];
        unsigned char b = base64decodeTable[(int)data[4 * n + 1]];
        decodedData[outSize - 1] = (a << 2) | (b >> 4);
    }
    else if (outSize % 3 == 2) {
        int n = outSize / 3;
        unsigned char a = base64decodeTable[(int)data[4 * n]];
        unsigned char b = base64decodeTable[(int)data[4 * n + 1]];
        unsigned char c = base64decodeTable[(int)data[4 * n + 2]];
        decodedData[outSize - 2] = (a << 2) | (b >> 4);
        decodedData[outSize - 1] = (b << 4) | (c >> 2);
    }

    *outputSize = outSize;
    return decodedData;
}

// Scan all files and directories in a base path
// WARNING: files.paths[] must be previously allocated and
// contain enough space to store all required paths
static void
ScanDirectoryFiles(const char* basePath, FilePathList* files, const char* filter)
{
    static char path[MAX_FILEPATH_LENGTH] = { 0 };
    memset(path, 0, MAX_FILEPATH_LENGTH);

    struct dirent* dp = NULL;
    DIR* dir = opendir(basePath);

    if (dir != NULL) {
        while ((dp = readdir(dir)) != NULL) {
            if ((strcmp(dp->d_name, ".") != 0) && (strcmp(dp->d_name, "..") != 0)) {
                sprintf(path, "%s/%s", basePath, dp->d_name);

                if (filter != NULL) {
                    if (IsFileExtension(path, filter)) {
                        strcpy(files->paths[files->count], path);
                        files->count++;
                    }
                }
                else {
                    strcpy(files->paths[files->count], path);
                    files->count++;
                }
            }
        }

        closedir(dir);
    }
    else
        TRACELOG(LOG_WARNING, "FILEIO: Directory cannot be opened (%s)", basePath);
}

// Scan all files and directories recursively from a base path
static void
ScanDirectoryFilesRecursively(const char* basePath, FilePathList* files, const char* filter)
{
    char path[MAX_FILEPATH_LENGTH] = { 0 };
    memset(path, 0, MAX_FILEPATH_LENGTH);

    struct dirent* dp = NULL;
    DIR* dir = opendir(basePath);

    if (dir != NULL) {
        while (((dp = readdir(dir)) != NULL) && (files->count < files->capacity)) {
            if ((strcmp(dp->d_name, ".") != 0) && (strcmp(dp->d_name, "..") != 0)) {
                // Construct new path from our base path
                sprintf(path, "%s/%s", basePath, dp->d_name);

                if (IsPathFile(path)) {
                    if (filter != NULL) {
                        if (IsFileExtension(path, filter)) {
                            strcpy(files->paths[files->count], path);
                            files->count++;
                        }
                    }
                    else {
                        strcpy(files->paths[files->count], path);
                        files->count++;
                    }

                    if (files->count >= files->capacity) {
                        TRACELOG(LOG_WARNING, "FILEIO: Maximum filepath scan capacity reached (%i files)", files->capacity);
                        break;
                    }
                }
                else
                    ScanDirectoryFilesRecursively(path, files, filter);
            }
        }

        closedir(dir);
    }
    else
        TRACELOG(LOG_WARNING, "FILEIO: Directory cannot be opened (%s)", basePath);
}

#if defined(PLATFORM_ANDROID)
// Initialize asset manager from android app
void
InitAssetManager(AAssetManager* manager, const char* dataPath)
{
    assetManager = manager;
    internalDataPath = dataPath;
}

// Replacement for fopen()
// Ref: https://developer.android.com/ndk/reference/group/asset
FILE*
android_fopen(const char* fileName, const char* mode)
{
    if (mode[0] == 'w') {
// fopen() is mapped to android_fopen() that only grants read access to
// assets directory through AAssetManager but we want to also be able to
// write data when required using the standard stdio FILE access functions
// Ref: https://stackoverflow.com/questions/11294487/android-writing-saving-files-from-native-code-only
#undef fopen
        return fopen(TextFormat("%s/%s", internalDataPath, fileName), mode);
#define fopen(name, mode) android_fopen(name, mode)
    }
    else {
        // NOTE: AAsset provides access to read-only asset
        AAsset* asset = AAssetManager_open(assetManager, fileName, AASSET_MODE_UNKNOWN);

        if (asset != NULL) {
            // Get pointer to file in the assets
            return funopen(asset, android_read, android_write, android_seek, android_close);
        }
        else {
#undef fopen
            // Just do a regular open if file is not found in the assets
            return fopen(TextFormat("%s/%s", internalDataPath, fileName), mode);
#define fopen(name, mode) android_fopen(name, mode)
        }
    }
}
#endif // PLATFORM_ANDROID

//----------------------------------------------------------------------------------
// Module specific Functions Definition
//----------------------------------------------------------------------------------
#if defined(PLATFORM_ANDROID)
static int
android_read(void* cookie, char* buf, int size)
{
    return AAsset_read((AAsset*)cookie, buf, size);
}

static int
android_write(void* cookie, const char* buf, int size)
{
    TRACELOG(LOG_WARNING, "ANDROID: Failed to provide write access to APK");

    return EACCES;
}

static fpos_t
android_seek(void* cookie, fpos_t offset, int whence)
{
    return AAsset_seek((AAsset*)cookie, offset, whence);
}

static int
android_close(void* cookie)
{
    AAsset_close((AAsset*)cookie);
    return 0;
}
#endif // PLATFORM_ANDROID
