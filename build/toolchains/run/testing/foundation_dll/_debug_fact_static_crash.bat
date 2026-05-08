@echo off
setlocal
call "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvarsall.bat" x64 -vcvars_ver=14.42 >nul 2>nul

set FAMILY=convert-char
set REPO_ROOT=D:\agent\booming-il2cpp
set BUILD_DIR=%REPO_ROOT%\verification\foundation-dll\System.Private.CoreLib\%FAMILY%\native_test\fact-verify\build
set GEN_CPP=%REPO_ROOT%\verification\foundation-dll\System.Private.CoreLib\%FAMILY%\il2cpp_dist\genuine\ConvertCharNativeEntry\generated\native-aot.generated.cpp

set INC=-I"%REPO_ROOT%\src\native\common" -I"%REPO_ROOT%\src\native\common\chaos" -I"%REPO_ROOT%\contracts\native\v0" -I"%REPO_ROOT%\src\native\runtime-core" -I"%REPO_ROOT%\src\native\bootstrap" -I"%REPO_ROOT%\src\native\interpreter" -I"%REPO_ROOT%\third_party\bdwgc\include" -I"%REPO_ROOT%\third_party\fmt\include"
set DEFS=-DCHAOS_IL2CPP_CHECK -DCHAOS_IL2CPP_VERIFY_MODE -DGC_NOT_DLL -DCHAOS_RUNTIME_ABI_STATIC

rem Generate expected_checksums.h if it doesn't exist yet
if not exist "%BUILD_DIR%\expected_checksums.h" (
    echo // Auto-generated for debug > "%BUILD_DIR%\expected_checksums.h"
    echo constexpr int kExpectedCount = 18; >> "%BUILD_DIR%\expected_checksums.h"
    echo constexpr int kExpectedChecksums[kExpectedCount] = { >> "%BUILD_DIR%\expected_checksums.h"
    echo     -1, 42, 65, -1, -1, -1, 42, 42, 42, 42, 42, 42, -1, -1, -1, 42, 42, 42 >> "%BUILD_DIR%\expected_checksums.h"
    echo }; >> "%BUILD_DIR%\expected_checksums.h"
    echo Created expected_checksums.h
)

echo === 1. crash_test_standalone.exe (standalone, no lib) ===
cl /nologo /std:c++20 /EHsc /W3 /utf-8 /O2 /GS- /MD %INC% %DEFS% -I"%BUILD_DIR%" -Fe"%BUILD_DIR%\crash_test_sa.exe" "%BUILD_DIR%\crash_test.cpp" 2>&1
echo rc=%ERRORLEVEL%
if exist "%BUILD_DIR%\crash_test_sa.exe" (
    echo.
    echo Running crash_test_sa.exe...
    "%BUILD_DIR%\crash_test_sa.exe"
    echo exe rc=%ERRORLEVEL%
) else (
    echo SKIPPED - compile failed
)

echo.
echo === 2. isolate_crash.exe (standalone: just include generated code + stubs) ===
cl /nologo /std:c++20 /EHsc /W3 /utf-8 /O2 /GS- /MD %INC% %DEFS% -Fe"%BUILD_DIR%\isolate_crash_sa.exe" "%BUILD_DIR%\isolate_crash.cpp" 2>&1
echo rc=%ERRORLEVEL%
if exist "%BUILD_DIR%\isolate_crash_sa.exe" (
    echo.
    echo Running isolate_crash_sa.exe...
    "%BUILD_DIR%\isolate_crash_sa.exe"
    echo exe rc=%ERRORLEVEL%
) else (
    echo SKIPPED - compile failed
)

echo.
echo === 3. isolate_crash.exe WITH /GS (to confirm crash with /GS on) ===
cl /nologo /std:c++20 /EHsc /W3 /utf-8 /O2 /MD %INC% %DEFS% -Fe"%BUILD_DIR%\isolate_crash_gs_on.exe" "%BUILD_DIR%\isolate_crash.cpp" 2>&1
echo rc=%ERRORLEVEL%
if exist "%BUILD_DIR%\isolate_crash_gs_on.exe" (
    echo.
    echo Running isolate_crash_gs_on.exe...
    "%BUILD_DIR%\isolate_crash_gs_on.exe"
    echo exe rc=%ERRORLEVEL%
) else (
    echo SKIPPED - compile failed
)