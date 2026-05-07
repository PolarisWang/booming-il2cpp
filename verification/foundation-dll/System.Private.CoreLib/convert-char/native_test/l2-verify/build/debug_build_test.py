"""Build debug test for L2 crash debugging."""
import subprocess
import sys

REPO = r"D:\agent\booming-il2cpp"
BUILD = REPO + r"\verification\foundation-dll\System.Private.CoreLib\convert-char\native_test\l2-verify\build"
GEN = REPO + r"\verification\foundation-dll\System.Private.CoreLib\convert-char\il2cpp_dist\genuine\ConvertCharNativeEntry\generated\native-aot.generated.cpp"
VERIFY_HOST = REPO + r"\src\native\benchmark-host\native_verify_main.cpp"
VCVARS = r'C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars64.bat'
PREFIX = f'"{VCVARS}" x64 -vcvars_ver=14.42 >nul 2>nul &&'

INCS = " ".join(f'-I"{d}"' for d in [
    REPO + r"\src\native\common",
    REPO + r"\src\native\common\chaos",
    REPO + r"\contracts\native\v0",
    REPO + r"\src\native\runtime-core",
    REPO + r"\src\native\interpreter",
    REPO + r"\src\native\bootstrap",
    REPO + r"\third_party\bdwgc\include",
    REPO + r"\third_party\fmt\include",
    BUILD,
])
DEFS = "-DCHAOS_IL2CPP_CHECK -DCHAOS_IL2CPP_TRACE_ENABLED -DCHAOS_IL2CPP_VERIFY_MODE -DCHAOS_RUNTIME_ABI_STATIC -DGC_NOT_DLL"
FLAGS = "/nologo /std:c++20 /c /EHsc /W3 /utf-8 /Od /MD"

NL = REPO + r"\build\native\src\native"
BDWGC = REPO + r"\build\native\bdwgc_build"
LINKLIBS = " ".join([
    f'"{NL}\\runtime-core\\Release\\chaos_runtime_core.lib"',
    f'"{NL}\\bootstrap\\Release\\chaos_bootstrap.lib"',
    f'"{NL}\\interpreter\\Release\\chaos_interpreter.lib"',
    f'"{NL}\\common\\Release\\chaos_common.lib"',
    f'"{NL}\\support\\Release\\chaos_support.lib"',
    f'"{NL}\\hot-update\\Release\\chaos_hot_update.lib"',
    f'"{REPO}\\build\\native\\fmt_build\\Release\\chaos_fmt.lib"',
    f'"{BDWGC}\\Release\\chaos_bdwgc.lib"',
    "ole32.lib",
    "user32.lib",
])

def run(cmd):
    """Run command under vcvars env."""
    full = PREFIX + cmd
    return subprocess.run(full, shell=True, capture_output=True, timeout=120)

# Clean up stale .obj files
import os as _os
for f in ["t_gen.obj", "t_verify.obj", "t_crash_test.exe"]:
    p = _os.path.join(BUILD, f)
    if _os.path.exists(p):
        _os.remove(p)

# 1. Compile generated code
r = run(f'cl {FLAGS} {INCS} {DEFS} -Fo"{BUILD}\\t_gen.obj" "{GEN}"')
print(f"Compile gen: rc={r.returncode}")
if r.returncode != 0:
    for line in (r.stdout + r.stderr).splitlines():
        if "error" in line.lower():
            print(f"  {line}")
    sys.exit(1)

# 2. Compile verify host
r = run(f'cl {FLAGS} {INCS} {DEFS} -Fo"{BUILD}\\t_verify.obj" "{VERIFY_HOST}"')
print(f"Compile verify: rc={r.returncode}")
if r.returncode != 0:
    for line in (r.stdout + r.stderr).splitlines():
        if "error" in line.lower():
            print(f"  {line}")
    sys.exit(1)

# 3. Link
r = run(f'link /nologo /out:"{BUILD}\\t_crash_test.exe" "{BUILD}\\t_verify.obj" "{BUILD}\\t_gen.obj" {LINKLIBS}')
print(f"Link: rc={r.returncode}")
if r.returncode != 0:
    for line in (r.stdout + r.stderr).splitlines():
        if "error" in line.lower() or "fatal" in line.lower():
            print(f"  {line}")
    sys.exit(1)

# 4. Run
r = run(f'"{BUILD}\\t_crash_test.exe"')
print(f"Run: rc={r.returncode}")
out = (r.stdout + r.stderr).decode("utf-8", errors="replace")
print(f"Output ({len(out)} bytes):")
print(out[:2000])
