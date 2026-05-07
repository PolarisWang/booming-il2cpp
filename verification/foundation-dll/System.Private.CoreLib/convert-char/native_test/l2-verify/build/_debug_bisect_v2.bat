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
set LINKLIBS="%NATIVE_LIB%\runtime-core\Release\chaos_runtime_core.lib" "%NATIVE_LIB%\bootstrap\Release\chaos_bootstrap.lib" "%NATIVE_LIB%\interpreter\Release\chaos_interpreter.lib" "%NATIVE_LIB%\common\Release\chaos_common.lib" "%NATIVE_LIB%\support\Release\chaos_support.lib" "%NATIVE_LIB%\hot-update\Release\chaos_hot_update.lib" "%REPO%\build\native\fmt_build\Release\chaos_fmt.lib" "%BDWGC%\Release\chaos_bdwgc.lib" ole32.lib user32.lib

echo ========== PRE-COMPILE GENERATED CODE ==========
cl %FLAGS% %INCS% %DEFS% -Fo"%BUILD%\bg_gen.obj" "%GEN%"
if %ERRORLEVEL% NEQ 0 exit /b 1

echo.
echo ========== TEST 0: original native_verify_main.cpp ==========
cl %FLAGS% %INCS% %DEFS% -Fo"%BUILD%\bg0_v.obj" "%REPO%\src\native\benchmark-host\native_verify_main.cpp" || exit /b 1
link /nologo /out:"%BUILD%\bg0.exe" "%BUILD%\bg0_v.obj" "%BUILD%\bg_gen.obj" %LINKLIBS% 2>&1 || exit /b 1
"%BUILD%\bg0.exe"
echo RC0=%ERRORLEVEL%

echo.
echo ========== TEST 1: ChaosAbiManifestValidate stub ==========
rem The crash happens during static init. The main suspects are:
rem 1. String table init (InitializeFromAot) - no sync primitive needed, just stores pointers
rem 2. NameIndex init (RegisterModuleNameIndex) - uses g_name_index_registry which has std::mutex
rem 3. Module registration (RegisterModule) - uses g_free_list which is std::vector
rem 4. ABI manifest validation - ChaosAbiManifestValidate called from RegisterModule

rem Let's check: the crash is likely in ChaosAbiManifestValidate because s_abi_manifest uses
rem reinterpret_cast and the checksum validation loops through method entries.
rem OR it could be in the format library static init (fmt).

rem TEST: minimal main but with fmt library initialization
echo #include <cstdio> > "%BUILD%\bg1.cpp"
echo int main() { printf("hello\n"); return 0; } >> "%BUILD%\bg1.cpp"
cl %FLAGS% %INCS% %DEFS% -Fo"%BUILD%\bg1.obj" "%BUILD%\bg1.cpp" || exit /b 1
link /nologo /out:"%BUILD%\bg1.exe" "%BUILD%\bg1.obj" %LINKLIBS% 2>&1 || exit /b 1
"%BUILD%\bg1.exe"
echo RC1=%ERRORLEVEL%

echo.
echo ========== TEST 2: native_verify_main with explicit RegisterModule stub ==========
echo #include <cstdio> > "%BUILD%\bg2.cpp"
echo #include <csetjmp> >> "%BUILD%\bg2.cpp"
echo extern "C" int __chaos_assert_failures; >> "%BUILD%\bg2.cpp"
echo // Local SetExceptionFallback - not thread_local >> "%BUILD%\bg2.cpp"
echo static void (*g_local_fb)() = nullptr; >> "%BUILD%\bg2.cpp"
echo extern "C" void SetExceptionFallback(void(*fn)()) { g_local_fb = fn; } >> "%BUILD%\bg2.cpp"
echo extern "C" int RunNativeAot(int); >> "%BUILD%\bg2.cpp"
echo #include "expected_checksums.h" >> "%BUILD%\bg2.cpp"
echo static jmp_buf buf; static void fb(){longjmp(buf,1);} >> "%BUILD%\bg2.cpp"
echo int main(){ >> "%BUILD%\bg2.cpp"
echo __chaos_assert_failures=0; SetExceptionFallback(fb); int rf=0; >> "%BUILD%\bg2.cpp"
echo for(int i=0;i<kExpectedCount;i++){ >> "%BUILD%\bg2.cpp"
echo int a=0;bool t=0;if(setjmp(buf)==0){a=RunNativeAot(i);}else{t=1;} >> "%BUILD%\bg2.cpp"
echo if(kExpectedChecksums[i]==-1)continue; >> "%BUILD%\bg2.cpp"
echo if(t){printf("FAIL [%%d]: threw exp %%d\n",i,kExpectedChecksums[i]);rf++;continue;} >> "%BUILD%\bg2.cpp"
echo if(a!=kExpectedChecksums[i]){printf("FAIL [%%d]: exp %%d got %%d\n",i,kExpectedChecksums[i],a);rf++;} >> "%BUILD%\bg2.cpp"
echo } >> "%BUILD%\bg2.cpp"
echo printf("L2: %%d/%%d passed\n",kExpectedCount-rf,kExpectedCount); >> "%BUILD%\bg2.cpp"
echo return __chaos_assert_failures+rf; } >> "%BUILD%\bg2.cpp"

cl %FLAGS% %INCS% %DEFS% -Fo"%BUILD%\bg2_v.obj" "%BUILD%\bg2.cpp" || exit /b 1
link /nologo /out:"%BUILD%\bg2.exe" "%BUILD%\bg2_v.obj" "%BUILD%\bg_gen.obj" %LINKLIBS% 2>&1 || exit /b 1
"%BUILD%\bg2.exe"
echo RC2=%ERRORLEVEL%
