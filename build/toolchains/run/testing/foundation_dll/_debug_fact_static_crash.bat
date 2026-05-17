@echo off
call "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvarsall.bat" x64 -vcvars_ver=14.42 >nul 2>nul
if %ERRORLEVEL% neq 0 exit /b 1

set REPO=D:\agent\booming-il2cpp
set FAMILY=convert-char
set BUILD_DIR=%REPO%\verification\foundation-dll\System.Private.CoreLib\%FAMILY%\native_test\fact-verify\build

set INC=-I"%REPO%\src\native\common" -I"%REPO%\src\native\common\chaos" -I"%REPO%\contracts\native\v0" -I"%REPO%\src\native\runtime-core" -I"%REPO%\src\native\bootstrap" -I"%REPO%\src\native\interpreter" -I"%REPO%\third_party\fmt\include" -I"%REPO%\src\native\verification-host"
set DEFS=-DCHAOS_IL2CPP_CHECK -DCHAOS_IL2CPP_VERIFY_MODE -DGC_NOT_DLL -DCHAOS_RUNTIME_ABI_STATIC

set L=%REPO%\build\native
set LIBS=%L%\src\native\runtime-core\RelWithDebInfo\chaos_runtime_core.lib ^
    %L%\src\native\interpreter\RelWithDebInfo\chaos_interpreter.lib ^
    %L%\src\native\bootstrap\RelWithDebInfo\chaos_bootstrap.lib ^
    %L%\src\native\common\RelWithDebInfo\chaos_common.lib ^
    %L%\src\native\support\RelWithDebInfo\chaos_support.lib ^
    %L%\src\native\hot-update\RelWithDebInfo\chaos_hot_update.lib ^
    %L%\fmt_build\RelWithDebInfo\chaos_fmt.lib

set GEN_CPP=%REPO%\verification\foundation-dll\System.Private.CoreLib\%FAMILY%\il2cpp_dist\genuine\ConvertCharNativeEntry\generated\native-aot.generated.cpp

echo === Compile+Link fact_static_test.exe ===
cl /nologo /std:c++20 /EHsc /W3 /utf-8 /O2 /MD %INC% %DEFS% -I"%BUILD_DIR%" -Fe"%BUILD_DIR%\fact_static_test.exe" "%BUILD_DIR%\fact_verify_combined.cpp" %LIBS% ole32.lib user32.lib
echo CL_RC=%ERRORLEVEL%

if exist "%BUILD_DIR%\fact_static_test.exe" (
    echo.
    echo ===== Running fact_static_test.exe =====
    "%BUILD_DIR%\fact_static_test.exe"
    echo EXE_RC=%ERRORLEVEL%
)