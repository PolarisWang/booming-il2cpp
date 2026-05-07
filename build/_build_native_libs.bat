@echo off
call "C:/Program Files (x86)/Microsoft Visual Studio/2022/BuildTools/VC/Auxiliary/Build/vcvarsall.bat" x64 >nul 2>nul
if %ERRORLEVEL% neq 0 exit /b 1
msbuild build/native/src/native/interpreter/chaos_interpreter.vcxproj /p:Configuration=RelWithDebInfo /p:Platform=x64 /p:BuildProjectReferences=false /t:Build
if %ERRORLEVEL% neq 0 exit /b 1
msbuild build/native/src/native/bootstrap/chaos_bootstrap.vcxproj /p:Configuration=RelWithDebInfo /p:Platform=x64 /p:BuildProjectReferences=false /t:Build
if %ERRORLEVEL% neq 0 exit /b 1
msbuild build/native/src/native/support/chaos_support.vcxproj /p:Configuration=RelWithDebInfo /p:Platform=x64 /p:BuildProjectReferences=false /t:Build
if %ERRORLEVEL% neq 0 exit /b 1
msbuild build/native/src/native/hot-update/chaos_hot_update.vcxproj /p:Configuration=RelWithDebInfo /p:Platform=x64 /p:BuildProjectReferences=false /t:Build
if %ERRORLEVEL% neq 0 exit /b 1
echo ALL LIBS BUILT OK
