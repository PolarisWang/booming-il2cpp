@echo off
call "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars64.bat" 2>&1
echo INCLUDE=%INCLUDE% > _msvc_env.txt
echo PATH=%PATH% >> _msvc_env.txt
echo LIB=%LIB% >> _msvc_env.txt

cd /d D:\agent\booming-il2cpp\verification\foundation-dll\System.Private.CoreLib\convert-char\native_test\benchmark\build

echo === Compiling native-aot.generated.cpp ===
cl.exe /nologo /c /O2 /MT /std:c++17 /EHsc /utf-8 /DWIN32 /D_WINDOWS /DNDEBUG /DFMT_USE_WINDOWS_H=0 /I"D:\agent\booming-il2cpp\contracts\native\v0" /I"D:\agent\booming-il2cpp\src\native\common" /I"D:\agent\booming-il2cpp\src\native\runtime-core" /I"D:\agent\booming-il2cpp\src\native\bootstrap" /I"D:\agent\booming-il2cpp\third_party\fmt\include" /Fo"native-aot.generated.obj" "D:\agent\booming-il2cpp\verification\foundation-dll\System.Private.CoreLib\convert-char\native\genuine\generated\native-aot.generated.cpp" 2>&1

echo === Linking benchmark_convert-char.exe ===
link.exe /nologo /OUT:benchmark_convert-char.exe native-aot.generated.obj native_aot_main.obj runtime_stubs.obj dispatch_table.obj module_registry.obj string_table.obj abi_manifest.obj ole32.lib 2>&1

echo === Done ===
dir benchmark_convert-char.exe
