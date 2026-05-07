@echo off
call "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars64.bat" x64 -vcvars_ver=14.42 >nul 2>nul
if %ERRORLEVEL% NEQ 0 exit /b %ERRORLEVEL%

set REPO=D:\agent\booming-il2cpp
set BUILD=%REPO%\verification\foundation-dll\System.Private.CoreLib\convert-char\native_test\l2-verify\build
set NATIVE_LIB=%REPO%\build\native\src\native
set BDWGC=%REPO%\build\native\bdwgc_build
set LIBS=%BUILD%\gen_for_bisect.obj
set LINKLIBS="%NATIVE_LIB%\runtime-core\Release\chaos_runtime_core.lib" "%NATIVE_LIB%\bootstrap\Release\chaos_bootstrap.lib" "%NATIVE_LIB%\interpreter\Release\chaos_interpreter.lib" "%NATIVE_LIB%\common\Release\chaos_common.lib" "%NATIVE_LIB%\support\Release\chaos_support.lib" "%NATIVE_LIB%\hot-update\Release\chaos_hot_update.lib" "%REPO%\build\native\fmt_build\Release\chaos_fmt.lib" "%BDWGC%\Release\chaos_bdwgc.lib" ole32.lib user32.lib

echo === Link STAGE 1 ===
link /nologo /out:"%BUILD%\t1.exe" "%BUILD%\t1.obj" %LIBS% %LINKLIBS% 2>&1 || exit /b 1
"%BUILD%\t1.exe"
echo RC1=%ERRORLEVEL%

echo === Link STAGE 2 ===
link /nologo /out:"%BUILD%\t2.exe" "%BUILD%\t2.obj" %LIBS% %LINKLIBS% 2>&1 || exit /b 1
"%BUILD%\t2.exe"
echo RC2=%ERRORLEVEL%

echo === Link STAGE 3 ===
link /nologo /out:"%BUILD%\t3.exe" "%BUILD%\t3.obj" %LIBS% %LINKLIBS% 2>&1 || exit /b 1
"%BUILD%\t3.exe"
echo RC3=%ERRORLEVEL%

echo === Link STAGE 4 ===
link /nologo /out:"%BUILD%\t4.exe" "%BUILD%\t4.obj" %LIBS% %LINKLIBS% 2>&1 || exit /b 1
"%BUILD%\t4.exe"
echo RC4=%ERRORLEVEL%
