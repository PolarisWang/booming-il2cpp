@echo off
SETLOCAL ENABLEDELAYEDEXPANSION
CALL "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvarsall.bat" x64 >nul 2>nul
if %ERRORLEVEL% neq 0 exit /b 1

cd /d D:\agent\booming-il2cpp

set EXE=verification\foundation-dll\System.Private.CoreLib\convert-char\il2cpp_dist\hotupdate\build\d3_hotupdate_convert-char.exe

echo Starting hotupdate test...
%EXE%
set E=%ERRORLEVEL%
echo EXIT_CODE=%E%
exit /b %E%
