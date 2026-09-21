@echo off
"C:\Program Files (x86)\Windows Kits\10\Debuggers\x64\cdb.exe" -sins -c "$<D:\agent\chaos-il2cpp\tmp_cdb_b3.txt" "D:\agent\chaos-il2cpp\artifacts\foundation-dll\System.Private.CoreLib\chunks\reflection\native\entry.exe" --fact-json < NUL
