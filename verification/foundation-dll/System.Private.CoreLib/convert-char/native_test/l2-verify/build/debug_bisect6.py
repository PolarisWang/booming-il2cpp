"""Test if extern "C" inline COMDAT issue causes the crash.

Hypothesis: with the anonymous namespace removed, extern "C" inline wrappers
now have external COMDAT linkage. Under /Od (no inlining), MSVC emits them as
real functions with selectany. Something about this causes the AV.

Test: replace `extern "C" inline` with `extern "C"` (no inline) everywhere.
"""
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
FLAGS = "/nologo /std:c++20 /c /EHsc /W3 /utf-8 /Od /MD"

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

def build_and_run(label, gen_code):
    gen_path = os.path.join(BUILD, f"bs6_{label}.cpp")
    with open(gen_path, "w") as f:
        f.write(gen_code)

    gen_obj = os.path.join(BUILD, f"bs6_{label}.obj")
    r1 = run(f'cl {FLAGS} {INCS} {DEFS} -Fo"{gen_obj}" "{gen_path}"')
    if r1.returncode != 0:
        errs = [l for l in (r1.stdout + r1.stderr).splitlines() if "error" in l.lower()]
        return f"{label}: COMPILE FAIL: {errs[-1] if errs else '?'}"

    host_path = os.path.join(BUILD, "bs6_host.cpp")
    with open(host_path, "w") as f:
        f.write(HOST)
    host_obj = os.path.join(BUILD, "bs6_host.obj")
    r2 = run(f'cl {FLAGS} {INCS} {DEFS} -Fo"{host_obj}" "{host_path}"')
    if r2.returncode != 0:
        return f"{label}: HOST COMPILE FAIL"

    exe = os.path.join(BUILD, f"bs6_{label}.exe")
    r3 = run(f'link /nologo /out:"{exe}" "{host_obj}" "{gen_obj}" {LINKLIBS}')
    if r3.returncode != 0:
        return f"{label}: LINK FAILED"

    r4 = run(f'"{exe}"')
    out = (r4.stdout + r4.stderr).strip()
    return f"{label}: RC={r4.returncode} out={out[:800]}"

# Link libs
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

with open(GEN, encoding="utf-8-sig") as f:
    original = f.read()

# Test 1: Remove 'inline' from all extern "C" inline functions
print("=== Test 1: Remove 'inline' from extern \"C\" inline wrappers ===")
code1 = original.replace('extern "C" inline ', 'extern "C" ')
r1 = build_and_run("no_inline", code1)
print(f"  {r1}")

# Test 2: Keep inline but compile with /O2 (force inline)
print("\n=== Test 2: extern \"C\" inline with /O2 (force inline expansion) ===")
gen_path2 = os.path.join(BUILD, "bs6_opt_inline.cpp")
with open(gen_path2, "w") as f:
    f.write(original)
gen_obj2 = os.path.join(BUILD, "bs6_opt_inline.obj")
r = run(f'cl /nologo /std:c++20 /c /EHsc /W3 /utf-8 /O2 /MD {INCS} {DEFS} -Fo"{gen_obj2}" "{gen_path2}"')
if r.returncode != 0:
    errs = [l for l in (r.stdout + r.stderr).splitlines() if "error" in l.lower()]
    print(f"  COMPILE FAIL: {errs[-1] if errs else '?'}")
else:
    host_path = os.path.join(BUILD, "bs6_host.cpp")
    host_obj = os.path.join(BUILD, "bs6_host.obj")
    r2 = run(f'cl {FLAGS} {INCS} {DEFS} -Fo"{host_obj}" "{host_path}"')
    if r2.returncode == 0:
        exe = os.path.join(BUILD, "bs6_opt_inline.exe")
        r3 = run(f'link /nologo /out:"{exe}" "{host_obj}" "{gen_obj2}" {LINKLIBS}')
        if r3.returncode == 0:
            r4 = run(f'"{exe}"')
            out = (r4.stdout + r4.stderr).strip()
            print(f"  RC={r4.returncode} out={out[:800]}")
        else:
            errs = [l for l in (r3.stdout + r3.stderr).splitlines() if "error" in l.lower()]
            print(f"  LINK FAIL: {errs[-1] if errs else '?'}")
    else:
        print(f"  HOST COMPILE FAIL")

# Test 3: Revert to anonymous namespace (old behavior)
print("\n=== Test 3: Re-add anonymous namespace wrapper ===")
code3 = original
# Find the right place to wrap - after the preamble/includes, before the data section
# The first line of external scope code is the string table static init
# Wrap the entire thing in namespace {}
# Actually just wrap from the first line that isn't #include/#pragma
code3 = 'namespace {\n' + code3 + '\n}'
r3 = build_and_run("anon_ns", code3)
print(f"  {r3}")
