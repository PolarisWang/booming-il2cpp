@echo off
set EXE=D:\agent\chaos-il2cpp\artifacts\presets\windows-x64-reference\tests\unit\runtime-native\runtime-core\gc\Debug\test_gc_atomic_alloc.exe
set SYM=srv*;D:\agent\chaos-il2cpp\artifacts\presets\windows-x64-reference\src\native\runtime-core\Debug;D:\agent\chaos-il2cpp\artifacts\presets\windows-x64-reference\lib\Debug;D:\agent\chaos-il2cpp\artifacts\presets\windows-x64-reference\tests\unit\runtime-native\runtime-core\gc\Debug
set CDB="C:\Program Files (x86)\Windows Kits\10\Debuggers\x64\cdb.exe"
%CDB% -y "%SYM%" -c "sxe av; g; .echo ---FAULT---; r; .echo ---STACK---; kbn 30; .echo ---DONE---; qd" -G -o "%EXE%" 2>&1
echo EXIT_CODE=%ERRORLEVEL%
