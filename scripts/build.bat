@echo off

SET build_args=-j 16

if not exist "..\build\build.ninja" (
    cmake -S ..\ -B ..\build -G"Ninja" -DCMAKE_C_COMPILER="D:/Tools/mingw64/bin/clang.exe" -DCMAKE_CXX_COMPILER="D:/Tools/mingw64/bin/clang++.exe"
)

pushd ..\build
ninja %build_args%
popd

if not exist "..\build\ascension\bin\logs" (
    mkdir ..\build\ascension\bin\logs
)
