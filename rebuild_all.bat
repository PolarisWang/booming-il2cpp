@echo off
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvarsall.bat" x64
if %errorlevel% neq 0 exit /b 1
echo BuildTools env ready

:: Rebuild chaos_jit.lib
cmake --build D:\agent\booming-il2cpp\build\native-profile --target chaos_jit --config Release || exit /b 1

:: Copy to SDK
copy /Y "D:\agent\booming-il2cpp\build\native-profile\src\native\jit\Release\chaos_jit.lib" "D:\agent\booming-il2cpp\testing\foundation-dll\System.Private.CoreLib\convert-char\codegen\ConvertCharSubjects\lib\chaos_jit.lib" || exit /b 1

:: Rebuild entry-jit.exe
set NATIVE_DIR=D:\agent\booming-il2cpp\testing\foundation-dll\System.Private.CoreLib\convert-char\native
set BUILD_DIR=%NATIVE_DIR%\build_jit

if exist %BUILD_DIR% rmdir /s /q %BUILD_DIR%
cmake -S %NATIVE_DIR% -B %BUILD_DIR% -G Ninja -DCHAOS_IL2CPP_CONFIG_TIER=profile -DCHAOS_IL2CPP_JIT_MODE=ON -DCMAKE_BUILD_TYPE=Release || exit /b 1
cmake --build %BUILD_DIR% || exit /b 1

:: Copy result
copy /Y "%BUILD_DIR%\entry.exe" "%NATIVE_DIR%\entry-jit.exe" || exit /b 1

echo BUILD SUCCESS
