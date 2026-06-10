@echo off
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvarsall.bat" x64 >nul
dumpbin /symbols D:\agent\booming-il2cpp\build\native-profile\src\native\jit\chaos_jit.dir\Release\jit_precode.obj 2>&1 | findstr /i "KeepNative"
if %errorlevel% equ 0 (
    echo KEEPNATIVE_FOUND
) else (
    echo KEEPNATIVE_NOT_FOUND
)
