@echo off
call "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars64.bat" x64 >nul 2>nul
if %errorlevel% neq 0 (
    echo [FAIL] vcvars64.bat failed
    pause
    exit /b 1
)
echo [INFO] Environment set up. Running fact_static_convert-char.exe >&2
D:\agent\booming-il2cpp\verification\foundation-dll\System.Private.CoreLib\convert-char\native_test\fact-verify\build\fact_static_convert-char.exe
set EXITCODE=%ERRORLEVEL%
echo [INFO] EXIT_CODE=%EXITCODE% >&2
echo RESULT:%EXITCODE%
pause