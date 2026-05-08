@echo on
@setlocal
call "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars64.bat" x64
if %errorlevel% neq 0 (
    echo [FAIL] vcvars
    exit /b 1
)
echo [INFO] Running...
D:\agent\booming-il2cpp\verification\foundation-dll\System.Private.CoreLib\convert-char\native_test\fact-verify\build\fact_static_convert-char.exe
echo [INFO] EXIT_CODE=%ERRORLEVEL%
@endlocal