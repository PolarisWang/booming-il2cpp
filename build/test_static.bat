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

REM Write minimal host source
echo #include ^<cstdio^> > "%BUILD_DIR%\test_static.cpp"
echo int main() { printf("main reached\n"); return 0; } >> "%BUILD_DIR%\test_static.cpp"
echo #include "%GENERATED%" >> "%BUILD_DIR%\test_static.cpp"

REM Compile
cl /nologo /std:c++20 /c /EHsc /W3 /utf-8 /Od /MD %INCLUDES% %DEFINES% -Fo"%BUILD_DIR%\test_static.obj" "%BUILD_DIR%\test_static.cpp"
if errorlevel 1 (
    echo COMPILE FAILED
    exit /b 1
)
echo COMPILE OK

REM Link
set CHAOS_LIB=D:\agent\booming-il2cpp\build\native\src\native\runtime-core\RelWithDebInfo\chaos_runtime_core.lib
link /nologo /out:"%BUILD_DIR%\test_static.exe" "%BUILD_DIR%\test_static.obj" "%CHAOS_LIB%" ole32.lib user32.lib
if errorlevel 1 (
    echo LINK FAILED
    exit /b 1
)
echo LINK OK

REM Run
"%BUILD_DIR%\test_static.exe"
echo EXIT CODE: %ERRORLEVEL%
