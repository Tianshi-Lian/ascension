@echo off

@REM We're going to run with 80% of available threads by default. As we can't use FP division,
@REM we multiply by by then divide by 12 for the equivalent of nproc*0.8.
FOR /F "tokens=*" %%g IN ('nproc') do (SET systhreads=%%g)
SET /A nthreads= (%systhreads%*10)/12

SET build_args=-j %nthreads%

if not exist "..\build\build.ninja" (
    cmake -S ..\ -B ..\build -G"Ninja" -DCMAKE_C_COMPILER="D:/Tools/mingw64/bin/clang.exe" -DCMAKE_CXX_COMPILER="D:/Tools/mingw64/bin/clang++.exe"
)

pushd ..\build
ninja -%build_args%
popd

if not exist "..\build\ascension\bin\logs" (
    mkdir ..\build\ascension\bin\logs
)
