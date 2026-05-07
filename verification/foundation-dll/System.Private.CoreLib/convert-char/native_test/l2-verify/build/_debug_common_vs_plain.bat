@echo off
call "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars64.bat" x64 -vcvars_ver=14.42 >nul 2>nul
if %ERRORLEVEL% NEQ 0 exit /b %ERRORLEVEL%

set REPO=D:\agent\booming-il2cpp
set BUILD=%REPO%\verification\foundation-dll\System.Private.CoreLib\convert-char\native_test\l2-verify\build

set INCS=-I"%REPO%\src\native\common" -I"%REPO%\src\native\common\chaos" -I"%REPO%\contracts\native\v0" -I"%REPO%\src\native\runtime-core" -I"%REPO%\src\native\interpreter" -I"%REPO%\src\native\bootstrap" -I"%REPO%\third_party\bdwgc\include" -I"%REPO%\third_party\fmt\include" -I"%BUILD%"
set DEFS=-DCHAOS_IL2CPP_CHECK -DCHAOS_IL2CPP_TRACE_ENABLED -DCHAOS_IL2CPP_VERIFY_MODE -DCHAOS_RUNTIME_ABI_STATIC -DGC_NOT_DLL
set FLAGS=/nologo /std:c++20 /c /EHsc /W3 /utf-8 /Od /MD
set NATIVE_LIB=%REPO%\build\native\src\native
set BDWGC=%REPO%\build\native\bdwgc_build
set LINKLIBS="%NATIVE_LIB%\runtime-core\Release\chaos_runtime_core.lib" "%NATIVE_LIB%\bootstrap\Release\chaos_bootstrap.lib" "%NATIVE_LIB%\interpreter\Release\chaos_interpreter.lib" "%NATIVE_LIB%\common\Release\chaos_common.lib" "%NATIVE_LIB%\support\Release\chaos_support.lib" "%NATIVE_LIB%\hot-update\Release\chaos_hot_update.lib" "%REPO%\build\native\fmt_build\Release\chaos_fmt.lib" "%BDWGC%\Release\chaos_bdwgc.lib" ole32.lib user32.lib

rem Gen code needs to be compiled fresh for each test (with same DEFS)
set GEN=%REPO%\verification\foundation-dll\System.Private.CoreLib\convert-char\il2cpp_dist\genuine\ConvertCharNativeEntry\generated\native-aot.generated.cpp

rem ===== TEST: include chaos/common.h + RunNativeAot, NOTHING ELSE =====
echo #include <chaos/common.h> > "%BUILD%\t_common.cpp"
echo extern "C" int RunNativeAot(int); >> "%BUILD%\t_common.cpp"
echo int main() { return RunNativeAot(0); } >> "%BUILD%\t_common.cpp"
cl %FLAGS% %INCS% %DEFS% -Fo"%BUILD%\t_common.obj" "%BUILD%\t_common.cpp" || exit /b 1
cl %FLAGS% %INCS% %DEFS% -Fo"%BUILD%\gen_t_common.obj" "%GEN%" || exit /b 1
link /nologo /out:"%BUILD%\t_common.exe" "%BUILD%\t_common.obj" "%BUILD%\gen_t_common.obj" %LINKLIBS% 2>&1 || echo LINK FAIL
"%BUILD%\t_common.exe"
echo RC_COMMON=%ERRORLEVEL%

echo.

rem ===== TEST: just <cstdio> + RunNativeAot =====
echo #include <cstdio> > "%BUILD%\t_plain.cpp"
echo extern "C" int RunNativeAot(int); >> "%BUILD%\t_plain.cpp"
echo int main() { int x=RunNativeAot(0); printf("%%d\n",x); return 0; } >> "%BUILD%\t_plain.cpp"
cl %FLAGS% %INCS% %DEFS% -Fo"%BUILD%\t_plain.obj" "%BUILD%\t_plain.cpp" || exit /b 1
link /nologo /out:"%BUILD%\t_plain.exe" "%BUILD%\t_plain.obj" "%BUILD%\gen_t_plain.obj" %LINKLIBS% 2>&1 || echo LINK FAIL
"%BUILD%\t_plain.exe"
echo RC_PLAIN=%ERRORLEVEL%
