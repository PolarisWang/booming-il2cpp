@echo off
call "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars64.bat" x64 >nul 2>nul
if %errorlevel% neq 0 (
    echo FAIL: vcvars64.bat
    exit /b 1
)

set BUILD_DIR=D:\agent\booming-il2cpp\verification\foundation-dll\System.Private.CoreLib\convert-char\native_test\fact-verify\build

set INCLUDE_FLAGS=-I"D:\agent\booming-il2cpp\src\native\common" -I"D:\agent\booming-il2cpp\src\native\common\chaos" -I"D:\agent\booming-il2cpp\contracts\native\v0" -I"D:\agent\booming-il2cpp\src\native\runtime-core" -I"D:\agent\booming-il2cpp\src\native\interpreter" -I"D:\agent\booming-il2cpp\src\native\bootstrap" -I"D:\agent\booming-il2cpp\third_party\bdwgc\include" -I"D:\agent\booming-il2cpp\third_party\fmt\include" -I"%BUILD_DIR%"

set DEFINES=-DCHAOS_IL2CPP_CHECK -DCHAOS_IL2CPP_VERIFY_MODE -DCHAOS_RUNTIME_ABI_STATIC -DGC_NOT_DLL
set COMPILE_FLAGS=/nologo /std:c++20 /c /EHsc /W3 /utf-8 /Od /MD

echo [INFO] Compiling minimal_debug.cpp ...
cl %COMPILE_FLAGS% %INCLUDE_FLAGS% %DEFINES% -Fo"%BUILD_DIR%\minimal_debug.obj" "%BUILD_DIR%\minimal_debug.cpp"
if %errorlevel% neq 0 (
    echo FAIL: compile
    exit /b 1
)

echo [INFO] Linking minimal_debug.exe ...
set CHAOS_LIB=D:\agent\booming-il2cpp\build\native\src\native\runtime-core\RelWithDebInfo\chaos_runtime_core.lib
link /nologo /out:"%BUILD_DIR%\minimal_debug.exe" "%BUILD_DIR%\minimal_debug.obj" "%CHAOS_LIB%" ole32.lib user32.lib
if %errorlevel% neq 0 (
    echo FAIL: link
    exit /b 1
)

echo [INFO] Running minimal_debug.exe ...
"%BUILD_DIR%\minimal_debug.exe"
echo EXIT_CODE=%ERRORLEVEL%