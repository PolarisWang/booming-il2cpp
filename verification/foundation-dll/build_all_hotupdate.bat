@echo off
setlocal enabledelayedexpansion

set REPO_ROOT=D:\agent\booming-il2cpp

echo ============================================================
echo Building all foundation-dll D3 hotupdate tests
echo ============================================================

set BUILD_FAILED=0

echo.
echo NOTE: This batch file uses "call" chaining which may have issues
echo when run from bash via "cmd //c". If you encounter issues, run
echo the Python runner instead:
echo   python build/toolchains/run/testing/foundation_dll/d3_hotupdate_runner.py --all
echo.

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
    echo.
    echo [BUILD] %%F ...
    pushd "%REPO_ROOT%\verification\foundation-dll\System.Private.CoreLib\%%F\il2cpp_dist\hotupdate\build"
    call "_build_d3_%%F.bat"
    popd
    if !ERRORLEVEL! equ 0 (
        echo [OK] %%F built successfully
    ) else (
        echo [FAIL] %%F build failed
        set BUILD_FAILED=1
    )
)

echo.
echo ============================================================
if %BUILD_FAILED% equ 0 (
    echo All families built successfully
) else (
    echo Some families FAILED to build
)
echo ============================================================
exit /b %BUILD_FAILED%
