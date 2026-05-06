@echo off
setlocal enabledelayedexpansion

set REPO_ROOT=D:\agent\booming-il2cpp
set OUTPUT_DIR=%REPO_ROOT%\verification\foundation-dll\hotupdate-results
mkdir "%OUTPUT_DIR%" 2>nul

echo ============================================================
echo Running all foundation-dll D3 hotupdate tests
echo ============================================================

set RUN_FAILED=0

for %%F in (
    array-indexing-copy
    buffer-memory
    collections-generic-core
    convert-char
    enum-parsing
    generic-nullable-value
    guid-random-hashcode
    math-numerics
    object-equality-identity
    primitive-numeric-conversions
    string-char-text-core
    time-date-time-timespan
    threading-monitor-interlocked
    attributes-custom-metadata
    boxing-unboxing-casts
    delegate-core-invocation
    exception-throw-diagnostics
    globalization-culture
    io-streams-basics
    reflection-member-basics
    reflection-member-complete
    reflection-activation
    reflection-assembly
    reflection-binding
    reflection-generics
    reflection-module
    reflection-parameters
    reflection-type
    runtime-compiler-services
    span-memory-buffers
    threading-tasks-primitives
    type-runtime-handles
) do (
    set "EXE=%REPO_ROOT%\verification\foundation-dll\System.Private.CoreLib\%%F\il2cpp_dist\hotupdate\build\d3_hotupdate_%%F.exe"
    set "OUT=%OUTPUT_DIR%\%%F.json"
    echo [RUN] %%F ...
    if exist "!EXE!" (
        "!EXE!" > "!OUT!" 2>&1
        if !ERRORLEVEL! equ 0 (
            echo [OK] %%F passed
        ) else (
            echo [FAIL] %%F failed (see !OUT!)
            set RUN_FAILED=1
        )
    ) else (
        echo [SKIP] %%F exe not found
    )
)

echo.
echo ============================================================
if %RUN_FAILED% equ 0 (
    echo All families passed
) else (
    echo Some families FAILED
)
echo Results in: %OUTPUT_DIR%
echo ============================================================
exit /b %RUN_FAILED%
