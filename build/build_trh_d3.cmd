@echo off
call "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvarsall.bat" x64 >nul 2>&1
if %ERRORLEVEL% neq 0 exit /b 1

set INC=-I"D:\agent\booming-il2cpp\src\native\common" -I"D:\agent\booming-il2cpp\src\native\common\chaos" -I"D:\agent\booming-il2cpp\contracts\native\v0" -I"D:\agent\booming-il2cpp\src\native\runtime-core" -I"D:\agent\booming-il2cpp\src\native\bootstrap" -I"D:\agent\booming-il2cpp\src\native\interpreter" -I"D:\agent\booming-il2cpp\third_party\fmt\include" -I"D:\agent\booming-il2cpp\verification\foundation-dll\System.Private.CoreLib"

set DEF=-DCHAOS_IL2CPP_CHECK
set CFLAGS=/nologo /std:c++17 /c /EHac /W3 /utf-8 /O2 /MD /GS-

set SRC_DIR=D:\agent\booming-il2cpp\verification\foundation-dll\System.Private.CoreLib
set FAMILY=type-runtime-handles

echo Compiling HotUpdateTest.cpp...
cl %CFLAGS% %INC% %DEF% -Fo"%SRC_DIR%\%FAMILY%\il2cpp_dist\hotupdate\build\HotUpdateTest.obj" "%SRC_DIR%\%FAMILY%\il2cpp_dist\hotupdate\HotUpdateTest.cpp"
if %ERRORLEVEL% neq 0 exit /b 1

echo Compiling native-aot.generated.cpp...
cl %CFLAGS% %INC% %DEF% -Fo"%SRC_DIR%\%FAMILY%\il2cpp_dist\hotupdate\build\genuine_fixed.obj" "%SRC_DIR%\%FAMILY%\il2cpp_dist\hotupdate\genuine-fixed\native-aot.generated.cpp"
if %ERRORLEVEL% neq 0 exit /b 1

echo Linking...
link /nologo -OUT:"%SRC_DIR%\%FAMILY%\il2cpp_dist\hotupdate\build\hotpatch_verify_%FAMILY%.exe" "%SRC_DIR%\%FAMILY%\il2cpp_dist\hotupdate\build\HotUpdateTest.obj" "%SRC_DIR%\%FAMILY%\il2cpp_dist\hotupdate\build\genuine_fixed.obj"
if %ERRORLEVEL% neq 0 exit /b 1

echo Build OK
