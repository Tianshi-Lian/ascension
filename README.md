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

The project currently builds with `clang++` using `c++17`

It's recommended to set an alias for `./scripts/dev.bat` so you can run commands from the main project directory such as `dev build`.  
PowerShell: `New-Alias -Name dev -Value .\scripts\dev.bat`

Commands
- Build: `dev build`  
- Clean: `dev clean`
- Run: `dev run`
