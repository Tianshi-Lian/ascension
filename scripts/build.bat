@echo off

SET build_args=-j 16

if not exist "..\build\build.ninja" (
    cmake -S ..\ -B ..\build -G "Ninja" -D CMAKE_CXX_COMPILER=clang++
)

pushd ..\build
ninja %build_args%
popd

if not exist "..\build\ascension\bin\logs" (
    mkdir ..\build\ascension\bin\logs
)
