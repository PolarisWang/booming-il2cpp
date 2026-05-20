@echo off
set LIBPATH=D:\agent\booming-il2cpp\artifacts\presets\debug\src\native\runtime-core\RelWithDebInfo\chaos_runtime_core.lib
"C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Tools\MSVC\14.44.35207\bin\HostX64\x64\dumpbin.exe" /ARCHIVEMEMBERS "%LIBPATH%" 2>&1 | findstr /i "\.obj"
if %ERRORLEVEL% neq 0 echo NO_OBJ_FOUND
