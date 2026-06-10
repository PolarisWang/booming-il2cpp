@echo off
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvarsall.bat" x64
MSBuild.exe "D:\agent\booming-il2cpp\testing\foundation-dll\System.Private.CoreLib\chunks\buffers\build_jit_output\build_vs\chaos_entry.vcxproj" /p:Configuration=RelWithDebInfo /p:Platform=x64 /t:Build
if %ERRORLEVEL% neq 0 exit /b 1
copy /Y "D:\agent\booming-il2cpp\testing\foundation-dll\System.Private.CoreLib\chunks\buffers\build_jit_output\build_vs\RelWithDebInfo\chaos_entry.exe" "D:\agent\booming-il2cpp\entry-jit.exe"
exit /b 0
