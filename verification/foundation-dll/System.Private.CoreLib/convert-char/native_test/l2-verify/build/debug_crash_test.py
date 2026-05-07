"""Bisect L2 crash: test with SetExceptionFallback defined locally."""
import subprocess
import os

REPO = r"D:\agent\booming-il2cpp"
BUILD = REPO + r"\verification\foundation-dll\System.Private.CoreLib\convert-char\native_test\l2-verify\build"
GEN = REPO + r"\verification\foundation-dll\System.Private.CoreLib\convert-char\il2cpp_dist\genuine\ConvertCharNativeEntry\generated\native-aot.generated.cpp"
VCVARS = r'C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars64.bat'
PREFIX = f'call "{VCVARS}" x64 -vcvars_ver=14.42 >nul 2>nul &&'

INCS = " ".join(f'-I"{d}"' for d in [
    REPO + r"\src\native\common", REPO + r"\src\native\common\chaos",
    REPO + r"\contracts\native\v0", REPO + r"\src\native\runtime-core",
    REPO + r"\src\native\interpreter", REPO + r"\src\native\bootstrap",
    REPO + r"\third_party\bdwgc\include", REPO + r"\third_party\fmt\include",
    BUILD])
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
    "ole32.lib", "user32.lib",
])

def run(cmd):
    full = f'cmd.exe /c "{PREFIX} {cmd}"'
    r = subprocess.run(full, shell=True, capture_output=True, timeout=120)
    return r

def compile(src, obj, extra_defs=""):
    if os.path.exists(obj): os.remove(obj)
    r = run(f'cl {FLAGS} {INCS} {DEFS} {extra_defs} -Fo"{obj}" "{src}"')
    ok = r.returncode == 0
    if not ok:
        for line in (r.stdout + r.stderr).split("\n"):
            if "error" in line.lower() and "warning" not in line.lower():
                print(f"    {line}")
    return ok

def link(exe, objs):
    if os.path.exists(exe): os.remove(exe)
    obj_list = " ".join(f'"{o}"' for o in objs)
    r = run(f'link /nologo /out:"{exe}" {obj_list} {LINKLIBS}')
    ok = r.returncode == 0
    if not ok:
        for line in (r.stdout + r.stderr).split("\n"):
            if "error" in line.lower() or "fatal" in line.lower():
                print(f"    {line}")
    return ok

def run_exe(exe):
    r = run(f'"{exe}"')
    out = (r.stdout + r.stderr).decode("utf-8", errors="replace")
    return r.returncode, out

# Pre-compile generated code (same for all tests)
gen_obj = os.path.join(BUILD, "bisect_gen.obj")
compile(GEN, gen_obj)

# ===== TEST 0: native_verify_main.cpp (original) - confirm crash =====
print("=== TEST 0: original native_verify_main.cpp ===")
v_obj0 = os.path.join(BUILD, "b0_v.obj")
compile(REPO + r"\src\native\benchmark-host\native_verify_main.cpp", v_obj0)
exe0 = os.path.join(BUILD, "b0.exe")
link(exe0, [v_obj0, gen_obj])
rc, out = run_exe(exe0)
print(f"  RC={rc}")
if out.strip():
    print(f"  Output: {out[:500]}")

