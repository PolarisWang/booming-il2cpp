@echo off
call "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars64.bat" x64 >nul 2>nul
if %errorlevel% neq 0 exit /b 1

set BD=D:\agent\booming-il2cpp\verification\foundation-dll\System.Private.CoreLib\convert-char\native_test\fact-verify\build
set INCL=-I"D:\agent\booming-il2cpp\src\native\common" -I"D:\agent\booming-il2cpp\src\native\common\chaos" -I"D:\agent\booming-il2cpp\contracts\native\v0" -I"D:\agent\booming-il2cpp\src\native\runtime-core" -I"D:\agent\booming-il2cpp\src\native\interpreter" -I"D:\agent\booming-il2cpp\src\native\bootstrap" -I"D:\agent\booming-il2cpp\third_party\bdwgc\include" -I"D:\agent\booming-il2cpp\third_party\fmt\include" -I"%BD%"
set DEFS=-DCHAOS_IL2CPP_CHECK -DCHAOS_IL2CPP_TRACE_ENABLED -DCHAOS_IL2CPP_VERIFY_MODE -DCHAOS_RUNTIME_ABI_STATIC -DGC_NOT_DLL
set CFLAGS=/nologo /std:c++20 /c /EHsc /utf-8 /Od /MD

:: Compile isolate_crash1.cpp (no generated include — tests linkage)
echo === Compile isolate_crash1 ===
cl %CFLAGS% %INCL% %DEFS% -Fo"%BD%\isolate_crash1.obj" "%BD%\isolate_crash1.cpp"
if %errorlevel% neq 0 exit /b 1
echo === Link isolate_crash1 ===
link /nologo /out:"%BD%\isolate_crash1.exe" "%BD%\isolate_crash1.obj"
if %errorlevel% neq 0 exit /b 1
echo === Run isolate_crash1 ===
"%BD%\isolate_crash1.exe"
echo RC=%ERRORLEVEL%