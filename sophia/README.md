# Sophia
A set of CMake utility files

## Usage
### Directly
Download this repository and extract the `cmake` folder to your project folder. You can then import all the supplied utilities and functions by directly importing the helpers file, which automatically includes all the files in this project, for example:
```cmake
include(cmake/helpers)
```

### Submodule
Add this repository as a submodule to your main project repository and then you can import it into your main `CMakeLists.txt` as a subdirectory, for example:  
Terminal:
```git 
git submodule add git@github.com:r-ggraham/sophia.git libs/sophia
git submodule update --init --recursive
```  
CMakeLists.txt:
```cmake
add_subdirectory(libs/sophia)
include(helpers)
```
This is how it's being used in my 2D game [ascension](https://github.com/r-ggraham/ascension).

### CMake FetchContent
If you're happy to just download the repository during the build configuration and don't particularly care about keeping a copy on disk you can use the new CMake `FetchContent` module to import the project when you configure CMake, for example:
```cmake
include(FetchContent)
FetchContent_Declare(sophia
    GIT_REPOSITORY git@github.com:r-ggraham/sophia.git
    GIT_TAG "origin/main"
)
FetchContent_MakeAvailable(sophia)
include(helpers)
```
This is how it's being used in my 2D game engine [erika](https://github.com/r-ggraham/erika).
