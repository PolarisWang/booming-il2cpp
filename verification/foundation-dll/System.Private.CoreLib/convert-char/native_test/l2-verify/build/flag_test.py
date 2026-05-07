"""Test specific /Od sub-flags to find what /O1 does differently that /Od doesn't."""
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
LINKLIBS = " ".join([
    f'"{os.path.join(NL, "runtime-core", REL, "chaos_runtime_core.lib")}"',
    f'"{os.path.join(NL, "bootstrap", REL, "chaos_bootstrap.lib")}"',
    f'"{os.path.join(NL, "interpreter", REL, "chaos_interpreter.lib")}"',
    f'"{os.path.join(NL, "common", REL, "chaos_common.lib")}"',
    f'"{os.path.join(NL, "support", REL, "chaos_support.lib")}"',
    f'"{os.path.join(NL, "hot-update", REL, "chaos_hot_update.lib")}"',
    f'"{os.path.join(REPO, "build", "native", "fmt_build", REL, "chaos_fmt.lib")}"',
    f'"{os.path.join(BDWGC, REL, "chaos_bdwgc.lib")}"',
    "ole32.lib", "user32.lib"
])

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

# Write host once
host_path = os.path.join(BUILD, "ft_host.cpp")
with open(host_path, "w") as f:
    f.write(HOST)
host_obj = os.path.join(BUILD, "ft_host.obj")
r = run(f'cl /nologo /std:c++20 /c /EHsc /W3 /utf-8 /Od /MD {INCS} {DEFS} -Fo"{host_obj}" "{host_path}"')
if r.returncode != 0:
    print("HOST COMPILE FAIL")

def test(flags, label):
    gen_obj = os.path.join(BUILD, f"ft_{label}.obj")
    r1 = run(f'cl {flags} {INCS} {DEFS} -Fo"{gen_obj}" "{GEN}"')
    if r1.returncode != 0:
        errs = [l for l in (r1.stdout + r1.stderr).splitlines() if "error" in l.lower()]
        return f"{label}: COMPILE FAIL: {errs[-1] if errs else '?'}"
    exe = os.path.join(BUILD, f"ft_{label}.exe")
    r2 = run(f'link /nologo /out:"{exe}" "{host_obj}" "{gen_obj}" {LINKLIBS}')
    if r2.returncode != 0:
        errs = [l for l in (r2.stdout + r2.stderr).splitlines() if "error" in l.lower()]
        return f"{label}: LINK FAIL: {errs[-1] if errs else '?'}"
    r3 = run(f'"{exe}"')
    out = (r3.stdout + r3.stderr).strip()
    return f"{label}: RC={r3.returncode} out={out[:500]}"

# Baseline: /Od (crash) and /O1 (works)
print(test("/nologo /std:c++20 /c /EHsc /W3 /utf-8 /Od /MD", "od"))
print(test("/nologo /std:c++20 /c /EHsc /W3 /utf-8 /O1 /MD", "o1"))

# Individual flags that /O1 enables (vs /Od):
# /Ob2 - inline expansion (we tested this - /Od+/Ob2 still crashed)
# /Oi - generate intrinsics
# /Ot - favor speed (vs /Os favor size)
# /Oy - frame pointer omission
# /GF - string pooling (read-only strings)
# /Gy - function-level linking
print(test("/nologo /std:c++20 /c /EHsc /W3 /utf-8 /Od /MD /GF", "od_gf"))
print(test("/nologo /std:c++20 /c /EHsc /W3 /utf-8 /Od /MD /Gy", "od_gy"))
print(test("/nologo /std:c++20 /c /EHsc /W3 /utf-8 /Od /MD /GF /Gy", "od_gf_gy"))
print(test("/nologo /std:c++20 /c /EHsc /W3 /utf-8 /Od /MD /Ot", "od_ot"))
print(test("/nologo /std:c++20 /c /EHsc /W3 /utf-8 /Od /MD /Oy", "od_oy"))

# Try /O1 with /Ob0
print(test("/nologo /std:c++20 /c /EHsc /W3 /utf-8 /O1 /Ob0 /MD", "o1_ob0"))
