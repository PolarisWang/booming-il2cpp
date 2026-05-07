"""Debug L2 crash - find root cause."""
import subprocess
import sys
import os

REPO = r"D:\agent\booming-il2cpp"
BUILD = REPO + r"\verification\foundation-dll\System.Private.CoreLib\convert-char\native_test\l2-verify\build"
GEN = REPO + r"\verification\foundation-dll\System.Private.CoreLib\convert-char\il2cpp_dist\genuine\ConvertCharNativeEntry\generated\native-aot.generated.cpp"
VCVARS = r'C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars64.bat'
PREFIX = f'call "{VCVARS}" x64 -vcvars_ver=14.42 >nul 2>nul &&'

# Build include/directory flags
def esc(p):
    return p.replace("/", "\\")

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
    "ole32.lib", "user32.lib",
])

VERIFY_HOST = REPO + r"\src\native\benchmark-host\native_verify_main.cpp"
runtime_inst = REPO + r"\src\native\runtime-core\runtime_instantiation.cpp"
convert_src = REPO + r"\src\native\runtime-core\convert.cpp"

def run(cmd):
    """Run command under vcvars env via cmd.exe /c."""
    full = f'cmd.exe /c "{PREFIX} {cmd}"'
    r = subprocess.run(full, shell=True, capture_output=True, timeout=120)
    return r

def fresh_compile(src, obj_name):
    obj = os.path.join(BUILD, obj_name)
    if os.path.exists(obj):
        os.remove(obj)
    r = run(f'cl {FLAGS} {INCS} {DEFS} -Fo"{obj}" "{src}"')
    print(f"  Compile {obj_name}: rc={r.returncode}", flush=True)
    if r.returncode != 0:
        for line in (r.stdout + r.stderr).split("\n"):
            if "error" in line.lower() and "warning" not in line.lower():
                print(f"    {line}", flush=True)
    return r.returncode == 0, obj

def do_link(exe_name, objs):
    exe = os.path.join(BUILD, exe_name)
    if os.path.exists(exe):
        os.remove(exe)
    obj_list = " ".join(f'"{o}"' for o in objs)
    r = run(f'link /nologo /out:"{exe}" {obj_list} {LINKLIBS}')
    print(f"  Link {exe_name}: rc={r.returncode}", flush=True)
    if r.returncode != 0:
        for line in (r.stdout + r.stderr).split("\n"):
            if "error" in line.lower() or "fatal" in line.lower():
                print(f"    {line}", flush=True)
    return r.returncode == 0, exe

def do_run(exe):
    r = run(f'"{exe}"')
    out = (r.stdout + r.stderr).decode("utf-8", errors="replace")
    print(f"  Run: rc={r.returncode}", flush=True)
    if out.strip():
        print(f"  Output: {out[:1000]}", flush=True)
    return r.returncode, out

# ===== TEST 0: native_verify_main.cpp as-is (the failing config) =====
print("=== TEST 0: native_verify_main.cpp (original) ===", flush=True)
ok1, obj_v = fresh_compile(VERIFY_HOST, "t0_verify.obj")
ok2, obj_g = fresh_compile(GEN, "t0_gen.obj")
ok3, obj_ri = fresh_compile(runtime_inst, "t0_ri.obj")
ok4, obj_cv = fresh_compile(convert_src, "t0_cv.obj")
if all([ok1, ok2, ok3, ok4]):
    ok5, exe = do_link("t0_test.exe", [obj_v, obj_g, obj_ri, obj_cv])
    if ok5:
        do_run(exe)

# ===== TEST 1: printf host (no chaos/common.h) + generated only =====
print("\n=== TEST 1: printf host (no chaos/common.h) + generated only ===", flush=True)
printf_main = os.path.join(BUILD, "t1_printf_main.cpp")
with open(printf_main, "w") as f:
    f.write(r"""#include <cstdlib>
#include <cstdio>
#include <csetjmp>
extern "C" int __chaos_assert_failures;
extern "C" void SetExceptionFallback(void (*fn)());
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
    int total_failures = __chaos_assert_failures + return_value_failures;
    int passed = kExpectedCount - return_value_failures;
    printf("L2: %d/%d passed (assert_failures=%d, return_failures=%d)\n", passed, kExpectedCount, __chaos_assert_failures, return_value_failures);
    return total_failures;
}
""")
ok1, obj_p = fresh_compile(printf_main, "t1_p.obj")
ok2, obj_g2 = fresh_compile(GEN, "t1_g.obj")
if all([ok1, ok2]):
    ok3, exe = do_link("t1_test.exe", [obj_p, obj_g2])
    if ok3:
        do_run(exe)

# ===== TEST 2: printf host + generated + runtime_instantiation =====
print("\n=== TEST 2: printf host + generated + runtime_instantiation ===", flush=True)
ok1, obj_p2 = fresh_compile(printf_main, "t2_p.obj")
ok2, obj_g2 = fresh_compile(GEN, "t2_g.obj")
ok3, obj_ri2 = fresh_compile(runtime_inst, "t2_ri.obj")
if all([ok1, ok2, ok3]):
    ok4, exe = do_link("t2_test.exe", [obj_p2, obj_g2, obj_ri2])
    if ok4:
        do_run(exe)

# ===== TEST 3: printf host + generated + runtime_instantiation + convert =====
print("\n=== TEST 3: printf host + generated + ri + convert ===", flush=True)
ok1, obj_p3 = fresh_compile(printf_main, "t3_p.obj")
ok2, obj_g3 = fresh_compile(GEN, "t3_g.obj")
ok3, obj_ri3 = fresh_compile(runtime_inst, "t3_ri.obj")
ok4, obj_cv3 = fresh_compile(convert_src, "t3_cv.obj")
if all([ok1, ok2, ok3, ok4]):
    ok5, exe = do_link("t3_test.exe", [obj_p3, obj_g3, obj_ri3, obj_cv3])
    if ok5:
        do_run(exe)
