@echo off
call "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvarsall.bat" x64 >nul 2>&1
set LIBDIR=C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Tools\MSVC\14.38.33130\lib\x64

echo Files in %LIBDIR%:
dir /b "%LIBDIR%\msvcp*.lib" "%LIBDIR%\libcp*.lib" 2>nul
echo.

echo Searching for __std_find_end_1:
for %%l in (msvcp140 libcpmt libcpmt1 concrt140 msvcrt libcmt libucrt) do (
    dumpbin /symbols "%LIBDIR%\%%l.lib" 2>nul | findstr /c:"__std_find_end_1" >nul && echo FOUND in %%l.lib
)
echo Searching for _Thrd_sleep_for:
for %%l in (msvcp140 libcpmt libcpmt1 concrt140 msvcrt libcmt libucrt) do (
    dumpbin /symbols "%LIBDIR%\%%l.lib" 2>nul | findstr /c:"_Thrd_sleep_for" >nul && echo FOUND in %%l.lib
)
echo Searching for _Cnd_timedwait_for_unchecked:
for %%l in (msvcp140 libcpmt libcpmt1 concrt140 msvcrt libcmt libucrt) do (
    dumpbin /symbols "%LIBDIR%\%%l.lib" 2>nul | findstr /c:"_Cnd_timedwait_for_unchecked" >nul && echo FOUND in %%l.lib
)
echo Done.
