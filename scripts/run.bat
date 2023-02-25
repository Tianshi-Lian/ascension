@echo off

if not exist "..\build\ascension\bin\ascension.exe" (
    echo missing executable, attempting to build first...
    call build.bat
)

pushd ..\build\ascension\bin
.\ascension
popd
