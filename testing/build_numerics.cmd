@echo off
call "C:\Program Files\Microsoft Visual Studio\2022\Professional\Common7\Tools\VsDevCmd.bat" -arch=x64 >nul 2>&1
cd /d "D:\agent\booming-il2cpp\testing\foundation-dll\System.Private.CoreLib\chunks\numerics\native\build"
if not exist CMakeCache.txt (
    cmake -S "D:\agent\booming-il2cpp\testing\foundation-dll\System.Private.CoreLib\chunks\numerics\native" -B . -G Ninja -DCHAOS_IL2CPP_CONFIG_TIER=check -DCHAOS_IL2CPP_JIT_MODE=OFF
    if errorlevel 1 exit /b %errorlevel%
)
ninja
echo EXIT_CODE=%ERRORLEVEL%
