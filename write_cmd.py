import sys
content = r"""@echo off
call "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvarsall.bat" x64 >nul 2>&1
if %ERRORLEVEL% neq 0 exit /b 1
cd /d D:\agent\booming-il2cpp\build\native
if %ERRORLEVEL% neq 0 exit /b 1
msbuild chaos_interpreter_integration.vcxproj /p:Configuration=RelWithDebInfo /p:Platform=x64 /t:Build /nologo /verbosity:minimal
if %ERRORLEVEL% neq 0 exit /b 1
echo.
echo === Running chaos_interpreter_integration ===
tests\contracts\native\interpreter\RelWithDebInfo\chaos_interpreter_integration.exe
if %ERRORLEVEL% neq 0 exit /b 1
echo ALL TESTS PASSED
"""
path = sys.argv[1]
with open(path, 'w') as f:
    f.write(content)
print(f"Written to {path}")
