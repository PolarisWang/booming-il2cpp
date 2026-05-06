@echo off
setlocal enabledelayedexpansion

call "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvarsall.bat" x64 >/dev/null 2>/dev/null
if %ERRORLEVEL% neq 0 (
    echo vcvars failed
    exit /b 1
)

set repo_root=D:\agent\booming-il2cpp
set build_dir=%repo_root%\verification\foundation-dll\System.Private.CoreLib\convert-char\native_test\benchmark\build
set gen_cpp=%repo_root%\verification\foundation-dll\System.Private.CoreLib\convert-char\il2cpp_dist\genuine\generated\native-aot.generated.cpp
set verify_dir=%repo_root%\verification\foundation-dll\System.Private.CoreLib\convert-char\native_test\l2-verify\build

set common_inc=-I"%repo_root%\src\native\common" -I"%repo_root%\src\native\common\chaos"
set inc=%common_inc% -I"%repo_root%\contracts\native\v0" -I"%repo_root%\src\native\runtime-core" -I"%repo_root%\third_party\fmt\include" -I"%verify_dir%"

set cflags=/nologo /std:c++20 /c /EHsc /W3 /utf-8 /O2 -DCHAOS_IL2CPP_CHECK

echo Compile via BATCH...
cl %cflags% %inc% -Fo"%build_dir%\native-aot.generated.batch.obj" "%gen_cpp%"
echo CL_EXIT=%ERRORLEVEL%

set chaos_lib=%repo_root%\build\native-runtime\Release\chaos_runtime_core.lib
link /nologo /out:"%build_dir%\simple_batch.exe" "%build_dir%\simple_main.obj" "%build_dir%\native-aot.generated.batch.obj" "%chaos_lib%" ole32.lib
echo LINK_EXIT=%ERRORLEVEL%

echo Running...
"%build_dir%\simple_batch.exe"
echo RUN_EXIT=%ERRORLEVEL%
