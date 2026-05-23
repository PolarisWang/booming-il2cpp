@echo off
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvarsall.bat" x64
if errorlevel 1 exit /b 1

echo Building aot_core_ir_reader_test...
msbuild "D:\agent\booming-il2cpp\build\testing\src\native\runtime-core\aot_core_ir_reader_test.vcxproj" /p:Configuration=Debug /v:m
if errorlevel 1 exit /b 1
echo aot_core_ir_reader_test built OK

echo Running aot_core_ir_reader_test...
"D:\agent\booming-il2cpp\build\testing\src\native\runtime-core\Debug\aot_core_ir_reader_test.exe" 2>&1
if errorlevel 1 exit /b 1
echo aot_core_ir_reader_test PASSED
