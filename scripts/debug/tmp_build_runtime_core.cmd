@echo off
call "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvarsall.bat" x64
if %ERRORLEVEL% neq 0 exit /b 1

cd /d D:\agent\booming-il2cpp\build\native

echo === Build chaos_runtime_core.lib (Release) with explicit /std:c++17 ===

REM Add /std:c++17 to the CL_AdditionalOptions for runtime-core
set CL=/std:c++17 %CL%

MSBuild.exe "src\native\runtime-core\chaos_runtime_core.vcxproj" /p:Configuration=Release /p:Platform=x64 /p:BuildProjectReferences=false /t:Build
if %ERRORLEVEL% neq 0 (
    echo chaos_runtime_core build FAILED
    exit /b 1
)

echo === chaos_runtime_core.lib built OK ===
exit /b 0
