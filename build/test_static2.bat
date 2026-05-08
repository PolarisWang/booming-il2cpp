@echo off
set VCVARS="C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars64.bat"
if not exist %VCVARS% (
    echo vcvars not found
    exit /b 1
)
call %VCVARS% x64 >nul 2>nul
if errorlevel 1 exit /b 1

set BUILD_DIR=D:\agent\booming-il2cpp\verification\foundation-dll\System.Private.CoreLib\convert-char\native_test\fact-verify\build
set GENERATED=D:\agent\booming-il2cpp\verification\foundation-dll\System.Private.CoreLib\convert-char\il2cpp_dist\genuine\ConvertCharNativeEntry\generated\native-aot.generated.cpp

set INCLUDES=-I"D:\agent\booming-il2cpp\src\native\common" -I"D:\agent\booming-il2cpp\src\native\common\chaos" -I"D:\agent\booming-il2cpp\contracts\native\v0" -I"D:\agent\booming-il2cpp\src\native\runtime-core" -I"D:\agent\booming-il2cpp\src\native\interpreter" -I"D:\agent\booming-il2cpp\src\native\bootstrap" -I"D:\agent\booming-il2cpp\third_party\bdwgc\include" -I"D:\agent\booming-il2cpp\third_party\fmt\include" -I"%BUILD_DIR%"

set DEFINES=-DCHAOS_IL2CPP_CHECK -DCHAOS_IL2CPP_TRACE_ENABLED -DCHAOS_IL2CPP_VERIFY_MODE -DCHAOS_RUNTIME_ABI_STATIC -DGC_NOT_DLL

REM Write minimal host that just prints (no static init deps)
echo #include ^<cstdio^> > "%BUILD_DIR%\minimal_host2.cpp"
echo int main() { printf("main reached\n"); fflush(stdout); return 42; } >> "%BUILD_DIR%\minimal_host2.cpp"
echo #include "%GENERATED%" >> "%BUILD_DIR%\minimal_host2.cpp"

REM Compile
cl /nologo /std:c++20 /c /EHsc /W3 /utf-8 /Od /MD %INCLUDES% %DEFINES% -Fo"%BUILD_DIR%\minimal_host2.obj" "%BUILD_DIR%\minimal_host2.cpp"
if errorlevel 1 (
    echo COMPILE FAILED
    exit /b 1
)
echo COMPILE OK

REM Link with ALL libs (same as fact_verifier does)
set CHAOS_LIB=D:\agent\booming-il2cpp\build\native\src\native\runtime-core\RelWithDebInfo\chaos_runtime_core.lib
set BOOTSTRAP_LIB=D:\agent\booming-il2cpp\build\native\src\native\bootstrap\RelWithDebInfo\chaos_bootstrap.lib
set INTERP_LIB=D:\agent\booming-il2cpp\build\native\src\native\interpreter\RelWithDebInfo\chaos_interpreter.lib
set COMMON_LIB=D:\agent\booming-il2cpp\build\native\src\native\common\RelWithDebInfo\chaos_common.lib
set SUPPORT_LIB=D:\agent\booming-il2cpp\build\native\src\native\support\RelWithDebInfo\chaos_support.lib
set HOTUPDATE_LIB=D:\agent\booming-il2cpp\build\native\src\native\hot-update\RelWithDebInfo\chaos_hot_update.lib
set FMT_LIB=D:\agent\booming-il2cpp\build\native\fmt_build\RelWithDebInfo\chaos_fmt.lib
set BDWGC_LIB=D:\agent\booming-il2cpp\build\native\bdwgc_build\RelWithDebInfo\chaos_bdwgc.lib

link /nologo /out:"%BUILD_DIR%\minimal_host2.exe" "%BUILD_DIR%\minimal_host2.obj" "%CHAOS_LIB%" "%BOOTSTRAP_LIB%" "%INTERP_LIB%" "%COMMON_LIB%" "%SUPPORT_LIB%" "%HOTUPDATE_LIB%" "%FMT_LIB%" "%BDWGC_LIB%" ole32.lib user32.lib
if errorlevel 1 (
    echo LINK FAILED
    exit /b 1
)
echo LINK OK

REM Run
"%BUILD_DIR%\minimal_host2.exe"
echo EXIT CODE: %ERRORLEVEL%
