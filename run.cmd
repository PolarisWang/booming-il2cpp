@echo off
setlocal
set "BOOM_RUN_REQUEST_WINDOWS_TERMINAL="

if /I "%BOOM_RUN_USE_WINDOWS_TERMINAL%"=="0" goto launch
if defined WT_SESSION goto launch
if defined CI goto launch
if defined GITHUB_ACTIONS goto launch
if defined TF_BUILD goto launch

echo(%CMDCMDLINE%| findstr /I /C:" /c " >nul
if not errorlevel 1 goto launch

set "BOOM_RUN_REQUEST_WINDOWS_TERMINAL=1"

:launch
powershell -NoLogo -NoProfile -ExecutionPolicy Bypass -File "%~dp0run.ps1" %*
exit /b %ERRORLEVEL%
