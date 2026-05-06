@echo off
setlocal enabledelayedexpansion
set REPO=D:\agent\booming-il2cpp
set VERIF=%REPO%\verification\foundation-dll\System.Private.CoreLib
set SUMMARY=%REPO%\d3_summary.txt

echo D3 Hotupdate Test Summary > %SUMMARY%
echo ========================= >> %SUMMARY%
echo. >> %SUMMARY%
echo Family ^| Methods ^| Passed ^| Failed ^| BaselineSkipped ^| SemanticMismatch ^| ExitCode >> %SUMMARY%
echo ------^|---------^|-------^|-------^|----------------^|-----------------^|-------- >> %SUMMARY%

for %%f in (
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
    runtime-compiler-services
    span-memory-buffers
    type-runtime-handles
    reflection-type
    reflection-assembly
    reflection-generics
    reflection-member-complete
    reflection-parameters
    reflection-module
    reflection-binding
    reflection-activation
) do (
    set BUILD_DIR=%VERIF%\%%f\il2cpp_dist\hotupdate\build
    if exist "!BUILD_DIR!\_build_d3_%%f.bat" (
        call "!BUILD_DIR!\_build_d3_%%f.bat"
        if !ERRORLEVEL! equ 0 (
            if exist "!BUILD_DIR!\d3_hotupdate_%%f.exe" (
                "!BUILD_DIR!\d3_hotupdate_%%f.exe" > "!BUILD_DIR!\d3_result.json" 2>&1
                set EX=!ERRORLEVEL!
            ) else ( set EX=-2 )
        ) else ( set EX=-1 )
    ) else ( set EX=-3 )

    echo %%f: EXITCODE=!EX!
)

echo. >> %SUMMARY%
echo Done. >> %SUMMARY%
