@echo off
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvarsall.bat" x64
if %errorlevel% neq 0 exit /b 1
echo VS env ready

set NATIVE_DIR=D:\agent\booming-il2cpp\testing\foundation-dll\System.Private.CoreLib\convert-char\native
set BUILD_DIR=%NATIVE_DIR%\build_jit

:: Delete any stale build directory
if exist %BUILD_DIR% rmdir /s /q %BUILD_DIR%

:: Configure fresh
cmake -S %NATIVE_DIR% -B %BUILD_DIR% -G Ninja -DCHAOS_IL2CPP_CONFIG_TIER=profile -DCHAOS_IL2CPP_JIT_MODE=ON -DCMAKE_BUILD_TYPE=Release || exit /b 1

:: Build
cmake --build %BUILD_DIR% || exit /b 1

echo BUILD SUCCESS
