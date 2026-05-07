@echo off
call "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars64.bat" x64 -vcvars_ver=14.42 >nul 2>nul
if %ERRORLEVEL% NEQ 0 exit /b %ERRORLEVEL%

set REPO=D:\agent\booming-il2cpp
set BUILD=%REPO%\verification\foundation-dll\System.Private.CoreLib\convert-char\native_test\l2-verify\build

set NATIVE_LIB=%REPO%\build\native\src\native
set BDWGC=%REPO%\build\native\bdwgc_build

set LIBS=%BUILD%\gen_for_bisect.obj

echo === Run STAGE 1 ===
"%BUILD%\t1.exe"
echo RC1=%ERRORLEVEL%

echo === Run STAGE 2 ===
"%BUILD%\t2.exe"
echo RC2=%ERRORLEVEL%

echo === Run STAGE 3 ===
"%BUILD%\t3.exe"
echo RC3=%ERRORLEVEL%

echo === Run STAGE 4 ===
"%BUILD%\t4.exe"
echo RC4=%ERRORLEVEL%
