@echo off
call "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars64.bat" x64 -vcvars_ver=14.42 >nul 2>nul
if %ERRORLEVEL% NEQ 0 exit /b %ERRORLEVEL%

set REPO=D:\agent\booming-il2cpp
set BUILD=%REPO%\verification\foundation-dll\System.Private.CoreLib\convert-char\native_test\l2-verify\build

set INCS=-I"%REPO%\src\native\common" -I"%REPO%\src\native\common\chaos" -I"%REPO%\contracts\native\v0" -I"%REPO%\src\native\runtime-core" -I"%REPO%\src\native\interpreter" -I"%REPO%\src\native\bootstrap" -I"%REPO%\third_party\bdwgc\include" -I"%REPO%\third_party\fmt\include" -I"%BUILD%"
set DEFS=-DCHAOS_IL2CPP_CHECK -DCHAOS_IL2CPP_TRACE_ENABLED -DCHAOS_IL2CPP_VERIFY_MODE -DCHAOS_RUNTIME_ABI_STATIC -DGC_NOT_DLL
set FLAGS=/nologo /std:c++20 /c /EHsc /W3 /utf-8 /Od /MD

echo /* t3 */ > "%BUILD%\t3.cpp"
echo #include <cstdio> >> "%BUILD%\t3.cpp"
echo extern "C" void SetExceptionFallback(void(*)()); >> "%BUILD%\t3.cpp"
echo extern "C" int RunNativeAot(int); >> "%BUILD%\t3.cpp"
echo int main() { printf("t3\n"); fflush(stdout); SetExceptionFallback(nullptr); int x = RunNativeAot(0); printf("got "); printf("%d", x); printf("\n"); return 0; } >> "%BUILD%\t3.cpp"

cl %FLAGS% %INCS% %DEFS% -Fo"%BUILD%\t3.obj" "%BUILD%\t3.cpp" || exit /b 1

echo /* t4 */ > "%BUILD%\t4.cpp"
echo #include <cstdio> >> "%BUILD%\t4.cpp"
echo #include <csetjmp> >> "%BUILD%\t4.cpp"
echo extern "C" void SetExceptionFallback(void(*)()); >> "%BUILD%\t4.cpp"
echo extern "C" int RunNativeAot(int); >> "%BUILD%\t4.cpp"
echo #include "expected_checksums.h" >> "%BUILD%\t4.cpp"
echo static jmp_buf buf; static void fb() { longjmp(buf, 1); } >> "%BUILD%\t4.cpp"
echo int main() { printf("t4\n"); fflush(stdout); SetExceptionFallback(fb); if (setjmp(buf)==0) { RunNativeAot(0); } else { printf("threw\n"); } return 0; } >> "%BUILD%\t4.cpp"

cl %FLAGS% %INCS% %DEFS% -Fo"%BUILD%\t4.obj" "%BUILD%\t4.cpp" || exit /b 1

echo /* t5 */ > "%BUILD%\t5.cpp"
echo #include <cstdio> >> "%BUILD%\t5.cpp"
echo #include <csetjmp> >> "%BUILD%\t5.cpp"
echo extern "C" void SetExceptionFallback(void(*)()); >> "%BUILD%\t5.cpp"
echo extern "C" int RunNativeAot(int); >> "%BUILD%\t5.cpp"
echo #include "expected_checksums.h" >> "%BUILD%\t5.cpp"
echo static jmp_buf buf; static void fb() { longjmp(buf, 1); } >> "%BUILD%\t5.cpp"
echo int main() { SetExceptionFallback(fb); for (int i=0;i<kExpectedCount;i++) { if (setjmp(buf)==0) { int a=RunNativeAot(i); printf("["); printf("%d",i); printf("]="); printf("%d",a); printf("\n"); } else { printf("["); printf("%d",i); printf("] threw\n"); } } fflush(stdout); return 0; } >> "%BUILD%\t5.cpp"

cl %FLAGS% %INCS% %DEFS% -Fo"%BUILD%\t5.obj" "%BUILD%\t5.cpp" || exit /b 1

set NATIVE_LIB=%REPO%\build\native\src\native
set BDWGC=%REPO%\build\native\bdwgc_build
set LIBS=%BUILD%\gen_for_bisect.obj
set LINKLIBS="%NATIVE_LIB%\runtime-core\Release\chaos_runtime_core.lib" "%NATIVE_LIB%\bootstrap\Release\chaos_bootstrap.lib" "%NATIVE_LIB%\interpreter\Release\chaos_interpreter.lib" "%NATIVE_LIB%\common\Release\chaos_common.lib" "%NATIVE_LIB%\support\Release\chaos_support.lib" "%NATIVE_LIB%\hot-update\Release\chaos_hot_update.lib" "%REPO%\build\native\fmt_build\Release\chaos_fmt.lib" "%BDWGC%\Release\chaos_bdwgc.lib" ole32.lib user32.lib

echo === Link STAGE 3 ===
link /nologo /out:"%BUILD%\t3.exe" "%BUILD%\t3.obj" %LIBS% %LINKLIBS% 2>&1 || exit /b 1
"%BUILD%\t3.exe"
echo RC3=%ERRORLEVEL%

echo === Link STAGE 4 ===
link /nologo /out:"%BUILD%\t4.exe" "%BUILD%\t4.obj" %LIBS% %LINKLIBS% 2>&1 || exit /b 1
"%BUILD%\t4.exe"
echo RC4=%ERRORLEVEL%

echo === Link STAGE 5 ===
link /nologo /out:"%BUILD%\t5.exe" "%BUILD%\t5.obj" %LIBS% %LINKLIBS% 2>&1 || exit /b 1
"%BUILD%\t5.exe"
echo RC5=%ERRORLEVEL%
