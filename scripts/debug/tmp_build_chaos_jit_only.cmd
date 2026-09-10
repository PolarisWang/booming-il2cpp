@echo off
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvarsall.bat" x64
if %ERRORLEVEL% neq 0 exit /b 1

echo === Build chaos_jit.lib (BuildTools toolchain) ===
MSBuild.exe "D:\agent\booming-il2cpp\artifacts\presets\windows-x64-reference\src\native\jit\chaos_jit.vcxproj" /p:Configuration=RelWithDebInfo /p:Platform=x64 /p:BuildProjectReferences=false /t:Build
if %ERRORLEVEL% neq 0 (
    echo chaos_jit build FAILED
    exit /b 1
)

echo === Copy chaos_jit.lib to SDK ===
copy /Y "D:\agent\booming-il2cpp\artifacts\presets\windows-x64-reference\src\native\jit\RelWithDebInfo\chaos_jit.lib" "D:\agent\booming-il2cpp\testing\foundation-dll\System.Private.CoreLib\chunks\buffers\build_jit_output\codegen\lib\chaos_jit.lib"
if %ERRORLEVEL% neq 0 (
    echo Copy FAILED
    exit /b 1
)

echo === Rebuild entry-jit ===
MSBuild.exe "D:\agent\booming-il2cpp\testing\foundation-dll\System.Private.CoreLib\chunks\buffers\build_jit_output\build_vs\chaos_entry.vcxproj" /p:Configuration=RelWithDebInfo /p:Platform=x64 /t:Build
if %ERRORLEVEL% neq 0 (
    echo entry-jit build FAILED
    exit /b 1
)

echo === Copy entry-jit.exe to root ===
copy /Y "D:\agent\booming-il2cpp\testing\foundation-dll\System.Private.CoreLib\chunks\buffers\build_jit_output\build_vs\RelWithDebInfo\chaos_entry.exe" "D:\agent\booming-il2cpp\entry-jit.exe"
if %ERRORLEVEL% neq 0 (
    echo Copy entry-jit FAILED
    exit /b 1
)

echo === ALL DONE ===
exit /b 0
