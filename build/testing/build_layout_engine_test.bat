@echo off
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvarsall.bat" x64
if errorlevel 1 exit /b 1

set "BUILD_DIR=D:\agent\booming-il2cpp\build\testing"
set "SLN=%BUILD_DIR%\chaos_il2cpp_testing.sln"

REM Build layout_engine_test
msbuild "%SLN%" /t:layout_engine_test /p:Configuration=Debug /v:m
if errorlevel 1 exit /b 1
echo layout_engine_test built OK
