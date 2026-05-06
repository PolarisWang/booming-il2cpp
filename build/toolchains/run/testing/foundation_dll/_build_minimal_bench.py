"""Build a minimal benchmark exe using the verify-build approach."""
import subprocess, sys, os, json
from pathlib import Path

REPO_ROOT = Path("D:/agent/booming-il2cpp")
ASSEMBLY = "System.Private.CoreLib"
FAMILY = "convert-char"

family_dir = REPO_ROOT / "verification" / "foundation-dll" / ASSEMBLY / FAMILY
generated_cpp = family_dir / "il2cpp_dist" / "genuine" / "generated" / "native-aot.generated.cpp"
build_dir = family_dir / "native_test" / "benchmark" / "build"
build_dir.mkdir(parents=True, exist_ok=True)

# Write minimal main
main_cpp = build_dir / "bench_main.cpp"
main_cpp.write_text(
    '#include <cstdio>\n'
    '#include <chrono>\n'
    'extern "C" int __chaos_assert_failures;\n'
    'extern "C" int RunNativeAot(int entryIndex);\n'
    'int main() {\n'
    '    __chaos_assert_failures = 0;\n'
    '    printf("{\\"results\\":[");\n'
    '    for (int i = 0; i < 18; i++) {\n'
    '        auto t0 = std::chrono::steady_clock::now();\n'
    '        int result = RunNativeAot(i);\n'
    '        auto t1 = std::chrono::steady_clock::now();\n'
    '        double ms = std::chrono::duration<double, std::milli>(t1 - t0).count();\n'
    '        if (i > 0) printf(",");\n'
    '        printf("{\\"method\\":%d,\\"ms\\":%f,\\"ret\\":%d}", i, ms, result);\n'
    '    }\n'
    '    printf("],\\"assert_failures\\":%d}", __chaos_assert_failures);\n'
    '    return 0;\n'
    '}\n',
    encoding="utf-8"
)
print(f"Wrote {main_cpp}")

# Find MSVC
candidates = [
    Path("C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Auxiliary/Build/vcvarsall.bat"),
    Path("C:/Program Files/Microsoft Visual Studio/2022/Professional/VC/Auxiliary/Build/vcvarsall.bat"),
]
vcvars = next((c for c in candidates if c.exists()), None)
if not vcvars:
    print("MSVC not found")
    sys.exit(1)

# Get MSVC env
result = subprocess.run(f'"{vcvars}" x64 && set', shell=True, capture_output=True, text=True, timeout=30)
msvc_env = {}
for line in result.stdout.splitlines():
    if "=" in line:
        k, v = line.split("=", 1)
        msvc_env[k.upper()] = v

# Find cl.exe
base = Path("C:/Program Files/Microsoft Visual Studio/2022/Professional/VC/Tools/MSVC")
if not base.exists():
    base = Path("C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC")
versions = sorted([d for d in base.iterdir() if d.is_dir() and d.name[0].isdigit()])
cl_exe = versions[-1] / "bin" / "Hostx64" / "x64" / "cl.exe"
link_exe = cl_exe.parent / "link.exe"

include_dirs = [
    REPO_ROOT / "src" / "native" / "common",
    REPO_ROOT / "src" / "native" / "common" / "chaos",
    REPO_ROOT / "contracts" / "native" / "v0",
    REPO_ROOT / "src" / "native" / "runtime-core",
    REPO_ROOT / "third_party" / "fmt" / "include",
    family_dir / "native_test" / "l2-verify" / "build",
]
include_flags = " ".join(f'-I"{d}"' for d in include_dirs)
chaos_lib = REPO_ROOT / "build" / "native-runtime" / "Release" / "chaos_runtime_core.lib"

compile_flags = "/nologo /std:c++20 /c /EHsc /W3 /utf-8 /O2"
defines = "-DCHAOS_IL2CPP_CHECK"
exe_path = build_dir / "bench_minimal.exe"

# Compile bench_main
obj1 = build_dir / "bench_main.obj"
cmd1 = f'"{cl_exe}" {compile_flags} {include_flags} {defines} -Fo"{obj1}" "{main_cpp}"'
print("Compiling bench_main.cpp...")
r1 = subprocess.run(cmd1, shell=True, capture_output=True, text=True, timeout=120, env={**os.environ, **msvc_env})
if r1.returncode != 0:
    err = [l for l in (r1.stdout + r1.stderr).splitlines() if l.strip() and "Microsoft" not in l]
    print("FAILED:")
    for e in err[-5:]: print(f"  {e}")
    sys.exit(1)
print("  OK")

# Compile native-aot.generated
gen_obj = build_dir / "native-aot.generated.obj"
if not gen_obj.exists():
    cmd2 = f'"{cl_exe}" {compile_flags} {include_flags} {defines} -Fo"{gen_obj}" "{generated_cpp}"'
    print("Compiling native-aot.generated.cpp...")
    r2 = subprocess.run(cmd2, shell=True, capture_output=True, text=True, timeout=300, env={**os.environ, **msvc_env})
    if r2.returncode != 0:
        err = [l for l in (r2.stdout + r2.stderr).splitlines() if l.strip() and "Microsoft" not in l]
        print("FAILED:")
        for e in err[-10:]: print(f"  {e}")
        sys.exit(1)
    print("  OK")
else:
    print("  (using cached .obj)")

# Link
link_cmd = f'"{link_exe}" /nologo /out:"{exe_path}" "{obj1}" "{gen_obj}" "{chaos_lib}" ole32.lib'
print("Linking...")
r3 = subprocess.run(link_cmd, shell=True, capture_output=True, text=True, timeout=60, env={**os.environ, **msvc_env})
if r3.returncode != 0:
    err = [l for l in r3.stderr.splitlines() if l.strip() and "Microsoft" not in l]
    print("FAILED:")
    for e in err[-5:]: print(f"  {e}")
    sys.exit(1)
print("  OK")

# Run
print(f"Running {exe_path}...")
r4 = subprocess.run([str(exe_path)], capture_output=True, text=True, timeout=60)
print(f"Return code: {r4.returncode}")
print(f"Stdout ({len(r4.stdout)} bytes):")
print(r4.stdout[:2000])
if r4.stderr:
    print(f"Stderr: {r4.stderr[:500]}")
