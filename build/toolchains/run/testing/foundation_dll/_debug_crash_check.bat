@echo off
call "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvarsall.bat" x64 -vcvars_ver=14.42 >nul 2>nul

if "%~1"=="/compile" goto :compile
if "%~1"=="/run" goto :run
exit /b 1

:compile
cl /nologo /std:c++20 /c /EHsc /W3 /utf-8 /O2 ^
  -I"D:\agent\booming-il2cpp\src\native\common" ^
  -I"D:\agent\booming-il2cpp\src\native\common\chaos" ^
  -I"D:\agent\booming-il2cpp\contracts\native\v0" ^
  -I"D:\agent\booming-il2cpp\src\native\runtime-core" ^
  -I"D:\agent\booming-il2cpp\third_party\fmt\include" ^
  -DCHAOS_IL2CPP_CHECK ^
  -Fo"%~2" ^
  "%~3"
exit /b %ERRORLEVEL%

:run
"%~2"
exit /b %ERRORLEVEL%