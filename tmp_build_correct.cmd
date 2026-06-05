@echo off
call "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvarsall.bat" x64
if %ERRORLEVEL% neq 0 exit /b 1

echo === Step 1: Build chaos_jit.lib using the correct cmake preset ===
MSBuild.exe "D:\agent\booming-il2cpp\artifacts\presets\windows-x64-reference\src\native\jit\chaos_jit.vcxproj" /p:Configuration=RelWithDebInfo /p:Platform=x64 /p:BuildProjectReferences=false /t:Build
if %ERRORLEVEL% neq 0 (
    echo chaos_jit build FAILED
    exit /b 1
)

echo === Step 2: Copy chaos_jit.lib to artifacts/lib ===
copy /Y "D:\agent\booming-il2cpp\artifacts\presets\windows-x64-reference\src\native\jit\RelWithDebInfo\chaos_jit.lib" "D:\agent\booming-il2cpp\artifacts\presets\windows-x64-reference\lib\chaos_jit.lib"
if %ERRORLEVEL% neq 0 (
    echo Copy FAILED
    exit /b 1
)

echo === Step 3: Copy ALL libs from artifacts to SDK ===
set SDK_LIB=D:\agent\booming-il2cpp\testing\foundation-dll\System.Private.CoreLib\chunks\buffers\build_jit_output\codegen\lib
copy /Y "D:\agent\booming-il2cpp\artifacts\presets\windows-x64-reference\lib\*.lib" "%SDK_LIB%"
if %ERRORLEVEL% neq 0 (
    echo Copy libs FAILED
    exit /b 1
)

echo === Step 4: Rebuild entry-jit ===
MSBuild.exe "D:\agent\booming-il2cpp\testing\foundation-dll\System.Private.CoreLib\chunks\buffers\build_jit_output\build_vs\chaos_entry.vcxproj" /p:Configuration=RelWithDebInfo /p:Platform=x64 /t:Build
if %ERRORLEVEL% neq 0 (
    echo entry-jit build FAILED
    exit /b 1
)

echo === Step 5: Copy entry-jit.exe to root ===
copy /Y "D:\agent\booming-il2cpp\testing\foundation-dll\System.Private.CoreLib\chunks\buffers\build_jit_output\build_vs\RelWithDebInfo\chaos_entry.exe" "D:\agent\booming-il2cpp\entry-jit.exe"
if %ERRORLEVEL% neq 0 (
    echo Copy entry-jit FAILED
    exit /b 1
)

echo === ALL DONE ===
exit /b 0
