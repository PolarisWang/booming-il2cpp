@echo off
call "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvarsall.bat" x64 >nul 2>nul
if %errorlevel% neq 0 exit /b 1

set BD=D:\agent\booming-il2cpp\verification\foundation-dll\System.Private.CoreLib\convert-char\native_test\fact-verify\build
set REPO=D:\agent\booming-il2cpp
set LIB=%REPO%\build\native\src\native\runtime-core\RelWithDebInfo\chaos_runtime_core.lib

set INCL=-I"%REPO%\src\native\common" -I"%REPO%\src\native\common\chaos" -I"%REPO%\contracts\native\v0" -I"%REPO%\src\native\runtime-core" -I"%REPO%\src\native\interpreter" -I"%REPO%\src\native\bootstrap" -I"%REPO%\third_party\bdwgc\include" -I"%REPO%\third_party\fmt\include" -I"%BD%"
set DEFS=-DCHAOS_IL2CPP_CHECK -DCHAOS_IL2CPP_VERIFY_MODE -DGC_NOT_DLL -DCHAOS_RUNTIME_ABI_STATIC -D_CRT_SECURE_NO_WARNINGS

echo =========================================
echo Build fact_verify_combined.exe (with chaos_runtime_core.lib)
echo =========================================
cl /nologo /std:c++20 /c /EHsc /W3 /utf-8 /Od /MD %INCL% %DEFS% -Fo"%BD%\fact_verify_combined.obj" "%BD%\fact_verify_combined.cpp"
if %errorlevel% neq 0 exit /b 1

link /nologo /map:"%BD%\fact_verify_combined.map" /out:"%BD%\fact_verify_combined.exe" "%BD%\fact_verify_combined.obj" "%LIB%"
if %errorlevel% neq 0 exit /b 1

echo --- Running fact_verify_combined.exe ---
"%BD%\fact_verify_combined.exe"
echo RC=%ERRORLEVEL%