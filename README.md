# Ascension
A game about ascending through the seven planes of heaven

## Application
- [**ascension**](ascension/): The main application for the game

## Dependencies
### Development scripts
- [**python3**](https://www.python.org/downloads/): All build scripts are run through python

### Application & libraries
- [**cmake**](https://cmake.org/): Build system of choice
- [**ninja**](https://ninja-build.org/): CMake generator of choice
- [**clang**](https://clang.llvm.org/) or [**gcc**](https://gcc.gnu.org/): Supported C++20 compiler
- [**WinLibs**](https://winlibs.com/#download-release) (Optional/Windows): I personally use this for Windows development, using the `GCC + LLVM + MinGW-w64 (UCRT)` version. It conveniently contains all of the needed tools for building ISO C++ on Windows. Just extract somewhere add the `bin` folder to your PATH.

## Build
All libraries and dependencies are either included in this repository or added as submodules in the respective libs directories.
Don't forget to initialise them with `git submodule --init --recursive`

The project currently builds on Windows & Linux(Ubuntu) with `g++` and `clang++` using ISO `c++20`.

It's recommended to set an alias for `./scripts/dev.bat` or `./scripts/dev.sh` so you can run commands from the main project directory such as `dev build`.
- PowerShell: `New-Alias -Name dev -Value .\scripts\dev.bat`
- bash: `echo alias dev='./scripts/dev.sh' >> ~/.bashrc`

**Commands:**
- Help: `dev help`
- Build: `dev build`
- Configure CMake: `dev config`
- Clean: `dev clean`
- Run: `dev run`

Commands can also be chained as you wish, for example `dev clean build -bt=Release run -bt=Release` will clean the build files, then build the release version and finally run the release executable successively.

### Steps
1. Install all the application dependencies as listed above in [Dependencies](#dependencies)
1. Install the python dependencies with `pip install -r .\requirements.txt`
1. Build the game and it's libraries with `dev build`
    - To select the compiler use `dev configure -cc=<c_compiler> -cxx=<c++_compiler> build`
    - To select the build configuration use `dev build -bt=<Debug,Release,RelWithDebInfo>`
1. Run the game with `dev run`
