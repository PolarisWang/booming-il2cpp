"""Bisect crash: test each method individually and print output to find the culprit."""
import subprocess
import os
import sys

REPO = r"D:\agent\booming-il2cpp"
BUILD = REPO + r"\verification\foundation-dll\System.Private.CoreLib\convert-char\native_test\l2-verify\build"
GEN = REPO + r"\verification\foundation-dll\System.Private.CoreLib\convert-char\il2cpp_dist\genuine\ConvertCharNativeEntry\generated\native-aot.generated.cpp"
VCVARS = r'C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars64.bat'

# Copy the exact build recipe from fact_l2_verifier.py (the L2 command line)
vcvars_prefix = f'call "{VCVARS}" x64 -vcvars_ver=14.42 >nul 2>nul && '
include_dirs = [
    REPO + r"\src\native\common",
    REPO + r"\src\native\common\chaos",
    REPO + r"\contracts\native\v0",
    REPO + r"\src\native\runtime-core",
    REPO + r"\src\native\interpreter",
    REPO + r"\src\native\bootstrap",
    REPO + r"\third_party\bdwgc\include",
    REPO + r"\third_party\fmt\include",
    BUILD,
]
include_flags = " ".join(f'-I"{d}"' for d in include_dirs)
compile_flags = "/nologo /std:c++20 /c /EHsc /W3 /utf-8 /Od /MD"
defines = "-DCHAOS_IL2CPP_CHECK -DCHAOS_IL2CPP_TRACE_ENABLED -DCHAOS_IL2CPP_VERIFY_MODE -DCHAOS_RUNTIME_ABI_STATIC -DGC_NOT_DLL"
native_lib_dir = REPO + r"\build\native\src\native"
bdwgc_dir = REPO + r"\build\native\bdwgc_build"
chaos_lib = REPO + r"\build\native\src\native\runtime-core\Release\chaos_runtime_core.lib"

# Build lib list
dep_libs = " ".join(
    f'"{native_lib_dir / Path(lib).name}\\Release\\{lib_name}.lib"'
    if os.path.sep not in lib else f'"{lib}"'
    for lib, lib_name in [
        (r"bootstrap", "chaos_bootstrap"),
        (r"interpreter", "chaos_interpreter"),
        (r"common", "chaos_common"),
        (r"support", "chaos_support"),
        (r"hot-update", "chaos_hot_update"),
    ]
)
# Fix paths
dep_libs = (
    f'"{native_lib_dir}\\bootstrap\\Release\\chaos_bootstrap.lib" '
    f'"{native_lib_dir}\\interpreter\\Release\\chaos_interpreter.lib" '
    f'"{native_lib_dir}\\common\\Release\\chaos_common.lib" '
    f'"{native_lib_dir}\\support\\Release\\chaos_support.lib" '
    f'"{native_lib_dir}\\hot-update\\Release\\chaos_hot_update.lib" '
    f'"{REPO}\\build\\native\\fmt_build\\Release\\chaos_fmt.lib" '
    f'"{bdwgc_dir}\\Release\\chaos_bdwgc.lib" '
    f'ole32.lib user32.lib'
)

# Write test host: call each RunNativeAot(i) individually with printf tracing
test_host = os.path.join(BUILD, "bisect_host.cpp")
with open(test_host, "w") as f:
    f.write("""#include <cstdio>
#include <cstdlib>
#include <csetjmp>
extern "C" int __chaos_assert_failures;
extern "C" void SetExceptionFallback(void (*fn)());
extern "C" int RunNativeAot(int entryIndex);
#include "expected_checksums.h"
static jmp_buf s_verify_buf;
static void exception_fallback() { longjmp(s_verify_buf, 1); }
static int count = 0;
int main() {
    __chaos_assert_failures = 0;
    SetExceptionFallback(&exception_fallback);
    for (int i = 0; i < kExpectedCount; i++) {
        printf("[%d] starting...\\n", i); fflush(stdout);
        int actual = 0;
        bool threw = false;
        if (setjmp(s_verify_buf) == 0) {
            actual = RunNativeAot(i);
            printf("[%d] returned %d\\n", i, actual); fflush(stdout);
        } else {
            threw = true;
            printf("[%d] threw exception\\n", i); fflush(stdout);
        }
        if (kExpectedChecksums[i] == -1) {
            printf("[%d] checksum=-1, skipped\\n", i); fflush(stdout);
            continue;
        }
        if (threw) {
            printf("[%d] FAIL: threw, expected %d\\n", i, kExpectedChecksums[i]); fflush(stdout);
            continue;
        }
        if (actual != kExpectedChecksums[i]) {
            printf("[%d] FAIL: expected %d, got %d\\n", i, kExpectedChecksums[i], actual); fflush(stdout);
        }
    }
    printf("DONE\\n"); fflush(stdout);
    SetExceptionFallback(nullptr);
    return __chaos_assert_failures;
}
""")

# Pre-compile generated code
def run(cmd):
    full = f'{vcvars_prefix} {cmd}'
    return subprocess.run(full, shell=True, capture_output=True, timeout=120)

# Compile everything
gen_obj = os.path.join(BUILD, "bt_gen.obj")
r = run(f'cl {compile_flags} {include_flags} {defines} -Fo"{gen_obj}" "{GEN}"')
assert r.returncode == 0, f"Gen compile failed: {r.stderr[-300:]}"

host_obj = os.path.join(BUILD, "bt_host.obj")
r = run(f'cl {compile_flags} {include_flags} {defines} -Fo"{host_obj}" "{test_host}"')
assert r.returncode == 0, f"Host compile failed: {r.stderr[-300:]}"

# Link
exe = os.path.join(BUILD, "bt_test.exe")
r = run(f'link /nologo /out:"{exe}" "{host_obj}" "{gen_obj}" "{chaos_lib}" {dep_libs}')
assert r.returncode == 0, f"Link failed: {r.stderr[-300:]}"
print("Build OK, running...", flush=True)

# Run
r = run(f'"{exe}" 2>&1')
print(f"RC={r.returncode}", flush=True)
out = (r.stdout + r.stderr).decode("utf-8", errors="replace")
print(f"Output ({len(out)} bytes):")
print(out[:3000])
