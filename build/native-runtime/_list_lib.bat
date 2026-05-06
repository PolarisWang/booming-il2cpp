@echo off
call "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvarsall.bat" x64 >nul
lib.exe /list "D:\agent\booming-il2cpp\build\native-runtime\Release\chaos_runtime_core.lib"
