@echo off
call "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvarsall.bat" x64 >nul 2>&1
set L=C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Tools\MSVC\14.38.33130\lib\x64

echo === libcpmt.lib ===
dumpbin /symbols "%L%\libcpmt.lib" 2>nul | findstr /c:"__std_find_end" >nul && echo FOUND || echo NOT_FOUND

echo === msvcprt.lib ===
dumpbin /symbols "%L%\msvcprt.lib" 2>nul | findstr /c:"__std_find_end" >nul && echo FOUND || echo NOT_FOUND

echo === libcmt.lib ===
dumpbin /symbols "%L%\libcmt.lib" 2>nul | findstr /c:"__std_find_end" >nul && echo FOUND || echo NOT_FOUND

echo === msvcrt.lib ===
dumpbin /symbols "%L%\msvcrt.lib" 2>nul | findstr /c:"__std_find_end" >nul && echo FOUND || echo NOT_FOUND
