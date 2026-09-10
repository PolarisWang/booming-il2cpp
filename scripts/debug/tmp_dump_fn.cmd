@echo off
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvarsall.bat" x64 > nul 2>&1
dumpbin /disasm /range:0xDCF0,0xDD5B D:\agent\booming-il2cpp\entry-jit.exe
