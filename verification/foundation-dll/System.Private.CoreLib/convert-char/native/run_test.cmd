call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat"
cl.exe /EHsc /O2 /utf-8 /Fe:test_throw_ehsc.exe D:\agent\booming-il2cpp\verification\foundation-dll\System.Private.CoreLib\convert-char\native\test_throw.cpp
test_throw_ehsc.exe
echo EXIT: %ERRORLEVEL%
