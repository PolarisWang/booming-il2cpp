@echo off
call "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvarsall.bat" x64 >nul 2>&1
if %ERRORLEVEL% neq 0 exit /b 1

set INC=-I"D:\agent\booming-il2cpp\src\native\common" -I"D:\agent\booming-il2cpp\src\native\common\chaos" -I"D:\agent\booming-il2cpp\contracts\native\v0" -I"D:\agent\booming-il2cpp\src\native\runtime-core" -I"D:\agent\booming-il2cpp\src\native\bootstrap" -I"D:\agent\booming-il2cpp\src\native\interpreter" -I"D:\agent\booming-il2cpp\third_party\fmt\include" -I"D:\agent\booming-il2cpp\verification\foundation-dll\System.Private.CoreLib"

set DEF=-DCHAOS_IL2CPP_CHECK
set CFLAGS=/nologo /std:c++17 /c /EHac /W3 /utf-8 /O2 /MD /GS-

set SRC_DIR=D:\agent\booming-il2cpp\verification\foundation-dll\System.Private.CoreLib
set FAMILY=%1
if "%FAMILY%"=="" set FAMILY=type-runtime-handles

set LIB_DIR=D:\agent\booming-il2cpp\build\src\native
set LIBS=-LIBPATH:"%LIB_DIR%\runtime-core\Release" -LIBPATH:"%LIB_DIR%\hot-update\Release" -LIBPATH:"%LIB_DIR%\interpreter\Release" -LIBPATH:"%LIB_DIR%\bootstrap\Release" -LIBPATH:"%LIB_DIR%\common\Release" chaos_runtime_core.lib chaos_hot_update.lib chaos_interpreter.lib chaos_bootstrap.lib chaos_common.lib

echo.
echo === Building Hotpatch hotupdate test for %FAMILY% ===
echo.

echo Compiling HotUpdateTest.cpp...
cl %CFLAGS% %INC% %DEF% -Fo"%SRC_DIR%\%FAMILY%\il2cpp_dist\hotupdate\build\HotUpdateTest.obj" "%SRC_DIR%\%FAMILY%\il2cpp_dist\hotupdate\HotUpdateTest.cpp"
if %ERRORLEVEL% neq 0 exit /b 1

echo Compiling native-aot.generated.cpp...
cl %CFLAGS% %INC% %DEF% -Fo"%SRC_DIR%\%FAMILY%\il2cpp_dist\hotupdate\build\genuine_fixed.obj" "%SRC_DIR%\%FAMILY%\il2cpp_dist\hotupdate\genuine-fixed\native-aot.generated.cpp"
if %ERRORLEVEL% neq 0 exit /b 1

echo Linking...
link /nologo %LIBS% -OUT:"%SRC_DIR%\%FAMILY%\il2cpp_dist\hotupdate\build\hotpatch_verify_%FAMILY%.exe" "%SRC_DIR%\%FAMILY%\il2cpp_dist\hotupdate\build\HotUpdateTest.obj" "%SRC_DIR%\%FAMILY%\il2cpp_dist\hotupdate\build\genuine_fixed.obj"
if %ERRORLEVEL% neq 0 exit /b 1

echo Build OK for %FAMILY%
