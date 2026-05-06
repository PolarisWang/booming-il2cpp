@echo off
call "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvarsall.bat" x64 -vcvars_ver=14.42 >nul 2>&1
pushd "%~dp0"

set INCLUDE_FLAGS=-I"D:\agent\booming-il2cpp\src\native\common" -I"D:\agent\booming-il2cpp\src\native\common\chaos" -I"D:\agent\booming-il2cpp\contracts\native\v0" -I"D:\agent\booming-il2cpp\src\native\runtime-core" -I"D:\agent\booming-il2cpp\third_party\fmt\include" -I"%CD%"
REM CHAOS_IL2CPP_VERIFY_MODE disables the managed pointer tag scheme
REM (chaos_managed_pointer_local_slot_tag = 0) to avoid bit-0 collision
REM with odd integer checksums like 65 ('A'). The macro guard is emitted
REM by codegen in NativeAotLoweringPlanner.ObjectModelEmission.cs.
set DEFINES=-DCHAOS_IL2CPP_CHECK -DCHAOS_IL2CPP_TRACE_ENABLED -DCHAOS_IL2CPP_VERIFY_MODE

echo === COMPILE ===
cl.exe /nologo /std:c++17 /c /EHsc /W3 /utf-8 /O2 /MD %INCLUDE_FLAGS% %DEFINES% -Fo"native_verify_main.obj" "D:\agent\booming-il2cpp\src\native\benchmark-host\native_verify_main.cpp"

set GENERATED_CPP=D:\agent\booming-il2cpp\verification\foundation-dll\System.Private.CoreLib\convert-char\il2cpp_dist\genuine\generated\native-aot.generated.cpp
cl.exe /nologo /std:c++17 /c /EHsc /W3 /utf-8 /O2 /MD %INCLUDE_FLAGS% %DEFINES% -Fo"native-aot.generated.obj" "%GENERATED_CPP%"
echo COMPILE EXIT=%ERRORLEVEL%

if errorlevel 1 goto end

echo === LINK ===
set MAIN_NATIVE_DIR=D:\agent\booming-il2cpp\build\native\src\native
set MAIN_BDWGC_DIR=D:\agent\booming-il2cpp\build\native\bdwgc_build

link.exe /nologo /out:verify_convert-char.exe ^
    native_verify_main.obj native-aot.generated.obj ^
    D:\agent\booming-il2cpp\build\native-runtime\Release\chaos_runtime_core.lib ^
    "%MAIN_NATIVE_DIR%\bootstrap\Release\chaos_bootstrap.lib" ^
    "%MAIN_NATIVE_DIR%\interpreter\Release\chaos_interpreter.lib" ^
    "%MAIN_NATIVE_DIR%\common\Release\chaos_common.lib" ^
    "%MAIN_NATIVE_DIR%\support\Release\chaos_support.lib" ^
    "%MAIN_NATIVE_DIR%\hot-update\Release\chaos_hot_update.lib" ^
    "%MAIN_BDWGC_DIR%\Release\chaos_bdwgc.lib" ^
    ole32.lib user32.lib
echo LINK EXIT=%ERRORLEVEL%

echo === RUN ===
if exist verify_convert-char.exe (
    verify_convert-char.exe
    echo RUN EXIT=%ERRORLEVEL%
)
:end
popd
