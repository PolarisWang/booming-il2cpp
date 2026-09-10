@echo off
REM Batch pipeline runner for all families
REM Logs to build/testing/results/batch-run.log

set LOGFILE=D:\agent\booming-il2cpp\build\testing\results\batch-run.log
set SUMMARY=D:\agent\booming-il2cpp\build\testing\results\p2-baseline-summary.json
set PYTHON=C:\Program Files\Python312\python.exe

if not exist "D:\agent\booming-il2cpp\build\testing\results" mkdir "D:\agent\booming-il2cpp\build\testing\results"

echo Starting batch run at %DATE% %TIME% > "%LOGFILE%"
echo Starting batch run at %DATE% %TIME%

cd /d D:\agent\booming-il2cpp

REM Collect family list
setlocal enabledelayedexpansion
set COUNT=0
for /d %%d in ("D:\agent\booming-il2cpp\testing\foundation-dll\System.Private.CoreLib\*") do (
    set NAME=%%~nxd
    if not "!NAME!"=="reports" if not "!NAME!"=="results" if not "!NAME!"=="System.Private.CoreLib" (
        set /a COUNT+=1
        set FAMILY[!COUNT!]=!NAME!
    )
)
echo Found %COUNT% families >> "%LOGFILE%"
echo Found %COUNT% families

set PASSED=0
set FAILED=0

for /l %%i in (1,1,%COUNT%) do (
    set FAM=!FAMILY[%%i]!
    echo. >> "%LOGFILE%"
    echo ===== [%%i/%COUNT%] !FAM! ===== >> "%LOGFILE%"
    echo ===== [%%i/%COUNT%] !FAM! =====

    "%PYTHON%" testing/foundation-dll/run_pipeline.py !FAM! >> "%LOGFILE%" 2>&1
    set ERRLEV=!ERRORLEVEL!

    if !ERRLEV! equ 0 (
        set /a PASSED+=1
        echo   PASS >> "%LOGFILE%"
        echo   PASS
    ) else (
        set /a FAILED+=1
        echo   FAIL (exit=!ERRLEV!) >> "%LOGFILE%"
        echo   FAIL (exit=!ERRLEV!)
    )
)

echo. >> "%LOGFILE%"
echo ===== DONE: %PASSED% passed, %FAILED% failed, %COUNT% total ===== >> "%LOGFILE%"
echo ===== DONE: %PASSED% passed, %FAILED% failed, %COUNT% total =====
echo Completed at %DATE% %TIME% >> "%LOGFILE%"
