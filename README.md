# Ascension
A game about ascending through the seven planes of heaven

## Application
- [**ascension**](ascension/): The main application for the game
## Dependencies
- [**sophia**](sophia/): A set of CMake utility files
- [**yuki**](yuki/): A core library for development, graphics & games
- [**erika**](erika/): A 2D game engine written in C++17

## Build
All libraries and dependencies are either included in this repository or added as submodules in the respective libs directories.  
Don't forget to initialise them with `git submodule --init --recursive`

The project currently builds on Windows & Linux(Ubuntu) with `g++` and `clang++` using ISO `c++17`.

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
