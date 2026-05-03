@echo off
set VC_ROOT=C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Tools\MSVC\14.42.34433
set WIN_KITS=C:\Program Files (x86)\Windows Kits\10
set REPO=D:\agent\booming-il2cpp
set IMMUTABLE=%REPO%\verification\foundation-dll\System.Collections.Immutable
set PATH=%VC_ROOT%\bin\Hostx64\x64;%PATH%
set INCLUDE=%VC_ROOT%\include;%VC_ROOT%\atlmfc\include;%WIN_KITS%\Include\10.0.22621.0\ucrt;%WIN_KITS%\Include\10.0.22621.0\um;%WIN_KITS%\Include\10.0.22621.0\shared;%WIN_KITS%\Include\10.0.22621.0\winrt;%REPO%\src\native\common;%REPO%\src\native\runtime-core;%REPO%\contracts\native\v0;%REPO%\third_party\fmt\include;%IMMUTABLE%
set LIB=%VC_ROOT%\lib\x64;%WIN_KITS%\Lib\10.0.22621.0\ucrt\x64;%WIN_KITS%\Lib\10.0.22621.0\um\x64

set SRC=%~1
set FAMILY=%~n2
set OUTPUT=%IMMUTABLE%\_build\%FAMILY%.obj

if "%FAMILY%"=="" set FAMILY=%~n1

echo Compiling %FAMILY% ...
echo.

REM Step 1: Fix CHAOS_IL2CPP_ARRAY(T, N> pattern — the emitter uses > as both
REM macro-arg delimiter and template closer, but MSVC's preprocessor needs a
REM real ) to close the macro. Replace "ARRAY(..., N> chaos_xxx" with
REM "ARRAY(..., N) chaos_xxx" since the macro body itself provides the trailing >.
set FIXED_SRC=%IMMUTABLE%\_build\%~n1_fixed.cpp
if not exist "%IMMUTABLE%\_build" mkdir "%IMMUTABLE%\_build"
powershell -Command "(Get-Content '%SRC%') -replace 'CHAOS_IL2CPP_ARRAY\(([^,]+), (\d+)>', 'std::array<$1, $2>' | Set-Content '%FIXED_SRC%'"

REM Step 2: Compile the fixed source
cl /nologo /std:c++17 /utf-8 /FI"_compat.h" /c /Fo%OUTPUT% %FIXED_SRC% 2>&1
exit /b %ERRORLEVEL%
