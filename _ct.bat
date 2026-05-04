@echo off
call "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvarsall.bat" x64 >nul
cl.exe /nologo /std:c++17 /EHsc /c /I D:\agent\booming-il2cpp\contracts\native\v0 /I D:\agent\booming-il2cpp\src\native\runtime-core /I D:\agent\booming-il2cpp\src\native\interpreter /I D:\agent\booming-il2cpp\src\native\common /I D:\agent\booming-il2cpp\third_party\bdwgc\include D:\agent\booming-il2cpp\tests\contracts\native\interpreter\interpreter_integration.cpp /FoD:\agent\booming-il2cpp\interpreter_test.obj
echo EXIT=%ERRORLEVEL%
