"""Rebuild fact_static_test.exe and run it."""
import subprocess, sys
from pathlib import Path

VCVARS = r"C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvarsall.bat"
REPO_ROOT = Path(r"D:\agent\booming-il2cpp")
BUILD_DIR = REPO_ROOT / "verification" / "foundation-dll" / "System.Private.CoreLib" / "convert-char" / "native_test" / "fact-verify" / "build"
NATIVE_LIB = REPO_ROOT / "build" / "native"
CONFIG = "RelWithDebInfo"

# ── Get MSVC env ──────────────────────────────────────────────────────
result = subprocess.run(f'"{VCVARS}" x64 -vcvars_ver=14.42 && set', shell=True, capture_output=True, text=True, timeout=30)
msvc_env = {}
for line in result.stdout.splitlines():
    if "=" in line:
        k, v = line.split("=", 1)
        msvc_env[k.upper()] = v

# ── Find cl/link ─────────────────────────────────────────────────────
MSVC_BIN = Path(r"C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Tools\MSVC")
versions = sorted([d for d in MSVC_BIN.iterdir() if d.is_dir() and d.name[0].isdigit()])
TOOLCHAIN = versions[-1] / "bin" / "Hostx64" / "x64"
cl_exe = TOOLCHAIN / "cl.exe"
link_exe = TOOLCHAIN / "link.exe"

# ── Includes & Defines ────────────────────────────────────────────────
INC = " ".join(f'-I"{d}"' for d in [
    REPO_ROOT / "src" / "native" / "common",
    REPO_ROOT / "src" / "native" / "common" / "chaos",
    REPO_ROOT / "contracts" / "native" / "v0",
    REPO_ROOT / "src" / "native" / "runtime-core",
    REPO_ROOT / "src" / "native" / "runtime-core" / "gc",
    REPO_ROOT / "src" / "native" / "bootstrap",
    REPO_ROOT / "src" / "native" / "interpreter",
    REPO_ROOT / "src" / "native" / "interpreter" / "generated",
    REPO_ROOT / "src" / "native",
    REPO_ROOT / "third_party" / "fmt" / "include",
    REPO_ROOT / "third_party" / "bdwgc" / "include",
    BUILD_DIR,
])
DEFS = "-DCHAOS_IL2CPP_CONFIG_CHECK -DCHAOS_IL2CPP_VERIFY_MODE -DGC_NOT_DLL"

LIBS = " ".join(f'"{NATIVE_LIB / p}"' for p in [
    "src/native/runtime-core/RelWithDebInfo/chaos_runtime_core.lib",
    "src/native/interpreter/RelWithDebInfo/chaos_interpreter.lib",
    "src/native/bootstrap/RelWithDebInfo/chaos_bootstrap.lib",
    "src/native/common/RelWithDebInfo/chaos_common.lib",
    "src/native/support/RelWithDebInfo/chaos_support.lib",
    "src/native/hot-update/RelWithDebInfo/chaos_hot_update.lib",
    "fmt_build/RelWithDebInfo/chaos_fmt.lib",
    "bdwgc_build/RelWithDebInfo/chaos_bdwgc.lib",
])

SRC = BUILD_DIR / "fact_verify_combined.cpp"
EXE = BUILD_DIR / "fact_static_test.exe"

# ── Compile + Link in one step (like the bat does) ────────────────────
CFLAGS = "/nologo /std:c++20 /EHsc /W3 /utf-8 /O2 /MD"
cmd = f'"{cl_exe}" {CFLAGS} {INC} {DEFS} -I"{BUILD_DIR}" -Fe"{EXE}" "{SRC}" {LIBS} ole32.lib user32.lib'
print("Compiling + Linking...")
r1 = subprocess.run(cmd, shell=True, capture_output=True, text=True, timeout=120, env=msvc_env)
for l in r1.stdout.splitlines():
    if "error" in l.lower() or "warning" in l.lower() or "LNK" in l:
        print(f"  {l}")
if r1.returncode != 0:
    print("BUILD FAILED")
    sys.exit(1)
print("  Build OK")

# ── Run ───────────────────────────────────────────────────────────────
print("\nRunning fact_static_test.exe...")
r2 = subprocess.run(str(EXE), capture_output=True, text=True, timeout=60)
print(r2.stdout)
if r2.stderr:
    print(f"stderr: {r2.stderr[:200]}")
print(f"RC={r2.returncode}")
sys.exit(r2.returncode)