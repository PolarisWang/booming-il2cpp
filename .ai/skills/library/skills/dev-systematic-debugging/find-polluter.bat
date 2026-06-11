@echo off
REM Bisection script to find which test creates unwanted files/state
REM Usage: find-polluter.bat <file_or_dir_to_check> <test_pattern>
REM Example: find-polluter.bat ".git" "src\**\*.test.ts"

setlocal enabledelayedexpansion

if "%~2"=="" (
    echo Usage: %~nx0 ^<file_to_check^> ^<test_pattern^>
    echo Example: %~nx0 ".git" "src\**\*.test.ts"
    exit /b 1
)

set "POLLUTION_CHECK=%~1"
set "TEST_PATTERN=%~2"

echo [34mSearching for test that creates: !POLLUTION_CHECK![0m
echo Test pattern: !TEST_PATTERN!
echo.

REM Collect test files matching pattern
set "TMPFILE=%TEMP%\test_files_%RANDOM%.txt"
for /r %%f in (!TEST_PATTERN!) do (
    echo %%f >> "!TMPFILE!"
)

if not exist "!TMPFILE!" (
    echo No test files found matching: !TEST_PATTERN!
    exit /b 0
)

REM Count lines
set /a TOTAL=0
for /f %%c in ('find /c /v "" "!TMPFILE!"') do set "TOTAL=%%c"

echo Found !TOTAL! test files
echo.

set /a COUNT=0
for /f "delims=" %%f in (!TMPFILE!) do (
    set /a COUNT+=1
    set "TEST_FILE=%%f"

    REM Skip if pollution already exists
    if exist "!POLLUTION_CHECK!" (
        echo [33mPollution already exists before test !COUNT!/!TOTAL![0m
        echo    Skipping: !TEST_FILE!
    ) else (
        echo [!COUNT!/!TOTAL!] Testing: !TEST_FILE!

        REM Run the test (suppress output)
        call npm test "!TEST_FILE!" >nul 2>&1

        REM Check if pollution appeared
        if exist "!POLLUTION_CHECK!" (
            echo.
            echo [32mFOUND POLLUTER![0m
            echo    Test: !TEST_FILE!
            echo    Created: !POLLUTION_CHECK!
            echo.
            echo Pollution details:
            dir "!POLLUTION_CHECK!"
            echo.
            echo To investigate:
            echo   npm test "!TEST_FILE!"    ^& REM Run just this test
            echo   type "!TEST_FILE!"        ^& REM Review test code
            del /f "!TMPFILE!" >nul 2>&1
            exit /b 1
        )
    )
)

echo.
echo [32mNo polluter found - all tests clean![0m
del /f "!TMPFILE!" >nul 2>&1
exit /b 0
