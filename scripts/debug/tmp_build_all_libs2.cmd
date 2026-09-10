@echo off
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvarsall.bat" x64
if %ERRORLEVEL% neq 0 exit /b 1

set ARTIFACTS=D:\agent\booming-il2cpp\artifacts\presets\windows-x64-reference
set SDK_LIB=D:\agent\booming-il2cpp\testing\foundation-dll\System.Private.CoreLib\chunks\buffers\build_jit_output\codegen\lib

echo === Step 1: Build chaos_common.lib ===
MSBuild.exe "%ARTIFACTS%\src\native\common\chaos_common.vcxproj" /p:Configuration=RelWithDebInfo /p:Platform=x64 /p:BuildProjectReferences=false /t:Build
if %ERRORLEVEL% neq 0 (
    echo chaos_common build FAILED
    exit /b 1
)

echo === Step 2: Build chaos_jit.lib ===
MSBuild.exe "%ARTIFACTS%\src\native\jit\chaos_jit.vcxproj" /p:Configuration=RelWithDebInfo /p:Platform=x64 /p:BuildProjectReferences=false /t:Build
if %ERRORLEVEL% neq 0 (
    echo chaos_jit build FAILED
    exit /b 1
)

echo === Step 3: Copy ALL libs to SDK ===
copy /Y "%ARTIFACTS%\lib\*.lib" "%SDK_LIB%"
copy /Y "%ARTIFACTS%\src\native\jit\RelWithDebInfo\chaos_jit.lib" "%SDK_LIB%"
copy /Y "%ARTIFACTS%\src\native\common\RelWithDebInfo\chaos_common.lib" "%SDK_LIB%"

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
