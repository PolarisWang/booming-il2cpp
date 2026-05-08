@echo off
call "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars64.bat" x64
if %errorlevel% neq 0 exit /b 1

echo LIB=%LIB%
echo.

set "MSVC_LINK=C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Tools\MSVC\14.38.33130\bin\Hostx64\x64\link.exe"

set BD=D:\agent\booming-il2cpp\verification\foundation-dll\System.Private.CoreLib\convert-char\native_test\fact-verify\build
set REPO=D:\agent\booming-il2cpp
set LIB_PATH=%REPO%\build\native\src\native\runtime-core\RelWithDebInfo\chaos_runtime_core.lib

set INCL=-I"%REPO%\src\native\common" -I"%REPO%\src\native\common\chaos" -I"%REPO%\contracts\native\v0" -I"%REPO%\src\native\runtime-core" -I"%REPO%\src\native\interpreter" -I"%REPO%\src\native\bootstrap" -I"%REPO%\third_party\bdwgc\include" -I"%REPO%\third_party\fmt\include" -I"%BD%"
set DEFS=-DCHAOS_IL2CPP_CHECK -DCHAOS_IL2CPP_VERIFY_MODE -DGC_NOT_DLL -DCHAOS_RUNTIME_ABI_STATIC -D_CRT_SECURE_NO_WARNINGS

echo =========================================
echo 1. Compile fact_verify_combined.obj
echo =========================================
cl /nologo /std:c++20 /c /EHsc /W3 /utf-8 /Od /MD %INCL% %DEFS% -Fo"%BD%\fact_verify_combined.obj" "%BD%\fact_verify_combined.cpp"
if %errorlevel% neq 0 exit /b 1

echo.
echo =========================================
echo 2. Link fact_verify_combined.exe
echo =========================================
"%MSVC_LINK%" /nologo /map:"%BD%\fact_verify_combined.map" /out:"%BD%\fact_verify_combined.exe" "%BD%\fact_verify_combined.obj" "%LIB_PATH%"
if %errorlevel% neq 0 exit /b 1

echo.
echo =========================================
echo 3. Run fact_verify_combined.exe
echo =========================================
"%BD%\fact_verify_combined.exe"
echo RC=%ERRORLEVEL%