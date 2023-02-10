@echo off

if exist ".cache/" (
    rm -r .cache
)
if exist "build/" (
    rm -r build
)
