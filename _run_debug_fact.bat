@echo off
call "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars64.bat" x64 >nul 2>nul
if %errorlevel% neq 0 (
    echo [FAIL] vcvars64.bat failed
    pause
    exit /b 1
)
echo [INFO] Running fact_static_convert-char.exe ...
echo [INFO] Capturing all output...
D:\agent\booming-il2cpp\verification\foundation-dll\System.Private.CoreLib\convert-char\native_test\fact-verify\build\fact_static_convert-char.exe 2>&1
echo [INFO] EXIT_CODE=%ERRORLEVEL%
pause