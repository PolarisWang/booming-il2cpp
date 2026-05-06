@echo off
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvarsall.bat" x64 >nul 2>nul
if %ERRORLEVEL% neq 0 exit /b 1

echo Compiling benchmark host...
cl /nologo /std:c++20 /c /EHsc /W3 /utf-8 /O2 /MD -I"D:\agent\booming-il2cpp\src\native\common" -I"D:\agent\booming-il2cpp\src\native\common\chaos" -I"D:\agent\booming-il2cpp\contracts\native\v0" -I"D:\agent\booming-il2cpp\src\native\runtime-core" -I"D:\agent\booming-il2cpp\third_party\fmt\include" -DCHAOS_IL2CPP_CHECK -Fo"D:\agent\booming-il2cpp\verification\foundation-dll\System.Private.CoreLib\buffer-memory\native_test\benchmark\build\native_aot_main.obj" "D:\agent\booming-il2cpp\src\native\benchmark-host\native_aot_main.cpp"
if %ERRORLEVEL% neq 0 exit /b 1

echo Compiling generated C++ (native-aot.generated.cpp)...
cl /nologo /std:c++20 /c /EHsc /W3 /utf-8 /O2 /MD -I"D:\agent\booming-il2cpp\src\native\common" -I"D:\agent\booming-il2cpp\src\native\common\chaos" -I"D:\agent\booming-il2cpp\contracts\native\v0" -I"D:\agent\booming-il2cpp\src\native\runtime-core" -I"D:\agent\booming-il2cpp\third_party\fmt\include" -DCHAOS_IL2CPP_CHECK -Fo"D:\agent\booming-il2cpp\verification\foundation-dll\System.Private.CoreLib\buffer-memory\native_test\benchmark\build\native-aot.generated.obj" "D:\agent\booming-il2cpp\verification\foundation-dll\System.Private.CoreLib\buffer-memory\il2cpp_dist\genuine\generated\native-aot.generated.cpp"
if %ERRORLEVEL% neq 0 exit /b 1

echo Linking...
link /nologo /nodefaultlib:ucrt /out:"D:\agent\booming-il2cpp\verification\foundation-dll\System.Private.CoreLib\buffer-memory\native_test\benchmark\build\benchmark_buffer-memory.exe" "D:\agent\booming-il2cpp\verification\foundation-dll\System.Private.CoreLib\buffer-memory\native_test\benchmark\build\native_aot_main.obj" "D:\agent\booming-il2cpp\verification\foundation-dll\System.Private.CoreLib\buffer-memory\native_test\benchmark\build\native-aot.generated.obj" "D:\agent\booming-il2cpp\build\native-runtime\Release\chaos_runtime_core.lib" "D:\agent\booming-il2cpp\build\native\src\native\interpreter\Release\chaos_interpreter.lib" "D:\agent\booming-il2cpp\build\native\src\native\bootstrap\Release\chaos_bootstrap.lib" "D:\agent\booming-il2cpp\build\native\src\native\support\Release\chaos_support.lib" "D:\agent\booming-il2cpp\build\native\src\native\hot-update\Release\chaos_hot_update.lib" "D:\agent\booming-il2cpp\build\native\fmt_build\Release\chaos_fmt.lib" "D:\agent\booming-il2cpp\build\native\src\native\common\Release\chaos_common.lib" "D:\agent\booming-il2cpp\build\native\bdwgc_build\Release\chaos_bdwgc.lib" "%WindowsSdkDir%lib\%UCRTVersion%\ucrt\x64\ucrt.lib" ole32.lib user32.lib
if %ERRORLEVEL% neq 0 exit /b 1

echo Build OK
