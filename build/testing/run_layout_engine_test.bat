@echo off
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvarsall.bat" x64
if errorlevel 1 exit /b 1

echo Building layout_engine_test...
msbuild "D:\agent\booming-il2cpp\build\testing\src\native\runtime-core\layout_engine_test.vcxproj" /p:Configuration=Debug /v:m
if errorlevel 1 exit /b 1
echo layout_engine_test built OK

echo Running layout_engine_test...
"D:\agent\booming-il2cpp\build\testing\src\native\runtime-core\Debug\layout_engine_test.exe" 2>&1
if errorlevel 1 exit /b 1
echo layout_engine_test PASSED
