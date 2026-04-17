@echo off
REM Stop the brainstorm server and clean up
REM Usage: stop-server.bat <screen_dir>
REM
REM Kills the server process. Only deletes session directory if it's
REM under %TEMP% (ephemeral). Persistent directories (.booming\) are
REM kept so mockups can be reviewed later.

setlocal enabledelayedexpansion

set "SCREEN_DIR=%~1"

if "!SCREEN_DIR!"=="" (
    echo {"error": "Usage: stop-server.bat ^<screen_dir^>"}
    exit /b 1
)

set "PID_FILE=!SCREEN_DIR!\.server.pid"

if exist "!PID_FILE!" (
    set /p SERVER_PID=<"!PID_FILE!"
    taskkill /PID !SERVER_PID! /F >nul 2>&1
    del /f "!PID_FILE!" >nul 2>&1
    del /f "!SCREEN_DIR!\.server.log" >nul 2>&1

    REM Only delete ephemeral %TEMP% directories
    echo !SCREEN_DIR! | findstr /i "^%TEMP%" >nul 2>&1
    if not errorlevel 1 (
        rmdir /s /q "!SCREEN_DIR!" >nul 2>&1
    )

    echo {"status": "stopped"}
) else (
    echo {"status": "not_running"}
)

exit /b 0
