@echo off
call "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvarsall.bat" x64
if %errorlevel% neq 0 (
    echo vcvarsall failed
    exit /b 1
)
echo VS env loaded
cmake --build D:\agent\booming-il2cpp\testing\foundation-dll\System.Private.CoreLib\convert-char\native\build_jit
exit /b %errorlevel%
