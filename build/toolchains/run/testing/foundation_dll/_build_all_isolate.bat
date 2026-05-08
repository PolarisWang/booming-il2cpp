@echo off
call "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars64.bat" x64 >nul 2>nul
if %errorlevel% neq 0 (
    echo VCVARS FAILED
    exit /b 1
)

set BD=D:\agent\booming-il2cpp\verification\foundation-dll\System.Private.CoreLib\convert-char\native_test\fact-verify\build

:: Step 1 - isolate_static_init
echo === 1. isolate_static_init ===
cl /nologo /std:c++20 /c /EHsc /utf-8 /Od /MD ^
    -I"D:\agent\booming-il2cpp\src\native\common" ^
    -I"D:\agent\booming-il2cpp\src\native\common\chaos" ^
    -I"D:\agent\booming-il2cpp\contracts\native\v0" ^
    -I"D:\agent\booming-il2cpp\src\native\runtime-core" ^
    -I"D:\agent\booming-il2cpp\src\native\interpreter" ^
    -I"D:\agent\booming-il2cpp\src\native\bootstrap" ^
    -I"D:\agent\booming-il2cpp\third_party\bdwgc\include" ^
    -I"D:\agent\booming-il2cpp\third_party\fmt\include" ^
    -I"%BD%" ^
    -DCHAOS_IL2CPP_CHECK -DCHAOS_IL2CPP_TRACE_ENABLED -DCHAOS_IL2CPP_VERIFY_MODE -DCHAOS_RUNTIME_ABI_STATIC -DGC_NOT_DLL ^
    -Fo"%BD%\isolate_static_init.obj" "%BD%\isolate_static_init.cpp"
if %errorlevel% neq 0 exit /b 1
link /nologo /out:"%BD%\isolate_static_init.exe" "%BD%\isolate_static_init.obj"
if %errorlevel% neq 0 exit /b 1
"%BD%\isolate_static_init.exe"
echo RC=%ERRORLEVEL%

:: Step 2 - isolate_crash1
echo === 2. isolate_crash1 ===
cl /nologo /std:c++20 /c /EHsc /utf-8 /Od /MD ^
    -I"D:\agent\booming-il2cpp\src\native\common" ^
    -I"D:\agent\booming-il2cpp\src\native\common\chaos" ^
    -I"D:\agent\booming-il2cpp\contracts\native\v0" ^
    -I"D:\agent\booming-il2cpp\src\native\runtime-core" ^
    -I"D:\agent\booming-il2cpp\src\native\interpreter" ^
    -I"D:\agent\booming-il2cpp\src\native\bootstrap" ^
    -I"D:\agent\booming-il2cpp\third_party\bdwgc\include" ^
    -I"D:\agent\booming-il2cpp\third_party\fmt\include" ^
    -I"%BD%" ^
    -DCHAOS_IL2CPP_CHECK -DCHAOS_IL2CPP_TRACE_ENABLED -DCHAOS_IL2CPP_VERIFY_MODE -DCHAOS_RUNTIME_ABI_STATIC -DGC_NOT_DLL ^
    -Fo"%BD%\isolate_crash1.obj" "%BD%\isolate_crash1.cpp"
if %errorlevel% neq 0 exit /b 1
link /nologo /out:"%BD%\isolate_crash1.exe" "%BD%\isolate_crash1.obj"
if %errorlevel% neq 0 exit /b 1
"%BD%\isolate_crash1.exe"
echo RC=%ERRORLEVEL%

:: Step 3 - isolate_crash
echo === 3. isolate_crash ===
cl /nologo /std:c++20 /c /EHsc /utf-8 /Od /MD ^
    -I"D:\agent\booming-il2cpp\src\native\common" ^
    -I"D:\agent\booming-il2cpp\src\native\common\chaos" ^
    -I"D:\agent\booming-il2cpp\contracts\native\v0" ^
    -I"D:\agent\booming-il2cpp\src\native\runtime-core" ^
    -I"D:\agent\booming-il2cpp\src\native\interpreter" ^
    -I"D:\agent\booming-il2cpp\src\native\bootstrap" ^
    -I"D:\agent\booming-il2cpp\third_party\bdwgc\include" ^
    -I"D:\agent\booming-il2cpp\third_party\fmt\include" ^
    -I"%BD%" ^
    -DCHAOS_IL2CPP_CHECK -DCHAOS_IL2CPP_TRACE_ENABLED -DCHAOS_IL2CPP_VERIFY_MODE -DCHAOS_RUNTIME_ABI_STATIC -DGC_NOT_DLL ^
    -Fo"%BD%\isolate_crash.obj" "%BD%\isolate_crash.cpp"
if %errorlevel% neq 0 exit /b 1
link /nologo /out:"%BD%\isolate_crash.exe" "%BD%\isolate_crash.obj"
if %errorlevel% neq 0 exit /b 1
"%BD%\isolate_crash.exe"
echo RC=%ERRORLEVEL%

echo === ALL DONE ===