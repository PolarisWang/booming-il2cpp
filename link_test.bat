@echo on
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat"
cd /d D:\agent\booming-il2cpp
cl /nologo /c /MD /EHa /std:c++17 /Foentry_test.obj test_link.cpp
link /nologo /OUT:entry_test.exe entry_test.obj /LIBPATH:artifacts\presets\windows-x64-reference\src\native\runtime-core\RelWithDebInfo chaos_runtime_core.lib /LIBPATH:artifacts\presets\windows-x64-reference\fmt_build\RelWithDebInfo chaos_fmt.lib /LIBPATH:artifacts\presets\windows-x64-reference\src\native\common\RelWithDebInfo chaos_common.lib
echo LINK_EXIT=%ERRORLEVEL%
