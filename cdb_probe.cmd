@echo off
set EXE=D:\agent\chaos-il2cpp\artifacts\presets\windows-x64-reference\tests\unit\runtime-native\runtime-core\gc\Debug\test_gc_atomic_alloc.exe
set SYM=D:\agent\chaos-il2cpp\artifacts\presets\windows-x64-reference\src\native\runtime-core\Debug
set CDB="C:\Program Files (x86)\Windows Kits\10\Debuggers\x64\cdb.exe"
%CDB% -y "%SYM%" -c ".symopt+ 0x40; sxe av; g; .echo ---FAULT---; r; .echo --- THIS+oldgen dump ---; bp test_gc_atomic_alloc!chaos::il2cpp::runtime_core::MarkSweepOldGen::PopFreeBlockLocked \"r; .echo HDR; dq @rcx L40; .echo HEAD6; dd free_lists_head_??; g \\\"\" ; .echo ---done1---; q" -G -o "%EXE%" 2>&1"
