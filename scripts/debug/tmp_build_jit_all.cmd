@echo off
call "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvarsall.bat" x64
if %ERRORLEVEL% neq 0 exit /b 1

echo === Step 1: Build chaos_jit.lib ===
cd /d D:\agent\booming-il2cpp\build\native
MSBuild.exe "src/native/jit/chaos_jit.vcxproj" /p:Configuration=Debug /p:Platform=x64 /t:Build
if %ERRORLEVEL% neq 0 (
    echo chaos_jit build FAILED
    exit /b 1
)

echo === Step 2: Copy chaos_jit.lib to SDK ===
copy /Y "D:\agent\booming-il2cpp\build\native\src\native\jit\Debug\chaos_jit.lib" "D:\agent\booming-il2cpp\testing\foundation-dll\System.Private.CoreLib\chunks\buffers\build_jit_output\codegen\lib\chaos_jit.lib"
if %ERRORLEVEL% neq 0 (
    echo Copy chaos_jit.lib FAILED
    exit /b 1
)

echo === Step 3: Rebuild entry-jit ===
MSBuild.exe "D:\agent\booming-il2cpp\testing\foundation-dll\System.Private.CoreLib\chunks\buffers\build_jit_output\build_vs/chaos_entry.vcxproj" /p:Configuration=RelWithDebInfo /p:Platform=x64 /t:Build
if %ERRORLEVEL% neq 0 (
    echo entry-jit build FAILED
    exit /b 1
)

echo === DONE ===
