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

echo === STAGE 1: bare main, no extra includes ===
>"%BUILD%\t1.cpp" echo #include ^<cstdio^>
>>"%BUILD%\t1.cpp" echo extern "C" void SetExceptionFallback(void(*^)^);
>>"%BUILD%\t1.cpp" echo int main(^) { printf("t1\n"^); fflush(stdout^); SetExceptionFallback(nullptr^); return 0; }
cl %FLAGS% %INCS% %DEFS% -Fo"%BUILD%\t1.obj" "%BUILD%\t1.cpp" || exit /b 1

echo === STAGE 2: add RunNativeAot decl but dont call ===
>"%BUILD%\t2.cpp" echo #include ^<cstdio^>
>>"%BUILD%\t2.cpp" echo extern "C" void SetExceptionFallback(void(*^)^);
>>"%BUILD%\t2.cpp" echo extern "C" int RunNativeAot(int^);
>>"%BUILD%\t2.cpp" echo int main(^) { printf("t2\n"^); fflush(stdout^); SetExceptionFallback(nullptr^); return 0; }
cl %FLAGS% %INCS% %DEFS% -Fo"%BUILD%\t2.obj" "%BUILD%\t2.cpp" || exit /b 1

echo === STAGE 3: call RunNativeAot(0) but no setjmp/longjmp ===
>"%BUILD%\t3.cpp" echo #include ^<cstdio^>
>>"%BUILD%\t3.cpp" echo extern "C" void SetExceptionFallback(void(*^)^);
>>"%BUILD%\t3.cpp" echo extern "C" int RunNativeAot(int^);
>>"%BUILD%\t3.cpp" echo int main(^) { printf("t3\n"^); fflush(stdout^); SetExceptionFallback(nullptr^); int x = RunNativeAot(0^); printf("got %d\n", x^); return 0; }
cl %FLAGS% %INCS% %DEFS% -Fo"%BUILD%\t3.obj" "%BUILD%\t3.cpp" || exit /b 1

echo === STAGE 4: include expected_checksums.h + setjmp ===
>"%BUILD%\t4.cpp" echo #include ^<cstdio^>
>>"%BUILD%\t4.cpp" echo #include ^<csetjmp^>
>>"%BUILD%\t4.cpp" echo extern "C" void SetExceptionFallback(void(*^)^);
>>"%BUILD%\t4.cpp" echo extern "C" int RunNativeAot(int^);
>>"%BUILD%\t4.cpp" echo #include "expected_checksums.h"
>>"%BUILD%\t4.cpp" echo static jmp_buf buf; static void fb(^) { longjmp(buf, 1^); }
>>"%BUILD%\t4.cpp" echo int main(^) { printf("t4\n"^); fflush(stdout^); SetExceptionFallback(fb^); if (setjmp(buf^) == 0^) { RunNativeAot(0^); } else { printf("threw\n"^); } return 0; }
cl %FLAGS% %INCS% %DEFS% -Fo"%BUILD%\t4.obj" "%BUILD%\t4.cpp" || exit /b 1

echo === STAGE 5: all 18 entries in a loop ===
>"%BUILD%\t5.cpp" echo #include ^<cstdio^>
>>"%BUILD%\t5.cpp" echo #include ^<csetjmp^>
>>"%BUILD%\t5.cpp" echo extern "C" void SetExceptionFallback(void(*^)^);
>>"%BUILD%\t5.cpp" echo extern "C" int RunNativeAot(int^);
>>"%BUILD%\t5.cpp" echo #include "expected_checksums.h"
>>"%BUILD%\t5.cpp" echo static jmp_buf buf; static void fb(^) { longjmp(buf, 1^); }
>>"%BUILD%\t5.cpp" echo int main(^) { SetExceptionFallback(fb^); for (int i=0; i^kExpectedCount; i++^) { if (setjmp(buf^)==0^) { int a = RunNativeAot(i^); printf("[%d]=%d\n", i, a^); } else { printf("[%d] threw\n", i^); } } fflush(stdout^); return 0; }
cl %FLAGS% %INCS% %DEFS% -Fo"%BUILD%\t5.obj" "%BUILD%\t5.cpp" || exit /b 1

set LIBS=%BUILD%\gen_for_bisect.obj

