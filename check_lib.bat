@echo off
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat" >nul 2>&1
dumpbin /ARCHIVEMEMBERS "D:\agent\booming-il2cpp\artifacts\presets\debug\src\native\runtime-core\RelWithDebInfo\chaos_runtime_core.lib" 2>&1 | findstr /i "thread"
