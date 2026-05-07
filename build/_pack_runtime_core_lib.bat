@echo off
call "C:/Program Files (x86)/Microsoft Visual Studio/2022/BuildTools/VC/Auxiliary/Build/vcvarsall.bat" x64 >nul 2>nul
if %ERRORLEVEL% neq 0 exit /b 1
lib /nologo /out:"build/native/src/native/runtime-core/RelWithDebInfo/chaos_runtime_core.lib" "build/native/src/native/runtime-core/chaos_runtime_core.dir/RelWithDebInfo/"*.obj
if %ERRORLEVEL% neq 0 exit /b 1
echo BUILD OK
