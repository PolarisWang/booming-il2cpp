@echo off
call "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvarsall.bat" x64
if %errorlevel% neq 0 exit /b 1
echo VS env ready

set NATIVE_DIR=D:\agent\booming-il2cpp\testing\foundation-dll\System.Private.CoreLib\convert-char\native
set BUILD_DIR=%NATIVE_DIR%\build_jit

:: Configure
cmake -S %NATIVE_DIR% -B %BUILD_DIR% -G Ninja -DCHAOS_IL2CPP_CONFIG_TIER=profile -DCHAOS_IL2CPP_JIT_MODE=ON -DCMAKE_BUILD_TYPE=profile -DCHAOS_SDK_DIR=D:\agent\booming-il2cpp\chaos-sdk || exit /b 1

:: Build
cmake --build %BUILD_DIR% -j || exit /b 1

echo Build complete
