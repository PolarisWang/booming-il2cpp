@echo off
call "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvarsall.bat" x64 -vcvars_ver=14.42 >nul 2>&1
pushd "%~dp0"

set INCLUDE_FLAGS=-I"D:\agent\booming-il2cpp\src\native\common" -I"D:\agent\booming-il2cpp\src\native\common\chaos" -I"D:\agent\booming-il2cpp\contracts\native\v0" -I"D:\agent\booming-il2cpp\src\native\runtime-core" -I"D:\agent\booming-il2cpp\src\native\bootstrap" -I"D:\agent\booming-il2cpp\third_party\fmt\include" -I"%CD%"
set DEFINES=-DCHAOS_IL2CPP_CHECK -DCHAOS_IL2CPP_TRACE_ENABLED -DGC_NOT_DLL -DCHAOS_IL2CPP_VERIFY_MODE

echo === COMPILE L3 HOST ===
cl.exe /nologo /std:c++17 /c /EHsc /W3 /utf-8 /O2 /MD %INCLUDE_FLAGS% %DEFINES% -Fo"l3_verify_main.obj" "D:\agent\booming-il2cpp\src\native\benchmark-host\l3_verify_main.cpp"

set GENERATED_CPP=D:\agent\booming-il2cpp\verification\foundation-dll\System.Private.CoreLib\convert-char\il2cpp_dist\genuine\generated\native-aot.generated.cpp
cl.exe /nologo /std:c++17 /c /EHsc /W3 /utf-8 /O2 /MD %INCLUDE_FLAGS% %DEFINES% -Fo"native-aot.generated.obj" "%GENERATED_CPP%"
echo COMPILE EXIT=%ERRORLEVEL%
if errorlevel 1 goto end

echo === LINK ===
set MAIN_NATIVE_DIR=D:\agent\booming-il2cpp\build\native\src\native
set MAIN_BDWGC_DIR=D:\agent\booming-il2cpp\build\native\bdwgc_build

link.exe /nologo /out:l3_verify_convert-char.exe ^
    l3_verify_main.obj native-aot.generated.obj ^
    D:\agent\booming-il2cpp\build\native-runtime\Release\chaos_runtime_core.lib ^
    "%MAIN_NATIVE_DIR%\bootstrap\Release\chaos_bootstrap.lib" ^
    "%MAIN_NATIVE_DIR%\interpreter\Release\chaos_interpreter.lib" ^
    "%MAIN_NATIVE_DIR%\common\Release\chaos_common.lib" ^
    "%MAIN_NATIVE_DIR%\support\Release\chaos_support.lib" ^
    "%MAIN_NATIVE_DIR%\hot-update\Release\chaos_hot_update.lib" ^
    "%MAIN_BDWGC_DIR%\Release\chaos_bdwgc.lib" ^
    ole32.lib user32.lib
echo LINK EXIT=%ERRORLEVEL%

if exist l3_verify_convert-char.exe (
    echo === RUN L3 ===
    l3_verify_convert-char.exe
    echo RUN EXIT=%ERRORLEVEL%
)
:end
popd
