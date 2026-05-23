@echo off
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvarsall.bat" x64
if errorlevel 1 exit /b 1

echo Building assembly_manager_test...
msbuild "D:\agent\booming-il2cpp\build\testing\src\native\runtime-core\assembly_manager_test.vcxproj" /p:Configuration=Debug /v:m
if errorlevel 1 exit /b 1
echo assembly_manager_test built OK

echo Running assembly_manager_test...
"D:\agent\booming-il2cpp\build\testing\src\native\runtime-core\Debug\assembly_manager_test.exe" 2>&1
if errorlevel 1 exit /b 1
echo assembly_manager_test PASSED
