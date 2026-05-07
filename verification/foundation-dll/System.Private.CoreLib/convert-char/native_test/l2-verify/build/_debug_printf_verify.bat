@echo off
call "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars64.bat" x64 -vcvars_ver=14.42 >nul 2>nul
if %ERRORLEVEL% NEQ 0 exit /b %ERRORLEVEL%

set REPO=D:\agent\booming-il2cpp
set BUILD=%REPO%\verification\foundation-dll\System.Private.CoreLib\convert-char\native_test\l2-verify\build
set NATIVE_LIB=%REPO%\build\native\src\native
set BDWGC=%REPO%\build\native\bdwgc_build

set INCS=-I"%REPO%\src\native\common" -I"%REPO%\src\native\common\chaos" -I"%REPO%\contracts\native\v0" -I"%REPO%\src\native\runtime-core" -I"%REPO%\src\native\interpreter" -I"%REPO%\src\native\bootstrap" -I"%REPO%\third_party\bdwgc\include" -I"%REPO%\third_party\fmt\include" -I"%BUILD%"
set DEFS=-DCHAOS_IL2CPP_CHECK -DCHAOS_IL2CPP_TRACE_ENABLED -DCHAOS_IL2CPP_VERIFY_MODE -DCHAOS_RUNTIME_ABI_STATIC -DGC_NOT_DLL
set FLAGS=/nologo /std:c++20 /c /EHsc /W3 /utf-8 /Od /MD

rem Write verify_main_printf.cpp (no CHAOS_IL2CPP_LOG, just printf)
echo #include <cstdlib> > "%BUILD%\verify_main_printf.cpp"
echo #include <cstdio> >> "%BUILD%\verify_main_printf.cpp"
echo #include <csetjmp> >> "%BUILD%\verify_main_printf.cpp"
echo extern "C" int __chaos_assert_failures; >> "%BUILD%\verify_main_printf.cpp"
echo extern "C" void SetExceptionFallback(void (*fn)()); >> "%BUILD%\verify_main_printf.cpp"
echo extern "C" int RunNativeAot(int entryIndex); >> "%BUILD%\verify_main_printf.cpp"
echo #include "expected_checksums.h" >> "%BUILD%\verify_main_printf.cpp"
echo static jmp_buf s_verify_buf; >> "%BUILD%\verify_main_printf.cpp"
echo static void exception_fallback() { longjmp(s_verify_buf, 1); } >> "%BUILD%\verify_main_printf.cpp"
echo int main() { >> "%BUILD%\verify_main_printf.cpp"
echo     __chaos_assert_failures = 0; >> "%BUILD%\verify_main_printf.cpp"
echo     SetExceptionFallback(^&exception_fallback); >> "%BUILD%\verify_main_printf.cpp"
echo     int return_value_failures = 0; >> "%BUILD%\verify_main_printf.cpp"
echo     for (int i = 0; i ^< kExpectedCount; i++) { >> "%BUILD%\verify_main_printf.cpp"
echo         int actual = 0; bool threw = false; >> "%BUILD%\verify_main_printf.cpp"
echo         if (setjmp(s_verify_buf) == 0) { >> "%BUILD%\verify_main_printf.cpp"
echo             actual = RunNativeAot(i); >> "%BUILD%\verify_main_printf.cpp"
echo         } else { threw = true; } >> "%BUILD%\verify_main_printf.cpp"
echo         if (kExpectedChecksums[i] == -1) continue; >> "%BUILD%\verify_main_printf.cpp"
echo         if (threw) { printf("FAIL [%%d]: threw, expected %%d\n", i, kExpectedChecksums[i]); return_value_failures++; continue; } >> "%BUILD%\verify_main_printf.cpp"
echo         if (actual != kExpectedChecksums[i]) { printf("FAIL [%%d]: expected %%d, got %%d\n", i, kExpectedChecksums[i], actual); return_value_failures++; } >> "%BUILD%\verify_main_printf.cpp"
echo     } >> "%BUILD%\verify_main_printf.cpp"
echo     SetExceptionFallback(nullptr); >> "%BUILD%\verify_main_printf.cpp"
echo     int total = __chaos_assert_failures + return_value_failures; >> "%BUILD%\verify_main_printf.cpp"
echo     int passed = kExpectedCount - return_value_failures; >> "%BUILD%\verify_main_printf.cpp"
echo     printf("L2: %%d/%%d passed (assert=%%d, return=%%d)\n", passed, kExpectedCount, __chaos_assert_failures, return_value_failures); >> "%BUILD%\verify_main_printf.cpp"
echo     return total; >> "%BUILD%\verify_main_printf.cpp"
echo } >> "%BUILD%\verify_main_printf.cpp"

cl %FLAGS% %INCS% %DEFS% -Fo"%BUILD%\verify_main_printf.obj" "%BUILD%\verify_main_printf.cpp" || exit /b 1

echo === TEST: verify_main_printf.obj + gen.obj ===
set LIBS=%BUILD%\gen_for_bisect.obj
set LINKLIBS="%NATIVE_LIB%\runtime-core\Release\chaos_runtime_core.lib" "%NATIVE_LIB%\bootstrap\Release\chaos_bootstrap.lib" "%NATIVE_LIB%\interpreter\Release\chaos_interpreter.lib" "%NATIVE_LIB%\common\Release\chaos_common.lib" "%NATIVE_LIB%\support\Release\chaos_support.lib" "%NATIVE_LIB%\hot-update\Release\chaos_hot_update.lib" "%REPO%\build\native\fmt_build\Release\chaos_fmt.lib" "%BDWGC%\Release\chaos_bdwgc.lib" ole32.lib user32.lib

link /nologo /out:"%BUILD%\test_printf_verify.exe" "%BUILD%\verify_main_printf.obj" %LIBS% %LINKLIBS% 2>&1 || exit /b 1
"%BUILD%\test_printf_verify.exe"
echo RC=%ERRORLEVEL%
