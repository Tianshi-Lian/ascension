# Ascension
A game about ascending through the six planes

## Dependencies
- [**yuki**](https://github.com/r-ggraham/yuki): A core library for development, graphics & games
- [**erika**](https://github.com/r-ggraham/erika): A 2D game engine

## Build
All libraries and dependencies are either included in this repository or added as submodules in the [libs](libs) directory.
Don't forget to initialise them with `git submodule --init --recursive`

It's recommended to set an alias for `./scripts/dev.bat` so you can run commands from the main project directory such as `dev build`.
**PowerShell**: `new-Alias -Name dev -Value .\scripts\dev.bat`