echo === Link STAGE 1 ===
link /nologo /out:"%BUILD%\t1.exe" "%BUILD%\t1.obj" %LIBS% "%NATIVE_LIB%\runtime-core\Release\chaos_runtime_core.lib" "%NATIVE_LIB%\bootstrap\Release\chaos_bootstrap.lib" "%NATIVE_LIB%\interpreter\Release\chaos_interpreter.lib" "%NATIVE_LIB%\common\Release\chaos_common.lib" "%NATIVE_LIB%\support\Release\chaos_support.lib" "%NATIVE_LIB%\hot-update\Release\chaos_hot_update.lib" "%REPO%\build\native\fmt_build\Release\chaos_fmt.lib" "%BDWGC%\Release\chaos_bdwgc.lib" ole32.lib user32.lib 2>&1 || echo LINK FAILED
"%BUILD%\t1.exe" && echo RC1=%ERRORLEVEL% || echo RC1=%ERRORLEVEL%
echo.

echo === Link STAGE 2 ===
link /nologo /out:"%BUILD%\t2.exe" "%BUILD%\t2.obj" %LIBS% "%NATIVE_LIB%\runtime-core\Release\chaos_runtime_core.lib" "%NATIVE_LIB%\bootstrap\Release\chaos_bootstrap.lib" "%NATIVE_LIB%\interpreter\Release\chaos_interpreter.lib" "%NATIVE_LIB%\common\Release\chaos_common.lib" "%NATIVE_LIB%\support\Release\chaos_support.lib" "%NATIVE_LIB%\hot-update\Release\chaos_hot_update.lib" "%REPO%\build\native\fmt_build\Release\chaos_fmt.lib" "%BDWGC%\Release\chaos_bdwgc.lib" ole32.lib user32.lib 2>&1 || echo LINK FAILED
"%BUILD%\t2.exe" && echo RC2=%ERRORLEVEL% || echo RC2=%ERRORLEVEL%
echo.

echo === Link STAGE 3 ===
link /nologo /out:"%BUILD%\t3.exe" "%BUILD%\t3.obj" %LIBS% "%NATIVE_LIB%\runtime-core\Release\chaos_runtime_core.lib" "%NATIVE_LIB%\bootstrap\Release\chaos_bootstrap.lib" "%NATIVE_LIB%\interpreter\Release\chaos_interpreter.lib" "%NATIVE_LIB%\common\Release\chaos_common.lib" "%NATIVE_LIB%\support\Release\chaos_support.lib" "%NATIVE_LIB%\hot-update\Release\chaos_hot_update.lib" "%REPO%\build\native\fmt_build\Release\chaos_fmt.lib" "%BDWGC%\Release\chaos_bdwgc.lib" ole32.lib user32.lib 2>&1 || echo LINK FAILED
"%BUILD%\t3.exe" && echo RC3=%ERRORLEVEL% || echo RC3=%ERRORLEVEL%
echo.

echo === Link STAGE 4 ===
link /nologo /out:"%BUILD%\t4.exe" "%BUILD%\t4.obj" %LIBS% "%NATIVE_LIB%\runtime-core\Release\chaos_runtime_core.lib" "%NATIVE_LIB%\bootstrap\Release\chaos_bootstrap.lib" "%NATIVE_LIB%\interpreter\Release\chaos_interpreter.lib" "%NATIVE_LIB%\common\Release\chaos_common.lib" "%NATIVE_LIB%\support\Release\chaos_support.lib" "%NATIVE_LIB%\hot-update\Release\chaos_hot_update.lib" "%REPO%\build\native\fmt_build\Release\chaos_fmt.lib" "%BDWGC%\Release\chaos_bdwgc.lib" ole32.lib user32.lib 2>&1 || echo LINK FAILED
"%BUILD%\t4.exe" && echo RC4=%ERRORLEVEL% || echo RC4=%ERRORLEVEL%
echo.

echo === Link STAGE 5 ===
link /nologo /out:"%BUILD%\t5.exe" "%BUILD%\t5.obj" %LIBS% "%NATIVE_LIB%\runtime-core\Release\chaos_runtime_core.lib" "%NATIVE_LIB%\bootstrap\Release\chaos_bootstrap.lib" "%NATIVE_LIB%\interpreter\Release\chaos_interpreter.lib" "%NATIVE_LIB%\common\Release\chaos_common.lib" "%NATIVE_LIB%\support\Release\chaos_support.lib" "%NATIVE_LIB%\hot-update\Release\chaos_hot_update.lib" "%REPO%\build\native\fmt_build\Release\chaos_fmt.lib" "%BDWGC%\Release\chaos_bdwgc.lib" ole32.lib user32.lib 2>&1 || echo LINK FAILED
"%BUILD%\t5.exe" && echo RC5=%ERRORLEVEL% || echo RC5=%ERRORLEVEL%
echo.
