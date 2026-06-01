@echo off
call "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvarsall.bat" x64
if %errorlevel% neq 0 exit /b 1
dumpbin /symbols "D:\agent\booming-il2cpp\testing\foundation-dll\System.Private.CoreLib\convert-char\codegen\ConvertCharSubjects\lib\chaos_runtime_core.lib" 2>&1 | findstr /i "GetOriginalAotPtr"
if %errorlevel% equ 0 (
    echo SYMBOL FOUND
) else (
    echo SYMBOL NOT FOUND
)
