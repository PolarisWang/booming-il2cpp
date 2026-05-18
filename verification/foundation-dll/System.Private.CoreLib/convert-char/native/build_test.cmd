@echo off
set VCROOT=C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Tools\MSVC\14.29.30133
set SDKROOT=C:\Program Files (x86)\Windows Kits\10
set INCLUDE=%VCROOT%\include;%SDKROOT%\Include\10.0.22621.0\ucrt;%SDKROOT%\Include\10.0.22621.0\shared;%SDKROOT%\Include\10.0.22621.0\um
set LIB=%VCROOT%\lib\x64;%SDKROOT%\Lib\10.0.22621.0\ucrt\x64;%SDKROOT%\Lib\10.0.22621.0\um\x64

echo === Test with /EHa ===
"%VCROOT%\bin\HostX64\x64\cl.exe" /EHa /O2 /utf-8 /Fe:%~dp0test_throw_eha.exe %~dp0test_throw.cpp 2>&1
"%~dp0test_throw_eha.exe"
echo EXIT: %ERRORLEVEL%

echo.
echo === Test with /EHsc (control) ===
"%VCROOT%\bin\HostX64\x64\cl.exe" /EHsc /O2 /utf-8 /Fe:%~dp0test_throw_ehsc.exe %~dp0test_throw.cpp 2>&1
"%~dp0test_throw_ehsc.exe"
echo EXIT: %ERRORLEVEL%
