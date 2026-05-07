@echo off
call "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars64.bat" x64 -vcvars_ver=14.42 >nul 2>nul
if %ERRORLEVEL% NEQ 0 exit /b %ERRORLEVEL%

set REPO=D:\agent\booming-il2cpp
set BUILD=%REPO%\verification\foundation-dll\System.Private.CoreLib\convert-char\native_test\l2-verify\build
set GEN=%REPO%\verification\foundation-dll\System.Private.CoreLib\convert-char\il2cpp_dist\genuine\ConvertCharNativeEntry\generated\native-aot.generated.cpp

set INCS=-I"%REPO%\src\native\common" -I"%REPO%\src\native\common\chaos" -I"%REPO%\contracts\native\v0" -I"%REPO%\src\native\runtime-core" -I"%REPO%\src\native\interpreter" -I"%REPO%\src\native\bootstrap" -I"%REPO%\third_party\bdwgc\include" -I"%REPO%\third_party\fmt\include" -I"%BUILD%"
set DEFS=-DCHAOS_IL2CPP_CHECK -DCHAOS_IL2CPP_TRACE_ENABLED -DCHAOS_IL2CPP_VERIFY_MODE -DCHAOS_RUNTIME_ABI_STATIC -DGC_NOT_DLL
set FLAGS=/nologo /std:c++20 /c /EHsc /W3 /utf-8 /Od /MD
set NATIVE_LIB=%REPO%\build\native\src\native
set BDWGC=%REPO%\build\native\bdwgc_build

echo === FRESH COMPILE EVERYTHING ===

rem Fresh compile generated code
cl %FLAGS% %INCS% %DEFS% -Fo"%BUILD%\gen_fresh.obj" "%GEN%" || exit /b 1

rem printf-based verify main (not native_verify_main.cpp)
echo #include <cstdio> > "%BUILD%\fresh_main.cpp"
echo #include <csetjmp> >> "%BUILD%\fresh_main.cpp"
echo extern "C" int __chaos_assert_failures; >> "%BUILD%\fresh_main.cpp"
echo extern "C" void SetExceptionFallback(void(*fn)()); >> "%BUILD%\fresh_main.cpp"
echo extern "C" int RunNativeAot(int); >> "%BUILD%\fresh_main.cpp"
echo #include "expected_checksums.h" >> "%BUILD%\fresh_main.cpp"
echo static jmp_buf buf; static void fb(){longjmp(buf,1);} >> "%BUILD%\fresh_main.cpp"
echo int main(){ >> "%BUILD%\fresh_main.cpp"
echo __chaos_assert_failures=0; SetExceptionFallback(fb); int rf=0; >> "%BUILD%\fresh_main.cpp"
echo for(int i=0;i<kExpectedCount;i++){ >> "%BUILD%\fresh_main.cpp"
echo int a=0;bool t=false;if(setjmp(buf)==0){a=RunNativeAot(i);}else{t=true;} >> "%BUILD%\fresh_main.cpp"
echo if(kExpectedChecksums[i]==-1)continue; >> "%BUILD%\fresh_main.cpp"
echo if(t){printf("FAIL [%%d]: threw, exp %%d\n",i,kExpectedChecksums[i]);rf++;continue;} >> "%BUILD%\fresh_main.cpp"
echo if(a!=kExpectedChecksums[i]){printf("FAIL [%%d]: exp %%d got %%d\n",i,kExpectedChecksums[i],a);rf++;} >> "%BUILD%\fresh_main.cpp"
echo } >> "%BUILD%\fresh_main.cpp"
echo SetExceptionFallback(nullptr); >> "%BUILD%\fresh_main.cpp"
echo printf("L2: %%d/%%d passed\n",kExpectedCount-rf,kExpectedCount); >> "%BUILD%\fresh_main.cpp"
echo return __chaos_assert_failures+rf; >> "%BUILD%\fresh_main.cpp"
echo } >> "%BUILD%\fresh_main.cpp"

cl %FLAGS% %INCS% %DEFS% -Fo"%BUILD%\fresh_main.obj" "%BUILD%\fresh_main.cpp" || exit /b 1

set LINKLIBS="%NATIVE_LIB%\runtime-core\Release\chaos_runtime_core.lib" "%NATIVE_LIB%\bootstrap\Release\chaos_bootstrap.lib" "%NATIVE_LIB%\interpreter\Release\chaos_interpreter.lib" "%NATIVE_LIB%\common\Release\chaos_common.lib" "%NATIVE_LIB%\support\Release\chaos_support.lib" "%NATIVE_LIB%\hot-update\Release\chaos_hot_update.lib" "%REPO%\build\native\fmt_build\Release\chaos_fmt.lib" "%BDWGC%\Release\chaos_bdwgc.lib" ole32.lib user32.lib

link /nologo /out:"%BUILD%\fresh_verify.exe" "%BUILD%\fresh_main.obj" "%BUILD%\gen_fresh.obj" %LINKLIBS% 2>&1 || exit /b 1
echo === RUN ===
"%BUILD%\fresh_verify.exe"
echo RC=%ERRORLEVEL%

echo.

echo === COMPILE native_verify_main FRESH and test ===
cl %FLAGS% %INCS% %DEFS% -Fo"%BUILD%\native_verify_main_fresh.obj" "%REPO%\src\native\benchmark-host\native_verify_main.cpp" || exit /b 1
link /nologo /out:"%BUILD%\fresh_orig_verify.exe" "%BUILD%\native_verify_main_fresh.obj" "%BUILD%\gen_fresh.obj" %LINKLIBS% 2>&1 || exit /b 1
echo === RUN (original native_verify_main) ===
"%BUILD%\fresh_orig_verify.exe"
echo RC_ORIG=%ERRORLEVEL%
