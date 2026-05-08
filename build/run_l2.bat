@echo off
set VCVARSALL="C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvarsall.bat"
if not exist %VCVARSALL% (
    echo vcvarsall not found
    exit /b 1
)
REM Use exactly what fact_verifier.py does
call %VCVARSALL% x64 -vcvars_ver=14.42 >nul 2>nul
if errorlevel 1 (
    echo vcvarsall failed
    exit /b 1
)

set BUILD_DIR=D:\agent\booming-il2cpp\verification\foundation-dll\System.Private.CoreLib\convert-char\native_test\fact-verify\build
"%BUILD_DIR%\fact_static_convert-char.exe"
echo EXIT CODE: %ERRORLEVEL%
