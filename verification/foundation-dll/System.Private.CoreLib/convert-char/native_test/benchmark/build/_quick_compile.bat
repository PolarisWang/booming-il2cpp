@echo off
call "C:\Program Files\Microsoft Visual Studio2\Professional\VC\Auxiliary\Buildcvarsall.bat" x64 >/dev/null 2>/dev/null
if %ERRORLEVEL% neq 0 exit /b 1

set inc=-I"D:\agent\booming-il2cpp\src
ative\common" -I"D:\agent\booming-il2cpp\src
ative\common\chaos" -I"D:\agent\booming-il2cpp\contracts
ative0" -I"D:\agent\booming-il2cpp\src
ativeuntime-core" -I"D:\agent\booming-il2cpp	hird_partymt\include" -I"D:\agent\booming-il2cpp\verification\foundation-dll\System.Private.CoreLib\convert-char\native_test\l2-verify\build"

cl /nologo /std:c++20 /c /EHsc /W3 /utf-8 /O2 -DCHAOS_IL2CPP_CHECK %inc% -Fo"D:\agent\booming-il2cpp\verification\foundation-dll\System.Private.CoreLib\convert-char\native_test\benchmark\build
ative-aot.generated.batch2.obj" "D:\agent\booming-il2cpp\verification\foundation-dll\System.Private.CoreLib\convert-char\il2cpp_dist\genuine\generated\native-aot.generated.cpp"
echo CL_EXIT=%ERRORLEVEL%
