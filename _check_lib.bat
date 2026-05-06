@echo off
call "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvarsall.bat" x64 >nul 2>&1
echo --- LIB ---
echo %LIB%
echo --- INCLUDE ---
echo %INCLUDE%
echo --- Link with explicit STL libs ---
cd /d "D:\agent\booming-il2cpp\verification\foundation-dll\System.Private.CoreLib\convert-char\native_test\l2-verify\build"
set NATIVE_DIR=D:\agent\booming-il2cpp\build\native
link.exe /nologo /out:verify_convert-char.exe ^
    native_verify_main.obj ^
    native-aot.generated.obj ^
    D:\agent\booming-il2cpp\build\native-runtime\Release\chaos_runtime_core.lib ^
    "%NATIVE_DIR%\src\native\bootstrap\Release\chaos_bootstrap.lib" ^
    "%NATIVE_DIR%\src\native\interpreter\Release\chaos_interpreter.lib" ^
    "%NATIVE_DIR%\src\native\common\Release\chaos_common.lib" ^
    "%NATIVE_DIR%\src\native\support\Release\chaos_support.lib" ^
    "%NATIVE_DIR%\src\native\hot-update\Release\chaos_hot_update.lib" ^
    "%NATIVE_DIR%\bdwgc_build\Release\chaos_bdwgc.lib" ^
    ole32.lib user32.lib ^
    legacy_stdio_definitions.lib
echo EXIT=%ERRORLEVEL%
