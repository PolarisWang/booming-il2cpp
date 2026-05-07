"""Narrow down the exact /Od sub-flag causing the crash."""
import subprocess, os

REPO = r"D:\agent\booming-il2cpp"
BUILD = os.path.join(REPO, "verification", "foundation-dll", "System.Private.CoreLib",
                     "convert-char", "native_test", "l2-verify", "build")
GEN = os.path.join(REPO, "verification", "foundation-dll", "System.Private.CoreLib",
                   "convert-char", "il2cpp_dist", "genuine",
                   "ConvertCharNativeEntry", "generated", "native-aot.generated.cpp")
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

HOST = """#include <cstdio>
#include <csetjmp>
extern "C" int __chaos_assert_failures;
extern "C" void SetExceptionFallback(void (*fn)());
extern "C" int RunNativeAot(int);
#include "expected_checksums.h"
static jmp_buf buf;
static void fb() { longjmp(buf, 1); }
int main() {
    printf("main_started\\n"); fflush(stdout);
    __chaos_assert_failures = 0;
    SetExceptionFallback(fb);
    int rf = 0;
    for (int i = 0; i < kExpectedCount; i++) {
        printf("[%d] call\\n", i); fflush(stdout);
        int a = 0; bool t = false;
        if (setjmp(buf) == 0) { a = RunNativeAot(i); } else { t = true; }
        if (kExpectedChecksums[i] == -1) continue;
        if (t) { printf("FAIL[%d]:threw %d\\n", i, kExpectedChecksums[i]); rf++; continue; }
        if (a != kExpectedChecksums[i]) { printf("FAIL[%d]:exp %d got %d\\n", i, kExpectedChecksums[i], a); rf++; }
    }
    SetExceptionFallback(nullptr);
    printf("L2: %d/%d passed\\n", kExpectedCount-rf, kExpectedCount);
    fflush(stdout);
    return __chaos_assert_failures + rf;
}
"""

def run(cmd):
    full = f'call "{VCVARS}" x64 -vcvars_ver=14.42 >nul 2>nul && {cmd}'
    r = subprocess.run(full, shell=True, capture_output=True, text=True, timeout=120)
    return r

def build_and_run(label, gen_code, gen_flags):
    gen_path = os.path.join(BUILD, f"bs9_{label}.cpp")
    with open(gen_path, "w") as f:
        f.write(gen_code)
    gen_obj = os.path.join(BUILD, f"bs9_{label}.obj")
    r1 = run(f'cl {gen_flags} {INCS} {DEFS} -Fo"{gen_obj}" "{gen_path}"')
    if r1.returncode != 0:
        errs = [l for l in (r1.stdout + r1.stderr).splitlines() if "error" in l.lower()]
        return f"{label}: COMPILE FAIL: {errs[-1] if errs else '?'}"

    host_path = os.path.join(BUILD, "bs9_host.cpp")
    with open(host_path, "w") as f:
        f.write(HOST)
    host_obj = os.path.join(BUILD, "bs9_host.obj")
    r2 = run(f'cl /nologo /std:c++20 /c /EHsc /W3 /utf-8 /Od /MD {INCS} {DEFS} -Fo"{host_obj}" "{host_path}"')
    if r2.returncode != 0:
        return f"{label}: HOST COMPILE FAIL"

    exe = os.path.join(BUILD, f"bs9_{label}.exe")
    r3 = run(f'link /nologo /out:"{exe}" "{host_obj}" "{gen_obj}" {LINKLIBS}')
    if r3.returncode != 0:
        return f"{label}: LINK FAILED"

    r4 = run(f'"{exe}"')
    out = (r4.stdout + r4.stderr).strip()
    return f"{label}: RC={r4.returncode} out={out[:800]}"

BASE_OD = "/nologo /std:c++20 /c /EHsc /W3 /utf-8 /Od /MD"
O1 = "/nologo /std:c++20 /c /EHsc /W3 /utf-8 /O1 /MD"
O2 = "/nologo /std:c++20 /c /EHsc /W3 /utf-8 /O2 /MD"

with open(GEN, encoding="utf-8-sig") as f:
    original = f.read()

# Test A: /Od + /Oi (enable intrinsic functions)
print("=== A: /Od + /Oi ===")
r = build_and_run("od_oi", original, BASE_OD + " /Oi")
print(f"  {r}")

# Test B: Favor size (like /O1) but no global optimizations
print("\n=== B: /Od + /Os + /Ob2 + /Oi ===")
r = build_and_run("od_os_ob2_oi", original, BASE_OD + " /Os /Ob2 /Oi")
print(f"  {r}")

# Test C: /Od + /Og (global opts)
print("\n=== C: /Od + /Og ===")
r = build_and_run("od_og", original, BASE_OD + " /Og")
print(f"  {r}")

# Test D: /Od but /O2 for generated code only via pragma (not possible via CLI)
# Instead: try -d2ReportInlinedCode or any debug flag that differs

# Test E: key test - /O2 but use /MDd (debug CRT)
print("\n=== E: /O2 + /MDd (debug CRT) ===")
O2_MDd = "/nologo /std:c++20 /c /EHsc /W3 /utf-8 /O2 /MDd"
r = build_and_run("o2_mdd", original, O2_MDd)
print(f"  {r}")

# Test F: /Od but compile as C++20 with different struct packing
print("\n=== F: /Od + /Zp1 (byte-align structs) ===")
r = build_and_run("od_zp1", original, BASE_OD + " /Zp1")
print(f"  {r}")

# Test G: /O2 + /Ob0 + /Oi (to check if /Oi alone with no inlining saves us)
print("\n=== G: /O2 + /Ob0 + /Oi ===")
r = build_and_run("o2_ob0_oi", original, "/nologo /std:c++20 /c /EHsc /W3 /utf-8 /O2 /Ob0 /Oi /MD")
print(f"  {r}")

# Test H: /O2 only /Ob1 (inline only __inline/__forceinline/inline, no auto-inlining)
print("\n=== H: /O2 + /Ob1 (inline only 'inline' marked) ===")
r = build_and_run("o2_ob1", original, "/nologo /std:c++20 /c /EHsc /W3 /utf-8 /O2 /Ob1 /MD")
print(f"  {r}")

# Test I: let's see what the old .obj was compiled with
# It might have been /O2
print("\n=== I: reference - /O2 baseline ===")
r = build_and_run("ref_o2", original, O2)
print(f"  {r}")