# ===== TEST 1: local SetExceptionFallback stub (no thread_local) =====
print("\n=== TEST 1: local SetExceptionFallback (plain global) ===")
test1_src = os.path.join(BUILD, "b1_main.cpp")
with open(test1_src, "w") as f:
    f.write(r"""
#include <cstdlib>
#include <cstdio>
#include <csetjmp>
extern "C" int __chaos_assert_failures;
// Local stub: plain global, NOT thread_local
static void (*g_exception_fallback)() = nullptr;
extern "C" void SetExceptionFallback(void (*fn)()) { g_exception_fallback = fn; }
extern "C" int RunNativeAot(int entryIndex);
#include "expected_checksums.h"
static jmp_buf s_verify_buf;
static void exception_fallback() { longjmp(s_verify_buf, 1); }
int main() {
    __chaos_assert_failures = 0;
    SetExceptionFallback(&exception_fallback);
    int return_value_failures = 0;
    for (int i = 0; i < kExpectedCount; i++) {
        int actual = 0; bool threw = false;
        if (setjmp(s_verify_buf) == 0) { actual = RunNativeAot(i); }
        else { threw = true; }
        if (kExpectedChecksums[i] == -1) continue;
        if (threw) { printf("FAIL [%d]: native threw, expected %d\n", i, kExpectedChecksums[i]); return_value_failures++; continue; }
        if (actual != kExpectedChecksums[i]) { printf("FAIL [%d]: expected %d, got %d\n", i, kExpectedChecksums[i], actual); return_value_failures++; }
    }
    SetExceptionFallback(nullptr);
    int total = __chaos_assert_failures + return_value_failures;
    printf("L2: %d/%d passed (assert=%d, return=%d)\n", kExpectedCount-return_value_failures, kExpectedCount, __chaos_assert_failures, return_value_failures);
    return total;
}
""")
v_obj1 = os.path.join(BUILD, "b1_v.obj")
compile(test1_src, v_obj1)
exe1 = os.path.join(BUILD, "b1.exe")
link(exe1, [v_obj1, gen_obj])
rc, out = run_exe(exe1)
print(f"  RC={rc}")
print(f"  Output: {out[:1000]}")

# ===== TEST 2: native_verify_main.cpp but with -DNO_CHAOS_LOG_LOCAL_STATIC =====
# Actually just see what happens if we use chaos/common.h but NOT SetExceptionFallback
print("\n=== TEST 2: chaos/common.h included, local SetExceptionFallback ===")
test2_src = os.path.join(BUILD, "b2_main.cpp")
with open(test2_src, "w") as f:
    f.write(r"""
#include <chaos/common.h>
#include <cstdlib>
#include <cstdio>
#include <csetjmp>
extern "C" int __chaos_assert_failures;
// Local SetExceptionFallback
static void (*g_exception_fallback)() = nullptr;
extern "C" void SetExceptionFallback(void (*fn)()) { g_exception_fallback = fn; }
extern "C" int RunNativeAot(int entryIndex);
#include "expected_checksums.h"
static jmp_buf s_verify_buf;
static void exception_fallback() { longjmp(s_verify_buf, 1); }
int main() {
    __chaos_assert_failures = 0;
    SetExceptionFallback(&exception_fallback);
    int return_value_failures = 0;
    for (int i = 0; i < kExpectedCount; i++) {
        int actual = 0; bool threw = false;
        if (setjmp(s_verify_buf) == 0) { actual = RunNativeAot(i); }
        else { threw = true; }
        if (kExpectedChecksums[i] == -1) continue;
        if (threw) { CHAOS_IL2CPP_LOG_ERROR_M("L2", "FAIL [{0}]: native threw, expected {1}", i, kExpectedChecksums[i]); return_value_failures++; continue; }
        if (actual != kExpectedChecksums[i]) { CHAOS_IL2CPP_LOG_ERROR_M("L2", "FAIL [{0}]: expected {1}, got {2}", i, kExpectedChecksums[i], actual); return_value_failures++; }
    }
    SetExceptionFallback(nullptr);
    int total = __chaos_assert_failures + return_value_failures;
    CHAOS_IL2CPP_LOG_INFO_M("L2", "{0}/{1} passed (assert_failures={2}, return_failures={3})", kExpectedCount-return_value_failures, kExpectedCount, __chaos_assert_failures, return_value_failures);
    CHAOS_IL2CPP_LOG_FLUSH();
    return total;
}
""")
v_obj2 = os.path.join(BUILD, "b2_v.obj")
compile(test2_src, v_obj2)
exe2 = os.path.join(BUILD, "b2.exe")
link(exe2, [v_obj2, gen_obj])
rc, out = run_exe(exe2)
print(f"  RC={rc}")
print(f"  Output: {out[:1000]}")
