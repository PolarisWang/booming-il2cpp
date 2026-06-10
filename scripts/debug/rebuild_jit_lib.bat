@echo off
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvarsall.bat" x64
if %errorlevel% neq 0 exit /b 1
echo BuildTools env ready

:: Force rebuild chaos_jit.lib
msbuild D:\agent\booming-il2cpp\build\native-profile\src\native\jit\chaos_jit.vcxproj /p:Configuration=Release /p:Platform=x64 /t:Rebuild
if %errorlevel% neq 0 exit /b %errorlevel%

:: Copy to SDK
copy /Y "D:\agent\booming-il2cpp\build\native-profile\src\native\jit\Release\chaos_jit.lib" "D:\agent\booming-il2cpp\testing\foundation-dll\System.Private.CoreLib\convert-char\codegen\ConvertCharSubjects\lib\chaos_jit.lib"
if %errorlevel% neq 0 exit /b %errorlevel%

echo chaos_jit.lib rebuilt and copied
