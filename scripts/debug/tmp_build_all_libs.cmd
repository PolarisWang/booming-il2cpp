@echo off
call "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvarsall.bat" x64
if %ERRORLEVEL% neq 0 exit /b 1

cd /d D:\agent\booming-il2cpp

echo === Step 1: Build chaos_jit.lib (Release, skip references) ===
MSBuild.exe "build\native\src\native\jit\chaos_jit.vcxproj" /p:Configuration=Release /p:Platform=x64 /p:BuildProjectReferences=false /t:Build
if %ERRORLEVEL% neq 0 (
    echo chaos_jit build FAILED
    exit /b 1
)

echo === Step 2: Copy ALL libs from build/native Release dirs to SDK ===
set SDK_LIB=D:\agent\booming-il2cpp\testing\foundation-dll\System.Private.CoreLib\chunks\buffers\build_jit_output\codegen\lib

copy /Y "build\native\src\native\jit\Release\chaos_jit.lib" "%SDK_LIB%"

for %%D in (common support pal runtime-core interpreter bootstrap hot-update diagnostics/eventpipe diagnostics/debugger) do (
    for /f %%F in ('dir /b "build\native\src\native\%%D\Release\*.lib" 2^>nul') do (
        copy /Y "build\native\src\native\%%D\Release\%%F" "%SDK_LIB%"
    )
)
copy /Y "build\native\fmt_build\Release\chaos_fmt.lib" "%SDK_LIB%" 2>nul

echo === Step 3: Rebuild entry-jit ===
MSBuild.exe "testing\foundation-dll\System.Private.CoreLib\chunks\buffers\build_jit_output\build_vs\chaos_entry.vcxproj" /p:Configuration=RelWithDebInfo /p:Platform=x64 /t:Build
if %ERRORLEVEL% neq 0 (
    echo entry-jit build FAILED
    exit /b 1
)

echo === Step 4: Copy entry-jit.exe to root ===
copy /Y "testing\foundation-dll\System.Private.CoreLib\chunks\buffers\build_jit_output\build_vs\RelWithDebInfo\chaos_entry.exe" "entry-jit.exe"
if %ERRORLEVEL% neq 0 (
    echo Copy FAILED
    exit /b 1
)

echo === ALL DONE ===
exit /b 0
