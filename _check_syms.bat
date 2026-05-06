@echo off
call "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvarsall.bat" x64 >nul 2>&1
set L=C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Tools\MSVC\14.38.33130\lib\x64
dumpbin /symbols "%L%\msvcprt.lib" 2>nul > %TEMP%\msvcprt_syms.txt
findstr /c:"__std_find_end" %TEMP%\msvcprt_syms.txt && echo FOUND_in_msvcprt
findstr /c:"_Thrd_sleep_for" %TEMP%\msvcprt_syms.txt && echo FOUND_Thrd_in_msvcprt
findstr /c:"_Cnd_timedwait_for_unchecked" %TEMP%\msvcprt_syms.txt && echo FOUND_Cnd_in_msvcprt
dumpbin /symbols "%L%\libcpmt.lib" 2>nul > %TEMP%\libcpmt_syms.txt
findstr /c:"__std_find_end" %TEMP%\libcpmt_syms.txt && echo FOUND_in_libcpmt
findstr /c:"_Thrd_sleep_for" %TEMP%\libcpmt_syms.txt && echo FOUND_Thrd_in_libcpmt
findstr /c:"_Cnd_timedwait_for_unchecked" %TEMP%\libcpmt_syms.txt && echo FOUND_Cnd_in_libcpmt
echo.
echo Total __std_find_end lines:
findstr /c:"__std_find_end" %TEMP%\libcpmt_syms.txt | find /c /v ""
echo Total _Thrd_sleep_for lines in libcpmt:
findstr /c:"_Thrd_sleep_for" %TEMP%\libcpmt_syms.txt | find /c /v ""
