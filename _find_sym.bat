@echo off
call "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvarsall.bat" x64 >nul 2>&1
for /f "delims=" %%i in ('dir /b "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Tools\MSVC"') do set V=%%i
set LIBDIR=C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Tools\MSVC\%V%\lib\x64

echo Searching for __std_find_end_1 in %LIBDIR%
echo.

for %%l in (*.lib) do (
    dumpbin /symbols "%LIBDIR%\%%l" 2>nul | findstr /c:"__std_find_end_1" >nul && echo FOUND in %%l
)
echo Done.
