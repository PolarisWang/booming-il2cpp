@echo off
call "C:/Program Files/Microsoft Visual Studio/2022/Professional/VC/Auxiliary/Build/vcvarsall.bat" x64 >/dev/null 2>/dev/null
"D:/agent/booming-il2cpp/verification/foundation-dll/System.Private.CoreLib/io-streams-basics/native_test/fact-verify/build/fact_static_io-streams-basics.exe" 2>&1
echo EXIT_CODE=%errorlevel%
