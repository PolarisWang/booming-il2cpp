@echo off
setlocal
call "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvarsall.bat" x64 -vcvars_ver=14.42 >nul 2>nul
if %ERRORLEVEL% neq 0 exit /b 1

set REPO=D:\agent\booming-il2cpp
set FAMILY=convert-char

set BUILD_DIR=%REPO%\verification\foundation-dll\System.Private.CoreLib\%FAMILY%\native_test\fact-verify\build

set INC=-I"%REPO%\src\native\common" -I"%REPO%\src\native\common\chaos" -I"%REPO%\contracts\native\v0" -I"%REPO%\src\native\runtime-core" -I"%REPO%\third_party\fmt\include"
set DEFS=-DCHAOS_IL2CPP_CHECK -DCHAOS_IL2CPP_VERIFY_MODE -DGC_NOT_DLL
set LIB=%REPO%\build\native\src\native\runtime-core\RelWithDebInfo\chaos_runtime_core.lib

echo === Compile isolate_crash_lib with /GS- ===
cl /nologo /std:c++20 /EHsc /W3 /utf-8 /O2 /GS- /MD %INC% %DEFS% -Fe"%BUILD_DIR%\iso_crash_lib_gs_off.exe" "%BUILD_DIR%\isolate_crash_lib.cpp" %LIB%
echo CL_RC=%ERRORLEVEL%

echo.
echo === Compile isolate_crash_lib with /GS (default) ===
cl /nologo /std:c++20 /EHsc /W3 /utf-8 /O2 /MD %INC% %DEFS% -Fe"%BUILD_DIR%\iso_crash_lib_gs_on.exe" "%BUILD_DIR%\isolate_crash_lib.cpp" %LIB%
echo CL_RC=%ERRORLEVEL%

if exist "%BUILD_DIR%\iso_crash_lib_gs_off.exe" (
    echo.
    echo ===== Running iso_crash_lib_gs_off.exe =====
    "%BUILD_DIR%\iso_crash_lib_gs_off.exe"
    echo EXE_RC=%ERRORLEVEL%
)

if exist "%BUILD_DIR%\iso_crash_lib_gs_on.exe" (
    echo.
    echo ===== Running iso_crash_lib_gs_on.exe =====
    "%BUILD_DIR%\iso_crash_lib_gs_on.exe"
    echo EXE_RC=%ERRORLEVEL%
)