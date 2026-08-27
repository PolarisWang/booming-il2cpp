@echo off
call "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars64.bat" >nul 2>&1
if errorlevel 1 (
  echo VCVARS_FAILED
  exit /b 1
)
cl /nologo /EHsc "%~dp0wct_deadlock_spy.cpp" /Fe:"%~dp0wct_deadlock_spy.exe"
exit /b %errorlevel%
