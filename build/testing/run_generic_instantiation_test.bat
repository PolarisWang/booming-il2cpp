@echo off
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvarsall.bat" x64
if errorlevel 1 exit /b 1

echo Building generic_instantiation_test...
msbuild "D:\agent\booming-il2cpp\build\testing\src\native\runtime-core\generic_instantiation_test.vcxproj" /p:Configuration=Debug /v:m
if errorlevel 1 exit /b 1
echo generic_instantiation_test built OK

echo Running generic_instantiation_test...
"D:\agent\booming-il2cpp\build\testing\src\native\runtime-core\Debug\generic_instantiation_test.exe" 2>&1
if errorlevel 1 exit /b 1
echo generic_instantiation_test PASSED
