@echo off
call "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvarsall.bat" x64 >nul
set INC=/I "D:\agent\booming-il2cpp\contracts\native\v0" /I "D:\agent\booming-il2cpp\src\native\runtime-core" /I "D:\agent\booming-il2cpp\src\native\interpreter" /I "D:\agent\booming-il2cpp\src\native\common" /I "D:\agent\booming-il2cpp\third_party\bdwgc\include"
set SRC="D:\agent\booming-il2cpp\tests\contracts\native\interpreter\interpreter_integration.cpp" "D:\agent\booming-il2cpp\src\native\interpreter\interpreter_vm.cpp" "D:\agent\booming-il2cpp\src\native\runtime-core\il_to_ir_lowerer.cpp" "D:\agent\booming-il2cpp\src\native\runtime-core\vtable_registry_stub.cpp"
cl.exe /nologo /std:c++17 /EHsc %SRC% %INC% /Fe:"D:\agent\booming-il2cpp\interpreter_test.exe" 2>&1
if %ERRORLEVEL% NEQ 0 exit /b %ERRORLEVEL%
echo COMPILE_LINK_OK
"D:\agent\booming-il2cpp\interpreter_test.exe"
echo TEST_EXIT=%ERRORLEVEL%
