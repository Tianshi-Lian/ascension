# Sophia
A basic build system for C++, CMake & Ninja written in Python.

## Dependencies
- [**python3**](https://www.python.org/downloads/): All scripts are run through python

## Usage
Import this sub-project into your main projects route directory and invoke commands through `./sophia/dev.xxx`.

It's recommended to set an alias for `./sophia/dev.bat` or `./sophia/dev.sh` so you can run commands from the main project directory such as `dev build`.
- PowerShell: `New-Alias -Name dev -Value .\sophia\dev.bat`
- bash: `echo alias dev='./sophia/dev.sh' >> ~/.bashrc`

### Configuration
There are a small amount of configuration variables you can change to adjust the build. These can be set or changed inside the `.env` file in your root directory. Variables set in this file will always be overwritten by those passed as command line arguments, e.g. `dev build -bt=Debug`
```bash
# Set the project name.
# Must be the same as the main executable folder name.
PROJECT_NAME=ascension
# Set the build configuration.
# Options: Debug, Release, RelWithDebInfo
BUILD_TYPE=Release
# Set the C compiler.
# Supported: gcc, clang
C_COMPILER=clang
# Set the C++ compiler.
# Supported: g++, clang++
CXX_COMPILER=clang++
```

See my 2D game [**ascension**](https://github.com/r-ggraham/ascension) for how this library is used.
