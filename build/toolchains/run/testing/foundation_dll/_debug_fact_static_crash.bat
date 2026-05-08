@echo off
setlocal
call "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvarsall.bat" x64 -vcvars_ver=14.42 >nul 2>nul
if %ERRORLEVEL% neq 0 exit /b 1

set REPO=D:\agent\booming-il2cpp
set FAMILY=convert-char
set BUILD_DIR=%REPO%\verification\foundation-dll\System.Private.CoreLib\%FAMILY%\native_test\fact-verify\build

set INC=-I"%REPO%\src\native\common" -I"%REPO%\src\native\common\chaos" -I"%REPO%\contracts\native\v0" -I"%REPO%\src\native\runtime-core" -I"%REPO%\third_party\fmt\include" -I"%REPO%\third_party\bdwgc\include"
set DEFS=-DCHAOS_IL2CPP_CHECK -DCHAOS_IL2CPP_VERIFY_MODE -DGC_NOT_DLL

set LIBDIR=%REPO%\build\native
set LIBS=%LIBDIR%\src\native\runtime-core\RelWithDebInfo\chaos_runtime_core.lib ^
    %LIBDIR%\src\native\interpreter\RelWithDebInfo\chaos_interpreter.lib ^
    %LIBDIR%\src\native\bootstrap\RelWithDebInfo\chaos_bootstrap.lib ^
    %LIBDIR%\src\native\common\RelWithDebInfo\chaos_common.lib ^
    %LIBDIR%\src\native\support\RelWithDebInfo\chaos_support.lib ^
    %LIBDIR%\src\native\hot-update\RelWithDebInfo\chaos_hot_update.lib ^
    %LIBDIR%\..\..\fmt_build\RelWithDebInfo\chaos_fmt.lib ^
    %LIBDIR%\..\..\bdwgc_build\RelWithDebInfo\chaos_bdwgc.lib

set GEN_CPP=%REPO%\verification\foundation-dll\System.Private.CoreLib\%FAMILY%\il2cpp_dist\genuine\ConvertCharNativeEntry\generated\native-aot.generated.cpp

echo === Step 1: Compile bench_main.cpp ===
cl /nologo /std:c++20 /c /EHsc /W3 /utf-8 /O2 /MD %INC% %DEFS% -Fo"%BUILD_DIR%\bench_main.obj" "%REPO%\verification\foundation-dll\System.Private.CoreLib\%FAMILY%\native_test\benchmark\build\bench_main.cpp"
echo CL_RC=%ERRORLEVEL%

echo.
echo === Step 2: Compile generated C++ ===
cl /nologo /std:c++20 /c /EHsc /W3 /utf-8 /O2 /MD %INC% %DEFS% -Fo"%BUILD_DIR%\gen_new.obj" "%GEN_CPP%"
echo CL_RC=%ERRORLEVEL%

if exist "%BUILD_DIR%\bench_main.obj" if exist "%BUILD_DIR%\gen_new.obj" (
    echo.
    echo === Step 3: Link with FULL set of new libs ===
    link /nologo /out:"%BUILD_DIR%\test_full.exe" "%BUILD_DIR%\bench_main.obj" "%BUILD_DIR%\gen_new.obj" %LIBS% ole32.lib user32.lib
    echo LINK_RC=%ERRORLEVEL%
    if exist "%BUILD_DIR%\test_full.exe" (
        echo.
        echo ===== Running test_full.exe =====
        "%BUILD_DIR%\test_full.exe"
        echo EXE_RC=%ERRORLEVEL%
    )
)