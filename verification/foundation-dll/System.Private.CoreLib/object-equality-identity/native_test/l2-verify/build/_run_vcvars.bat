@echo off
call "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvarsall.bat" x64 >nul
"D:\agent\booming-il2cpp\verification\foundation-dll\System.Private.CoreLib\object-equality-identity\native_test\l2-verify\build\verify_object-equality-identity.exe"
exit /b %ERRORLEVEL%
