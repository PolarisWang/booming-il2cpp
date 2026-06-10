@echo off
call "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvarsall.bat" x64
if %ERRORLEVEL% neq 0 exit /b 1

echo INCLUDE found: ok

cd /d D:\agent\booming-il2cpp

if not exist "tmp\Release" mkdir "tmp\Release"

cl /nologo /std:c++17 /Zc:__cplusplus /Zc:preprocessor /c /EHsc /W3 /utf-8 /O2 /MD /GS- ^
    /DNDEBUG /DCHAOS_IL2CPP_CONFIG_CHECK /DCHAOS_IL2CPP_CHECK ^
    /DCHAOS_IL2CPP_JIT_MODE /DCHaosIl2cpp_EXPORTS ^
    /Isrc/native/jit /Isrc/native/common /Isrc/native/common/chaos ^
    /Isrc/native/runtime-core /Isrc/native/bootstrap ^
    /Isrc/native/interpreter /Isrc/native/support ^
    /Isrc/native/hot-update /Isrc/native/pal /Isrc/native ^
    /Ithird_party/unordered_dense/include /Ithird_party/fmt/include ^
    /Isrc/native/runtime-core/gc /Isrc/native/runtime-core/runtime_stubs ^
    /Fotmp/Release/WinSehHandler.obj src/native/jit/WinSehHandler.cpp

if %ERRORLEVEL% neq 0 (
    echo COMPILATION FAILED
    exit /b 1
)

echo COMPILATION OK
exit /b 0
