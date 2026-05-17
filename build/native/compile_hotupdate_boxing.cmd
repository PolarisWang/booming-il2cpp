@echo off
REM Manually set up MSVC 14.44 BuildTools environment
set VCTOOLS=C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Tools\MSVC\14.44.35207
set WINSDK=C:\Program Files (x86)\Windows Kits\10

set PATH=%VCTOOLS%\bin\Hostx64\x64;%PATH%
set INCLUDE=%VCTOOLS%\include;%WINSDK%\include\10.0.22621.0\ucrt;%WINSDK%\include\10.0.22621.0\shared;%WINSDK%\include\10.0.22621.0\um;%WINSDK%\include\10.0.22621.0\winrt;
set LIB=%VCTOOLS%\lib\x64;%WINSDK%\lib\10.0.22621.0\ucrt\x64;%WINSDK%\lib\10.0.22621.0\um\x64;

echo Using MSVC from: %VCTOOLS%
where cl.exe
cl.exe 2>&1 | find "Microsoft"
echo.

set REPO=D:\agent\booming-il2cpp
set OUTDIR=%REPO%\build\native\verification\foundation-dll\System.Private.CoreLib\boxing-unboxing-casts\il2cpp_dist\RelWithDebInfo

if not exist "%OUTDIR%" mkdir "%OUTDIR%"

set INCLUDE_DIRS=/I"%REPO%\src\native\common" /I"%REPO%\contracts\native\v0" /I"%REPO%\src\native\runtime-core" /I"%REPO%\src\native\runtime-core\gc" /I"%REPO%\src\native\bootstrap" /I"%REPO%\src\native\interpreter" /I"%REPO%\src\native\interpreter\generated" /I"%REPO%\src\native" /I"%REPO%\src\native\support" /I"%REPO%\third_party\fmt\include" /I"%REPO%\third_party\unordered_dense\include" /I"%REPO%\verification\foundation-dll\System.Private.CoreLib" /I"%REPO%\third_party\bdwgc\include"

set DEFINES=/DNOMINMAX /DCHAOS_RUNTIME_ABI_EXPORTS /DWIN32 /D_WINDOWS /DNDEBUG /DCHAOS_IL2CPP_LOG_LEVEL=3

set COMPILE_OPTS=/MD /O2 /EHa /utf-8 /GS- /std:c++20 /c /FI"%REPO%\verification\foundation-dll\System.Private.CoreLib\native_hotupdate_config.h"

set LIBS=%REPO%\build\native\src\native\hot-update\RelWithDebInfo\chaos_hot_update.lib
set LIBS=%LIBS% %REPO%\build\native\src\native\bootstrap\RelWithDebInfo\chaos_bootstrap.lib
set LIBS=%LIBS% %REPO%\build\native\src\native\runtime-core\RelWithDebInfo\chaos_runtime_core.lib
set LIBS=%LIBS% %REPO%\build\native\src\native\interpreter\RelWithDebInfo\chaos_interpreter.lib
set LIBS=%LIBS% %REPO%\build\native\src\native\common\RelWithDebInfo\chaos_common.lib
set LIBS=%LIBS% %REPO%\build\native\src\native\support\RelWithDebInfo\chaos_support.lib
set LIBS=%LIBS% %REPO%\build\native\fmt_build\RelWithDebInfo\chaos_fmt.lib

echo === Compiling patch_loader.cpp with HOTPATCH_DEBUG ===
cl %COMPILE_OPTS% %DEFINES% /DCHAOS_HOTPATCH_DEBUG=1 %INCLUDE_DIRS% /Fo"%OUTDIR%\patch_loader_debug.obj" "%REPO%\src\native\runtime-core\patch_loader.cpp"
if %errorlevel% neq 0 exit /b %errorlevel%

echo === Compiling HotUpdateTest.cpp ===
cl %COMPILE_OPTS% %DEFINES% %INCLUDE_DIRS% /Fo"%OUTDIR%\HotUpdateTest.obj" "%REPO%\verification\foundation-dll\System.Private.CoreLib\boxing-unboxing-casts\il2cpp_dist\hotupdate\HotUpdateTest.cpp"
if %errorlevel% neq 0 exit /b %errorlevel%

echo === Compiling native-aot.generated.cpp ===
cl %COMPILE_OPTS% %DEFINES% %INCLUDE_DIRS% /Fo"%OUTDIR%\native-aot.generated.obj" "%REPO%\verification\foundation-dll\System.Private.CoreLib\boxing-unboxing-casts\il2cpp_dist\hotupdate\genuine-fixed\native-aot.generated.cpp"
if %errorlevel% neq 0 exit /b %errorlevel%

echo === Linking ===
link /OUT:"%OUTDIR%\chaos_hotupdate_boxing_unboxing_casts.exe" /MACHINE:x64 /SUBSYSTEM:CONSOLE "%OUTDIR%\HotUpdateTest.obj" "%OUTDIR%\native-aot.generated.obj" "%OUTDIR%\patch_loader_debug.obj" %LIBS% ole32.lib oleaut32.lib
if %errorlevel% neq 0 exit /b %errorlevel%

echo === Build SUCCESS ===
