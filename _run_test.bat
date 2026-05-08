@echo off
call "C:/Program Files/Microsoft Visual Studio/2022/Professional/VC/Auxiliary/Build/vcvarsall.bat" x64 -vcvars_ver=14.42 >/dev/null 2>/dev/null
if %errorlevel% neq 0 exit /b 1
"verification/foundation-dll/System.Private.CoreLib/io-streams-basics/native_test/fact-verify/build/fact_static_io-streams-basics.exe"
