"""Minimal smoke test: link a trivial main with the same libs."""
import subprocess, os

REPO = r"D:\agent\booming-il2cpp"
BUILD = os.path.join(REPO, "verification", "foundation-dll", "System.Private.CoreLib",
                     "convert-char", "native_test", "l2-verify", "build")
VCVARS = r'C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars64.bat'

INCS = " ".join(f'-I"{d}"' for d in [
    os.path.join(REPO, "src", "native", "common"),
    os.path.join(REPO, "src", "native", "common", "chaos"),
    os.path.join(REPO, "contracts", "native", "v0"),
    os.path.join(REPO, "src", "native", "runtime-core"),
    os.path.join(REPO, "src", "native", "interpreter"),
    os.path.join(REPO, "src", "native", "bootstrap"),
    os.path.join(REPO, "third_party", "bdwgc", "include"),
    os.path.join(REPO, "third_party", "fmt", "include"),
    BUILD])
DEFS = "-DCHAOS_IL2CPP_CHECK -DCHAOS_IL2CPP_TRACE_ENABLED -DCHAOS_IL2CPP_VERIFY_MODE -DCHAOS_RUNTIME_ABI_STATIC -DGC_NOT_DLL"
FLAGS = "/nologo /std:c++20 /c /EHsc /W3 /utf-8 /Od /MD"

def run(cmd):
    full = f'call "{VCVARS}" x64 -vcvars_ver=14.42 >nul 2>nul && {cmd}'
    r = subprocess.run(full, shell=True, capture_output=True, text=True, timeout=120)
    return r

# Test A: Trivial main(), link with ZERO libraries
print("=== TEST A: Trivial main, NO libs ===")
src_a = os.path.join(BUILD, "sm_a.cpp")
with open(src_a, "w") as f:
    f.write("int main() { return 0; }\n")
obj_a = os.path.join(BUILD, "sm_a.obj")
r = run(f'cl {FLAGS} -Fo"{obj_a}" "{src_a}"')
print(f"  Compile RC={r.returncode}")
exe_a = os.path.join(BUILD, "sm_a.exe")
r = run(f'link /nologo /out:"{exe_a}" "{obj_a}"')
print(f"  Link RC={r.returncode}")
r = run(f'"{exe_a}"')
print(f"  Run RC={r.returncode}")

# Test B: Trivial main(), link with ALL libs (just the libs, no generated code)
print("\n=== TEST B: Trivial main, ALL libs ===")
NL = os.path.join(REPO, "build", "native", "src", "native")
BDWGC = os.path.join(REPO, "build", "native", "bdwgc_build")
REL = "RelWithDebInfo"
LINKLIBS = " ".join(
    [f'"{os.path.join(NL, "runtime-core", REL, "chaos_runtime_core.lib")}"'] +
    [f'"{os.path.join(NL, lib, REL, f"chaos_{name}.lib")}"'
     for lib, name in [("bootstrap","bootstrap"),("interpreter","interpreter"),
                       ("common","common"),("support","support"),("hot-update","hot_update")]] +
    [f'"{os.path.join(REPO, "build", "native", "fmt_build", REL, "chaos_fmt.lib")}"',
     f'"{os.path.join(BDWGC, REL, "chaos_bdwgc.lib")}"',
     "ole32.lib", "user32.lib"]
)
exe_b = os.path.join(BUILD, "sm_b.exe")
r = run(f'link /nologo /out:"{exe_b}" "{obj_a}" {LINKLIBS}')
print(f"  Link RC={r.returncode}")
if r.returncode == 0:
    r = run(f'"{exe_b}"')
    print(f"  Run RC={r.returncode}")
else:
    for l in (r.stdout + r.stderr).splitlines():
        if "error" in l.lower():
            print(f"  {l}")

# Test C: main() with #include <chaos/common.h>, link with chaos_common.lib
print("\n=== TEST C: include <chaos/common.h>, link ALL libs ===")
src_c = os.path.join(BUILD, "sm_c.cpp")
with open(src_c, "w") as f:
    f.write('#include <chaos/common.h>\nint main() { CHAOS_IL2CPP_LOG_INFO_M("T", "hello {0}", 42); CHAOS_IL2CPP_LOG_FLUSH(); return 0; }\n')
obj_c = os.path.join(BUILD, "sm_c.obj")
r = run(f'cl {FLAGS} {INCS} {DEFS} -Fo"{obj_c}" "{src_c}"')
print(f"  Compile RC={r.returncode}")
if r.returncode == 0:
    exe_c = os.path.join(BUILD, "sm_c.exe")
    r = run(f'link /nologo /out:"{exe_c}" "{obj_c}" {LINKLIBS}')
    print(f"  Link RC={r.returncode}")
    if r.returncode == 0:
        r = run(f'"{exe_c}"')
        print(f"  Run RC={r.returncode}")
        out = (r.stdout + r.stderr).strip()
        print(f"  Output: {out[:200]}")

# Test D: Minimal printf main, link WITH chaos_runtime_core.lib + all deps
print("\n=== TEST D: printf main, ALL libs (includes chaos_runtime_core) ===")
# Use the same gen_obj but with just a stub - maybe the gen_obj symbols are needed
src_d = os.path.join(BUILD, "sm_d.cpp")
with open(src_d, "w") as f:
    f.write('#include <cstdio>\n#include <csetjmp>\nint main() { printf("hello\\n"); return 0; }\n')
obj_d = os.path.join(BUILD, "sm_d.obj")
r = run(f'cl {FLAGS} -Fo"{obj_d}" "{src_d}"')
print(f"  Compile RC={r.returncode}")
exe_d = os.path.join(BUILD, "sm_d.exe")
r = run(f'link /nologo /out:"{exe_d}" "{obj_d}" {LINKLIBS}')
print(f"  Link RC={r.returncode}")
if r.returncode == 0:
    r = run(f'"{exe_d}"')
    print(f"  Run RC={r.returncode}")
