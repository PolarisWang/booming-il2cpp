@echo off
call "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars64.bat" x64 -vcvars_ver=14.42 >nul 2>nul
if %ERRORLEVEL% NEQ 0 exit /b %ERRORLEVEL%

set REPO=D:\agent\booming-il2cpp
set BUILD=%REPO%\verification\foundation-dll\System.Private.CoreLib\convert-char\native_test\l2-verify\build
set GEN=%REPO%\verification\foundation-dll\System.Private.CoreLib\convert-char\il2cpp_dist\genuine\ConvertCharNativeEntry\generated\native-aot.generated.cpp

set INCS=-I"%REPO%\src\native\common" -I"%REPO%\src\native\common\chaos" -I"%REPO%\contracts\native\v0" -I"%REPO%\src\native\runtime-core" -I"%REPO%\src\native\interpreter" -I"%REPO%\src\native\bootstrap" -I"%REPO%\third_party\bdwgc\include" -I"%REPO%\third_party\fmt\include" -I"%BUILD%"
set DEFS=-DCHAOS_IL2CPP_CHECK -DCHAOS_IL2CPP_TRACE_ENABLED -DCHAOS_IL2CPP_VERIFY_MODE -DCHAOS_RUNTIME_ABI_STATIC -DGC_NOT_DLL
set FLAGS=/nologo /std:c++20 /c /EHsc /W3 /utf-8 /Od /MD

echo === Compile generated (separate) ===
cl %FLAGS% %INCS% %DEFS% -Fo"%BUILD%\debug_gen_separate.obj" "%GEN%"
if %ERRORLEVEL% NEQ 0 exit /b %ERRORLEVEL%

echo === Compile minimal main (separate) ===
cl %FLAGS% %INCS% %DEFS% -Fo"%BUILD%\debug_minimal_main.obj" "%BUILD%\debug_minimal_main.cpp"
if %ERRORLEVEL% NEQ 0 exit /b %ERRORLEVEL%

echo === LINK separate .objs ===
set NATIVE_LIB=%REPO%\build\native\src\native
set BDWGC=%REPO%\build\native\bdwgc_build

link /nologo /out:"%BUILD%\debug_separate.exe" ^
  "%BUILD%\debug_minimal_main.obj" ^
  "%BUILD%\debug_gen_separate.obj" ^
  "%NATIVE_LIB%\runtime-core\Release\chaos_runtime_core.lib" ^
  "%NATIVE_LIB%\bootstrap\Release\chaos_bootstrap.lib" ^
  "%NATIVE_LIB%\interpreter\Release\chaos_interpreter.lib" ^
  "%NATIVE_LIB%\common\Release\chaos_common.lib" ^
  "%NATIVE_LIB%\support\Release\chaos_support.lib" ^
  "%NATIVE_LIB%\hot-update\Release\chaos_hot_update.lib" ^
  "%REPO%\build\native\fmt_build\Release\chaos_fmt.lib" ^
  "%BDWGC%\Release\chaos_bdwgc.lib" ^
  ole32.lib user32.lib
if %ERRORLEVEL% NEQ 0 exit /b %ERRORLEVEL%

echo === RUN separate ===
"%BUILD%\debug_separate.exe"
echo EXIT_CODE=%ERRORLEVEL%
