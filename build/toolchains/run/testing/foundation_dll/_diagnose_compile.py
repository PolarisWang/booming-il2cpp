"""Diagnose why the native-aot.generated.cpp compile produces different .obj files."""
from __future__ import annotations

import subprocess
import os
import sys
import hashlib
from pathlib import Path

REPO_ROOT = Path("D:/agent/booming-il2cpp")
FAMILY = "convert-char"
FAMILY_DIR = REPO_ROOT / "verification" / "foundation-dll" / "System.Private.CoreLib" / FAMILY
GENERATED_CPP = FAMILY_DIR / "il2cpp_dist" / "genuine" / "generated" / "native-aot.generated.cpp"
VERIFY_DIR = FAMILY_DIR / "native_test" / "l2-verify" / "build"
BUILD_DIR = FAMILY_DIR / "native_test" / "benchmark" / "build"

# 1. Check that the source file hasn't changed
cpp_hash = hashlib.md5(GENERATED_CPP.read_bytes()).hexdigest()
print(f"Generated C++ file: {GENERATED_CPP}")
print(f"  MD5: {cpp_hash}")
print(f"  Size: {GENERATED_CPP.stat().st_size}")
print()

# 2. Check the verify's .obj
verify_obj = VERIFY_DIR / "native-aot.generated.obj"
print(f"Verify .obj: {verify_obj}")
print(f"  Size: {verify_obj.stat().st_size} (exists: {verify_obj.exists()})")
print()

# 3. Find MSVC and compiler versions
vcvars_paths = [
    Path("C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Auxiliary/Build/vcvarsall.bat"),
    Path("C:/Program Files/Microsoft Visual Studio/2022/Professional/VC/Auxiliary/Build/vcvarsall.bat"),
]
vcvars = next((p for p in vcvars_paths if p.exists()), None)
print(f"vcvarsall: {vcvars}")

result = subprocess.run(f'"{vcvars}" x64 && cl --version 2>&1', shell=True, capture_output=True, text=True, timeout=30)
print(f"CL version:\n{result.stdout[:200]}")
print()

# 4. Capture MSVC env variables
result = subprocess.run(f'"{vcvars}" x64 && set', shell=True, capture_output=True, text=True, timeout=30)
msvc_env = {}
for line in result.stdout.splitlines():
    if "=" in line:
        k, v = line.split("=", 1)
        msvc_env[k.upper()] = v

# Check critical env vars
for key in ["INCLUDE", "LIB", "LIBPATH", "PATH", "VCTOOLSINSTALLDIR", "WINDOWSSDKDIR", "UCRTVERSION"]:
    if key in msvc_env:
        val = msvc_env[key][:200]
        print(f"  {key} = {val}...")
    else:
        print(f"  {key} = (not set)")
print()

# 5. Check if any include resolves differently
# Add /showIncludes to see what headers are included
include_dirs = [
    REPO_ROOT / "src" / "native" / "common",
    REPO_ROOT / "src" / "native" / "common" / "chaos",
    REPO_ROOT / "contracts" / "native" / "v0",
    REPO_ROOT / "src" / "native" / "runtime-core",
    REPO_ROOT / "third_party" / "fmt" / "include",
    VERIFY_DIR,
]
include_flags = " ".join(f'-I"{d}"' for d in include_dirs)
compile_flags = "/nologo /std:c++20 /c /EHsc /W3 /utf-8 /O2 /showIncludes"
defines = "-DCHAOS_IL2CPP_CHECK"

# Compile JUST the first 200 lines of the generated file to check includes
partial_src = BUILD_DIR / "_diagnose_includes.cpp"
gen_lines = GENERATED_CPP.read_text(encoding="utf-8").splitlines()
partial_src.write_text("\n".join(gen_lines[:100]) + "\n", encoding="utf-8")

base = Path("C:/Program Files/Microsoft Visual Studio/2022/Professional/VC/Tools/MSVC")
if not base.exists():
    base = Path("C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC")
versions = sorted([d for d in base.iterdir() if d.is_dir() and d.name[0].isdigit()])
cl_exe = versions[-1] / "bin" / "Hostx64" / "x64" / "cl.exe"
link_exe = cl_exe.parent / "link.exe"

print("=== /showIncludes output (first 50 lines) ===")
cmd = f'"{cl_exe}" {compile_flags} {include_flags} {defines} -Fo"{BUILD_DIR / "_diagnose_includes.obj"}" "{partial_src}"'
r = subprocess.run(cmd, shell=True, capture_output=True, text=True, timeout=30, env=msvc_env)
# Filter and show includes
for line in r.stdout.splitlines():
    if "Note: including file:" in line:
        print(f"  {line.strip()}")
    elif "error" in line.lower() or "fatal" in line.lower():
        print(f"  ERROR: {line.strip()}")
PYEOF
