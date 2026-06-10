@echo off
call "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvarsall.bat" x64
if %ERRORLEVEL% neq 0 exit /b 1
cd /d D:\agent\booming-il2cpp
MSBuild.exe "testing/foundation-dll/System.Private.CoreLib/chunks/buffers/build_jit_output/build_vs/chaos_entry.vcxproj" /p:Configuration=RelWithDebInfo /p:Platform=x64 /t:Build
exit /b %ERRORLEVEL%
