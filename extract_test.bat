@echo off
setlocal
cd /d D:\agent\booming-il2cpp
set LIB_PATH=chaos_runtime_core_copy.lib
set MEMBER=chaos_runtime_core.dir\RelWithDebInfo\thread_state.obj
set OUT_OBJ=thread_state_extracted.obj

echo EXTRACT:%MEMBER% > extract.rsp
echo %LIB_PATH% >> extract.rsp
echo /OUT:%OUT_OBJ% >> extract.rsp

"C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Tools\MSVC\14.44.35207\bin\HostX64\x64\lib.exe" @extract.rsp
echo EXIT_CODE=%ERRORLEVEL%
dir thread_state_extracted.obj
