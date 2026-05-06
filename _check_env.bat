@echo off
call "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvarsall.bat" x64 >nul 2>&1
echo === LIB ===
echo %LIB%
echo.
echo === Checking for specific libs ===
dir /b C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Tools\MSVC\14.42.34433\lib\x64\msvcp*.lib 2>nul
dir /b C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Tools\MSVC\14.42.34433\lib\x64\libcp*.lib 2>nul
echo.
echo === Windows Kits ===
dir /b "C:\Program Files (x86)\Windows Kits\10\Lib\10.0.26100.0\ucrt\x64\*.lib" 2>nul
