"""Run L2 verify exe and capture output."""
import subprocess
import sys

exe = r"D:\agent\booming-il2cpp\verification\foundation-dll\System.Private.CoreLib\convert-char\native_test\l2-verify\build\verify_convert-char.exe"
vcvars = r"C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars64.bat"

cmd = f'cmd.exe /c ""{vcvars}" x64 -vcvars_ver=14.42 >nul 2>nul && "{exe}""'
r = subprocess.run(cmd, shell=True, capture_output=True, timeout=120)
out = (r.stdout + r.stderr).decode("utf-8", errors="replace")
print(out[:3000])
print(f"exit: {r.returncode}", flush=True)
