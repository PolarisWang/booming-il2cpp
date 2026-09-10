@echo off
set EXE=D:\agent\chaos-il2cpp\artifacts\presets\windows-x64-reference\tests\unit\runtime-native\runtime-core\gc\Debug\test_gc_atomic_alloc.exe
set SYM=D:\agent\chaos-il2cpp\artifacts\presets\windows-x64-reference\src\native\runtime-core\Debug
set CDB="C:\Program Files (x86)\Windows Kits\10\Debuggers\x64\cdb.exe"
%CDB% -y "%SYM%" -c "sxe av; g; .echo ---FAULT---; r; .echo SELF=rcx; .echo ---DUMP this 512B---; dq @rcx L40; .echo ---SC6head content check---; dq @rcx+0x180 L8; q" -G -o "%EXE%" 2>&1"
