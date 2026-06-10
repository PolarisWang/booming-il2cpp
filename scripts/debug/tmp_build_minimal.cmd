@echo off
call "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvarsall.bat" x64
if %ERRORLEVEL% neq 0 exit /b 1

cd /d D:\agent\booming-il2cpp\build\native

echo === Building chaos_jit.lib only (Release) ===
MSBuild.exe "src/native/jit/chaos_jit.vcxproj" /p:Configuration=Release /p:Platform=x64 /p:BuildProjectReferences=false /t:Build
if %ERRORLEVEL% neq 0 (
    echo chaos_jit build FAILED
    exit /b 1
)

echo === Copy chaos_jit.lib to SDK ===
copy /Y "src\native\jit\Release\chaos_jit.lib" "D:\agent\booming-il2cpp\testing\foundation-dll\System.Private.CoreLib\chunks\buffers\build_jit_output\codegen\lib\chaos_jit.lib"
if %ERRORLEVEL% neq 0 (
    echo Copy FAILED
    exit /b 1
)

echo === Build entry-jit ===
MSBuild.exe "D:\agent\booming-il2cpp\testing\foundation-dll\System.Private.CoreLib\chunks\buffers\build_jit_output\build_vs\chaos_entry.vcxproj" /p:Configuration=RelWithDebInfo /p:Platform=x64 /t:Build
if %ERRORLEVEL% neq 0 (
    echo entry-jit build FAILED
    exit /b 1
)

echo === Copy entry-jit to root ===
copy /Y "D:\agent\booming-il2cpp\testing\foundation-dll\System.Private.CoreLib\chunks\buffers\build_jit_output\build_vs\RelWithDebInfo\chaos_entry.exe" "D:\agent\booming-il2cpp\entry-jit.exe"
if %ERRORLEVEL% neq 0 (
    echo Copy entry-jit FAILED
    exit /b 1
)

echo === ALL DONE ===
exit /b 0
