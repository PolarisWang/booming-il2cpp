"""Fine-grained bisect: which specific /Od vs /O2 difference causes the crash."""
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
    gen_path = os.path.join(BUILD, f"bs8_{label}.cpp")
    with open(gen_path, "w") as f:
        f.write(gen_code)
    gen_obj = os.path.join(BUILD, f"bs8_{label}.obj")
    r1 = run(f'cl {gen_flags} {INCS} {DEFS} -Fo"{gen_obj}" "{gen_path}"')
    if r1.returncode != 0:
        errs = [l for l in (r1.stdout + r1.stderr).splitlines() if "error" in l.lower()]
        return f"{label}: COMPILE FAIL: {errs[-1] if errs else '?'}"

    host_path = os.path.join(BUILD, "bs8_host.cpp")
    with open(host_path, "w") as f:
        f.write(HOST)
    host_obj = os.path.join(BUILD, "bs8_host.obj")
    r2 = run(f'cl /nologo /std:c++20 /c /EHsc /W3 /utf-8 /Od /MD {INCS} {DEFS} -Fo"{host_obj}" "{host_path}"')
    if r2.returncode != 0:
        return f"{label}: HOST COMPILE FAIL"

    exe = os.path.join(BUILD, f"bs8_{label}.exe")
    r3 = run(f'link /nologo /out:"{exe}" "{host_obj}" "{gen_obj}" {LINKLIBS}')
    if r3.returncode != 0:
        return f"{label}: LINK FAILED"

    r4 = run(f'"{exe}"')
    out = (r4.stdout + r4.stderr).strip()
    return f"{label}: RC={r4.returncode} out={out[:800]}"

BASE_OD = "/nologo /std:c++20 /c /EHsc /W3 /utf-8 /Od /MD"
BASE_O2 = "/nologo /std:c++20 /c /EHsc /W3 /utf-8 /O2 /MD"

with open(GEN, encoding="utf-8-sig") as f:
    original = f.read()

# Test A: base /Od (baseline — should crash)
print("=== A: /Od baseline (should crash) ===")
r = build_and_run("a_od", original, BASE_OD)
print(f"  {r}")

# Test B: /Od + /Ob2 (enable inlining only)
print("\n=== B: /Od + /Ob2 (enable inline expansion) ===")
r = build_and_run("b_od_ob2", original, "/nologo /std:c++20 /c /EHsc /W3 /utf-8 /Od /MD /Ob2")
print(f"  {r}")

# Test C: /Od + /Oy- (no frame pointer omission — tests stack frame issue)
print("\n=== C: /Od + /Oy- ===")
r = build_and_run("c_od_oy", original, "/nologo /std:c++20 /c /EHsc /W3 /utf-8 /Od /MD /Oy-")
print(f"  {r}")

# Test D: just /O1 (minimize size — enables some optimizations)
print("\n=== D: /O1 ===")
r = build_and_run("d_o1", original, "/nologo /std:c++20 /c /EHsc /W3 /utf-8 /O1 /MD")
print(f"  {r}")

# Test E: /Od but with /Z7 (debug info like /O2 default) — check if debug info matters
print("\n=== E: /Od + /Z7 ===")
r = build_and_run("e_od_z7", original, "/nologo /std:c++20 /c /EHsc /W3 /utf-8 /Od /MD /Z7")
print(f"  {r}")

# Test F: /O2 but add /Ob0 (disable inlining)
print("\n=== F: /O2 + /Ob0 (disable inlining) ===")
r = build_and_run("f_o2_ob0", original, "/nologo /std:c++20 /c /EHsc /W3 /utf-8 /O2 /MD /Ob0")
print(f"  {r}")

# Test G: /O2 but add /Od- (can't really do this, but try /O2 with /RTC1)
print("\n=== G: /O2 + /RTC1 ===")
r = build_and_run("g_o2_rtc", original, "/nologo /std:c++20 /c /EHsc /W3 /utf-8 /O2 /MD /RTC1")
print(f"  {r}")
