@echo off
call "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvarsall.bat" x64 >nul 2>&1
echo === standalone chaos_runtime_core.lib ===
dumpbin /directives "D:\agent\booming-il2cpp\build\native-runtime\Release\chaos_runtime_core.lib" 2>nul | findstr /i "defaultlib"
echo === main build chaos_bootstrap.lib ===
dumpbin /directives "D:\agent\booming-il2cpp\build\native\src\native\bootstrap\Release\chaos_bootstrap.lib" 2>nul | findstr /i "defaultlib"
echo === main build chaos_runtime_core.lib ===
dumpbin /directives "D:\agent\booming-il2cpp\build\native\src\native\runtime-core\Release\chaos_runtime_core.lib" 2>nul | findstr /i "defaultlib"
