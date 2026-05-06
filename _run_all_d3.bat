@echo off
setlocal enabledelayedexpansion
set REPO=D:\agent\booming-il2cpp
set VERIF=%REPO%\verification\foundation-dll\System.Private.CoreLib
set RESULTS=%REPO%\d3_all_results.txt

echo === D3 Hotupdate: Build + Run All Families === > %RESULTS%
echo Started: %DATE% %TIME% >> %RESULTS%
echo. >> %RESULTS%

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
    echo. >> %RESULTS%
    echo ===== %%f ===== >> %RESULTS%
    set BUILD_DIR=%VERIF%\%%f\il2cpp_dist\hotupdate\build
    if exist "!BUILD_DIR!\_build_d3_%%f.bat" (
        call "!BUILD_DIR!\_build_d3_%%f.bat"
        if !ERRORLEVEL! equ 0 (
            echo [BUILD OK] %%f >> %RESULTS%
            if exist "!BUILD_DIR!\d3_hotupdate_%%f.exe" (
                "!BUILD_DIR!\d3_hotupdate_%%f.exe" >> %RESULTS% 2>&1
                set EXITCODE=!ERRORLEVEL!
                echo [EXITCODE=!EXITCODE!] >> %RESULTS%
            ) else (
                echo [NO EXE] %%f >> %RESULTS%
            )
        ) else (
            echo [BUILD FAILED] %%f >> %RESULTS%
        )
    ) else (
        echo [NO BUILD SCRIPT] %%f >> %RESULTS%
    )
)

echo. >> %RESULTS%
echo Finished: %DATE% %TIME% >> %RESULTS%
echo Done.
