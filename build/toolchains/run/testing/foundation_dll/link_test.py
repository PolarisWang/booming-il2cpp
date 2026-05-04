"""Quick linker test for native benchmark."""
import subprocess, os, sys
from pathlib import Path

repo = Path(r"D:\agent\booming-il2cpp")
obj_dir = repo / "verification" / "foundation-dll" / "System.Private.CoreLib" / "convert-char" / "native_test" / "benchmark" / "build"
exe_path = obj_dir / "benchmark_convert-char.exe"

# Find link.exe
base = Path(r"C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Tools\MSVC")
versions = sorted([d for d in base.iterdir() if d.is_dir() and d.name[0].isdigit()])
link = versions[-1] / "bin" / "Hostx64" / "x64" / "link.exe"

# Find vcvars
vcvars = Path(r"C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvarsall.bat")

# Capture env from vcvars
r = subprocess.run(f'"{vcvars}" x64 >nul && set', shell=True, capture_output=True, text=True, timeout=30)
env = os.environ.copy()
for line in r.stdout.splitlines():
    if "=" in line:
        k, v = line.split("=", 1)
        env[k.upper()] = v

# Link
objs = [str(obj_dir / o) for o in ["native_aot_main.obj", "native-aot.generated.obj", "runtime_stubs.obj"]]
cmd = [str(link), "/nologo", f"/out:{exe_path}"] + objs
print(f"Linking: {cmd}")
r2 = subprocess.run(cmd, capture_output=True, text=True, timeout=60, env=env)
print(f"RC={r2.returncode}")
print(f"STDOUT: {r2.stdout}")
print(f"STDERR: {r2.stderr}")
if exe_path.exists():
    print(f"EXE created: {exe_path.stat().st_size} bytes")
