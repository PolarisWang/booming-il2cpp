@echo off
REM Start the brainstorm server and output connection info
REM Usage: start-server.bat [--project-dir <path>] [--host <bind-host>] [--url-host <display-host>] [--foreground] [--background]
REM
REM Starts server on a random high port, outputs JSON with URL.
REM Each session gets its own directory to avoid conflicts.
REM
REM Options:
REM   --project-dir <path>  Store session files under <path>\.booming\brainstorm\
REM                         instead of %TEMP%. Files persist after server stops.
REM   --host <bind-host>    Host/interface to bind (default: 127.0.0.1).
REM   --url-host <host>     Hostname shown in returned URL JSON.
REM   --foreground          Run server in the current terminal (no backgrounding).
REM   --background          Force background mode.

setlocal enabledelayedexpansion

set "SCRIPT_DIR=%~dp0"
set "PROJECT_DIR="
set "FOREGROUND=false"
set "FORCE_BACKGROUND=false"
set "BIND_HOST=127.0.0.1"
set "URL_HOST="

REM Parse arguments
:parse_args
if "%~1"=="" goto done_parse
if "%~1"=="--project-dir" (
    set "PROJECT_DIR=%~2"
    shift
    shift
    goto parse_args
)
if "%~1"=="--host" (
    set "BIND_HOST=%~2"
    shift
    shift
    goto parse_args
)
if "%~1"=="--url-host" (
    set "URL_HOST=%~2"
    shift
    shift
    goto parse_args
)
if "%~1"=="--foreground" (
    set "FOREGROUND=true"
    shift
    goto parse_args
)
if "%~1"=="--no-daemon" (
    set "FOREGROUND=true"
    shift
    goto parse_args
)
if "%~1"=="--background" (
    set "FORCE_BACKGROUND=true"
    shift
    goto parse_args
)
if "%~1"=="--daemon" (
    set "FORCE_BACKGROUND=true"
    shift
    goto parse_args
)
echo {"error": "Unknown argument: %~1"}
exit /b 1

:done_parse

REM Set URL_HOST default
if "!URL_HOST!"=="" (
    if "!BIND_HOST!"=="127.0.0.1" (
        set "URL_HOST=localhost"
    ) else if "!BIND_HOST!"=="localhost" (
        set "URL_HOST=localhost"
    ) else (
        set "URL_HOST=!BIND_HOST!"
    )
)

REM Generate unique session ID using PID and timestamp
for /f "tokens=1-4 delims=:.," %%a in ("%TIME%") do (
    set "TS=%%a%%b%%c%%d"
)
set "TS=!TS: =0!"
set "SESSION_ID=%~1!TS!"
for /f %%i in ('powershell -command "[System.Diagnostics.Process]::GetCurrentProcess().Id"') do set "SESSION_ID=%%i-!TS!"

REM Set session directory
if not "!PROJECT_DIR!"=="" (
    set "SCREEN_DIR=!PROJECT_DIR!\.booming\brainstorm\!SESSION_ID!"
) else (
    set "SCREEN_DIR=%TEMP%\brainstorm-!SESSION_ID!"
)

set "PID_FILE=!SCREEN_DIR!\.server.pid"
set "LOG_FILE=!SCREEN_DIR!\.server.log"

REM Create session directory
if not exist "!SCREEN_DIR!" mkdir "!SCREEN_DIR!"

REM Kill any existing server from old PID file
if exist "!PID_FILE!" (
    set /p OLD_PID=<"!PID_FILE!"
    taskkill /PID !OLD_PID! /F >nul 2>&1
    del /f "!PID_FILE!" >nul 2>&1
)

cd /d "!SCRIPT_DIR!"

REM Foreground mode
if "!FOREGROUND!"=="true" (
    echo !CURRENT_PID!>"!PID_FILE!"
    set "BRAINSTORM_DIR=!SCREEN_DIR!"
    set "BRAINSTORM_HOST=!BIND_HOST!"
    set "BRAINSTORM_URL_HOST=!URL_HOST!"
    node server.js
    exit /b %ERRORLEVEL%
)

REM Background mode - start server detached
set "BRAINSTORM_DIR=!SCREEN_DIR!"
set "BRAINSTORM_HOST=!BIND_HOST!"
set "BRAINSTORM_URL_HOST=!URL_HOST!"
start /b cmd /c "node "!SCRIPT_DIR!server.js" > "!LOG_FILE!" 2>&1"

REM Get PID of the node process just started (approximate)
timeout /t 1 /nobreak >nul
for /f "tokens=2" %%i in ('tasklist /fi "imagename eq node.exe" /fo list ^| findstr "PID:"') do (
    set "SERVER_PID=%%i"
)

if not "!SERVER_PID!"=="" (
    echo !SERVER_PID!>"!PID_FILE!"
)

REM Wait for server-started message in log file (up to 5 seconds)
set /a ATTEMPTS=0
:wait_loop
if !ATTEMPTS! geq 50 goto timeout_error
set /a ATTEMPTS+=1
timeout /t 0 /nobreak >nul 2>&1
ping -n 1 -w 100 127.0.0.1 >nul 2>&1

if exist "!LOG_FILE!" (
    findstr /c:"server-started" "!LOG_FILE!" >nul 2>&1
    if not errorlevel 1 (
        for /f "delims=" %%l in ('findstr /c:"server-started" "!LOG_FILE!"') do (
            echo %%l
            goto done
        )
    )
)
goto wait_loop

:timeout_error
echo {"error": "Server failed to start within 5 seconds"}
exit /b 1

:done
exit /b 0
