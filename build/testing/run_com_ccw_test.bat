@echo off
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvarsall.bat" x64
if errorlevel 1 exit /b 1

echo Building com_ccw_test...
msbuild "D:\agent\booming-il2cpp\build\testing\src\native\runtime-core\com_ccw.vcxproj" /p:Configuration=Debug /v:m
if errorlevel 1 exit /b 1
echo com_ccw_test built OK

echo Running com_ccw test...
"D:\agent\booming-il2cpp\build\testing\src\native\runtime-core\Debug\com_ccw.exe" 2>&1
if errorlevel 1 exit /b 1
echo com_ccw_test PASSED
