"""Test mixed optimization levels and target specific sections."""
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

def build_and_run(label, gen_code, gen_flags=None, host_flags=None):
    if gen_flags is None:
        gen_flags = "/nologo /std:c++20 /c /EHsc /W3 /utf-8 /Od /MD"
    if host_flags is None:
        host_flags = gen_flags

    gen_path = os.path.join(BUILD, f"bs7_{label}.cpp")
    with open(gen_path, "w") as f:
        f.write(gen_code)
    gen_obj = os.path.join(BUILD, f"bs7_{label}.obj")
    r1 = run(f'cl {gen_flags} {INCS} {DEFS} -Fo"{gen_obj}" "{gen_path}"')
    if r1.returncode != 0:
        errs = [l for l in (r1.stdout + r1.stderr).splitlines() if "error" in l.lower()]
        return f"{label}: COMPILE FAIL: {errs[-1] if errs else '?'}"

    host_path = os.path.join(BUILD, "bs7_host.cpp")
    with open(host_path, "w") as f:
        f.write(HOST)
    host_obj = os.path.join(BUILD, "bs7_host.obj")
    r2 = run(f'cl {host_flags} {INCS} {DEFS} -Fo"{host_obj}" "{host_path}"')
    if r2.returncode != 0:
        return f"{label}: HOST COMPILE FAIL"

    exe = os.path.join(BUILD, f"bs7_{label}.exe")
    r3 = run(f'link /nologo /out:"{exe}" "{host_obj}" "{gen_obj}" {LINKLIBS}')
    if r3.returncode != 0:
        return f"{label}: LINK FAILED"

    r4 = run(f'"{exe}"')
    out = (r4.stdout + r4.stderr).strip()
    return f"{label}: RC={r4.returncode} out={out[:800]}"

O2_FLAGS = "/nologo /std:c++20 /c /EHsc /W3 /utf-8 /O2 /MD"

with open(GEN, encoding="utf-8-sig") as f:
    original = f.read()

# Test A: gen compiled /Od, host compiled /O2
print("=== Test A: gen=/Od, host=/O2 ===")
r = build_and_run("gen_od_host_o2", original, gen_flags=None, host_flags=O2_FLAGS)
print(f"  {r}")

# Test B: gen compiled /O2, host compiled /Od
print("\n=== Test B: gen=/O2, host=/Od ===")
r = build_and_run("gen_o2_host_od", original, gen_flags=O2_FLAGS, host_flags=None)
print(f"  {r}")

# Test C: add /RTC1 to /Od for both (run-time checks)
print("\n=== Test C: /Od + /RTC1 for both ===")
RTC_FLAGS = "/nologo /std:c++20 /c /EHsc /W3 /utf-8 /Od /MD /RTC1"
r = build_and_run("gen_rtc", original, gen_flags=RTC_FLAGS)
print(f"  {r}")

# Test D: add /GS- to /Od to see if it's a security cookie issue
print("\n=== Test D: /Od with /GS- (no buffer security check) ===")
GSC_FLAGS = "/nologo /std:c++20 /c /EHsc /W3 /utf-8 /Od /MD /GS-"
r = build_and_run("gen_gs", original, gen_flags=GSC_FLAGS)
print(f"  {r}")
