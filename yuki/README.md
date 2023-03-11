# Yuki
A core library for C++ development, graphics & games

## Dependencies
- [**sophia**](../sophia): A set of CMake utility files

## Build
All libraries and dependencies are either included in this repository or added as submodules in the [libs](libs) directory.  
Don't forget to initialise them with `git submodule --init --recursive`

The project currently builds with `clang++` using `c++17`

It's recommended to set an alias for `./scripts/dev.bat` so you can run commands from the main project directory such as `dev build`.  
PowerShell: `New-Alias -Name dev -Value .\scripts\dev.bat`

Commands
- Build: `dev build`  
- Clean: `dev clean`
- Run: `dev run`
