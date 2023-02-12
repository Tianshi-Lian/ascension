@echo off

if not exist "..\build\dist\" (
    mkdir ..\build\dist
)

if not exist "..\build\build.ninja" (
    cmake -S ..\ -B ..\build -G "Ninja" -D CMAKE_CXX_COMPILER=clang++
)

pushd ..\build
ninja
popd
