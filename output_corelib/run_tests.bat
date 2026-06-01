@echo off
setlocal enabledelayedexpansion

set RUNTIME=D:\agent\booming-il2cpp\src\reference\Chaos.TestFramework.Runtime
set OUT=D:\agent\booming-il2cpp\output_corelib

for %%t in (
  System.Boolean System.Int32 System.Version System.Guid
  System.TimeSpan System.DateTime System.Decimal System.Double
  System.Math System.Convert System.Enum System.Random
  System.String System.Text.Encoding System.Text.StringBuilder
  System.BitConverter System.IO.Path System.IO.File System.Array
) do (
  for /f "usebackq delims=" %%d in (`dir /s /b "%OUT%\%%t\bin\%%t.dll" 2^>nul`) do (
    set DLL=%%d
    goto :found_%%t
  )
  :found_%%t
  echo.
  echo === %%t ===
  if defined DLL (
    dotnet run --project "%RUNTIME%" --no-build -- --assembly "!DLL!" --kind fact
  ) else (
    echo NO DLL FOUND for %%t
  )
  set DLL=
)
