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

echo === TEST A: Single TU (verify_printf_main.cpp includes generated code) ===
cl %FLAGS% %INCS% %DEFS% -Fo"%BUILD%\verify_printf_main.obj" "%BUILD%\verify_printf_main.cpp" || exit /b 1

link /nologo /out:"%BUILD%\verify_printf_main.exe" ^
  "%BUILD%\verify_printf_main.obj" ^
  "%NATIVE_LIB%\runtime-core\Release\chaos_runtime_core.lib" ^
  "%NATIVE_LIB%\bootstrap\Release\chaos_bootstrap.lib" ^
  "%NATIVE_LIB%\interpreter\Release\chaos_interpreter.lib" ^
  "%NATIVE_LIB%\common\Release\chaos_common.lib" ^
  "%NATIVE_LIB%\support\Release\chaos_support.lib" ^
  "%NATIVE_LIB%\hot-update\Release\chaos_hot_update.lib" ^
  "%REPO%\build\native\fmt_build\Release\chaos_fmt.lib" ^
  "%BDWGC%\Release\chaos_bdwgc.lib" ^
  ole32.lib user32.lib 2>&1 || exit /b 1
"%BUILD%\verify_printf_main.exe"
echo RC_A=%ERRORLEVEL%

echo.

echo === TEST B: native_verify_main.cpp WITHOUT chaos/common.h ===
rem Write a version that doesn't use CHAOS_IL2CPP_LOG (just printf)
echo #include <cstdlib> > "%BUILD%\noverify_minimal_main.cpp"
echo #include <cstdio> >> "%BUILD%\noverify_minimal_main.cpp"
echo #include <csetjmp> >> "%BUILD%\noverify_minimal_main.cpp"
echo extern "C" int __chaos_assert_failures; >> "%BUILD%\noverify_minimal_main.cpp"
echo extern "C" void SetExceptionFallback(void (*fn)()); >> "%BUILD%\noverify_minimal_main.cpp"
echo extern "C" int RunNativeAot(int entryIndex); >> "%BUILD%\noverify_minimal_main.cpp"
echo #include "expected_checksums.h" >> "%BUILD%\noverify_minimal_main.cpp"
echo static jmp_buf buf; static void fb() { longjmp(buf, 1); } >> "%BUILD%\noverify_minimal_main.cpp"
echo int main() { >> "%BUILD%\noverify_minimal_main.cpp"
echo     __chaos_assert_failures = 0; >> "%BUILD%\noverify_minimal_main.cpp"
echo     SetExceptionFallback(fb); >> "%BUILD%\noverify_minimal_main.cpp"
echo     int rf = 0; >> "%BUILD%\noverify_minimal_main.cpp"
echo     for (int i = 0; i ^< kExpectedCount; i++) { >> "%BUILD%\noverify_minimal_main.cpp"
echo         int actual = 0; bool threw = false; >> "%BUILD%\noverify_minimal_main.cpp"
echo         if (setjmp(buf) == 0) { >> "%BUILD%\noverify_minimal_main.cpp"
echo             actual = RunNativeAot(i); >> "%BUILD%\noverify_minimal_main.cpp"
echo         } else { threw = true; } >> "%BUILD%\noverify_minimal_main.cpp"
echo         if (kExpectedChecksums[i] == -1) continue; >> "%BUILD%\noverify_minimal_main.cpp"
echo         if (threw) { printf("FAIL [%%d]: threw, expected %%d\n", i, kExpectedChecksums[i]); rf++; continue; } >> "%BUILD%\noverify_minimal_main.cpp"
echo         if (actual != kExpectedChecksums[i]) { printf("FAIL [%%d]: expected %%d, got %%d\n", i, kExpectedChecksums[i], actual); rf++; } >> "%BUILD%\noverify_minimal_main.cpp"
echo     } >> "%BUILD%\noverify_minimal_main.cpp"
echo     SetExceptionFallback(nullptr); >> "%BUILD%\noverify_minimal_main.cpp"
echo     int total = __chaos_assert_failures + rf; >> "%BUILD%\noverify_minimal_main.cpp"
echo     printf("L2: %%d/%%d passed (assert=%%d, return=%%d)\n", kExpectedCount - rf, kExpectedCount, __chaos_assert_failures, rf); >> "%BUILD%\noverify_minimal_main.cpp"
echo     return total; >> "%BUILD%\noverify_minimal_main.cpp"
echo } >> "%BUILD%\noverify_minimal_main.cpp"

cl %FLAGS% %INCS% %DEFS% -Fo"%BUILD%\noverify_minimal_main.obj" "%BUILD%\noverify_minimal_main.cpp" || exit /b 1

rem Use SAME gen_for_bisect.obj from earlier
set LIBS=%BUILD%\gen_for_bisect.obj
set LINKLIBS="%NATIVE_LIB%\runtime-core\Release\chaos_runtime_core.lib" "%NATIVE_LIB%\bootstrap\Release\chaos_bootstrap.lib" "%NATIVE_LIB%\interpreter\Release\chaos_interpreter.lib" "%NATIVE_LIB%\common\Release\chaos_common.lib" "%NATIVE_LIB%\support\Release\chaos_support.lib" "%NATIVE_LIB%\hot-update\Release\chaos_hot_update.lib" "%REPO%\build\native\fmt_build\Release\chaos_fmt.lib" "%BDWGC%\Release\chaos_bdwgc.lib" ole32.lib user32.lib

link /nologo /out:"%BUILD%\noverify_minimal_main.exe" "%BUILD%\noverify_minimal_main.obj" %LIBS% %LINKLIBS% 2>&1 || exit /b 1
"%BUILD%\noverify_minimal_main.exe"
echo RC_B=%ERRORLEVEL%
