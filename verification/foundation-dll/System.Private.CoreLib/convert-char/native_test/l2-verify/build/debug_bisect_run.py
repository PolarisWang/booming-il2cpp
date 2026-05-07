"""Debug L2 crash via cmd.exe piping stderr to stdout to capture crashes."""
import subprocess
import os
import sys

REPO = r"D:\agent\booming-il2cpp"
BUILD = REPO + r"\verification\foundation-dll\System.Private.CoreLib\convert-char\native_test\l2-verify\build"
GEN = REPO + r"\verification\foundation-dll\System.Private.CoreLib\convert-char\il2cpp_dist\genuine\ConvertCharNativeEntry\generated\native-aot.generated.cpp"
VCVARS = r'C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars64.bat'
PREFIX = f'call "{VCVARS}" x64 -vcvars_ver=14.42 >nul 2>nul && '

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

def run_vcvars(cmd):
    """Run under vcvars env, capturing everything."""
    full = f'{PREFIX} {cmd}'
    return subprocess.run(full, shell=True, capture_output=True, timeout=120)

def run_direct(cmd):
    """Run directly (no vcvars wrapper), capturing everything including crash output."""
    return subprocess.run(cmd, shell=True, capture_output=True, timeout=30)

# Pre-compile everything needed
gen_obj = os.path.join(BUILD, "bg_gen.obj")
if not os.path.exists(gen_obj):
    r = run_vcvars(f'cl {FLAGS} {INCS} {DEFS} -Fo"{gen_obj}" "{GEN}"')
    assert r.returncode == 0, "Gen compile failed"

host_main = os.path.join(BUILD, "bg2_host.cpp")
with open(host_main, "w") as f:
    f.write("""#include <cstdio>
#include <csetjmp>
extern "C" int __chaos_assert_failures;
static void (*g_local_fb)() = nullptr;
extern "C" void SetExceptionFallback(void(*fn)()) { g_local_fb = fn; }
extern "C" int RunNativeAot(int);
#include "expected_checksums.h"
static jmp_buf buf;
static void fb(){longjmp(buf,1);}
int main(){
    __chaos_assert_failures=0; SetExceptionFallback(fb); int rf=0;
    for(int i=0;i<kExpectedCount;i++){
        int a=0;bool t=false;
        if(setjmp(buf)==0){a=RunNativeAot(i);}else{t=true;}
        if(kExpectedChecksums[i]==-1)continue;
        if(t){printf("FAIL [%d]: threw exp %d\\n",i,kExpectedChecksums[i]);rf++;continue;}
        if(a!=kExpectedChecksums[i]){printf("FAIL [%d]: exp %d got %d\\n",i,kExpectedChecksums[i],a);rf++;}
    }
    printf("L2: %d/%d passed\\n",kExpectedCount-rf,kExpectedCount);
    return __chaos_assert_failures+rf;
}
""")

# Compile fresh
host_obj = os.path.join(BUILD, "bg2_host.obj")
r = run_vcvars(f'cl {FLAGS} {INCS} {DEFS} -Fo"{host_obj}" "{host_main}"')
assert r.returncode == 0, f"Host compile failed: {r.stdout[-300:]} {r.stderr[-300:]}"

# Link
exe = os.path.join(BUILD, "bg2_test.exe")
r = run_vcvars(f'link /nologo /out:"{exe}" "{host_obj}" "{gen_obj}" {LINKLIBS}')
assert r.returncode == 0, f"Link failed: {r.stdout[-300:]} {r.stderr[-300:]}"

print("=== Build OK, running printf-based test ===", flush=True)

# Try direct execution (without vcvars wrapper) to capture crash output
# The exe is built with /MD so needs CRT DLLs - run under vcvars
r = run_vcvars(f'"{exe}"')
print(f"RC={r.returncode}", flush=True)
out = (r.stdout + r.stderr).decode("utf-8", errors="replace")
print(f"stdout/stderr ({len(out)} bytes):", flush=True)
print(out[:2000], flush=True)

# Now test with native_verify_main.cpp (original) but capture stderr too
print("\n=== Now testing original native_verify_main.cpp ===", flush=True)
orig_obj = os.path.join(BUILD, "bg_orig.obj")
r = run_vcvars(f'cl {FLAGS} {INCS} {DEFS} -Fo"{orig_obj}" "{REPO}\\src\\native\\benchmark-host\\native_verify_main.cpp"')
assert r.returncode == 0

orig_exe = os.path.join(BUILD, "bg_orig_test.exe")
r = run_vcvars(f'link /nologo /out:"{orig_exe}" "{orig_obj}" "{gen_obj}" {LINKLIBS}')
assert r.returncode == 0

r = run_vcvars(f'"{orig_exe}"')
print(f"RC={r.returncode}", flush=True)
out = (r.stdout + r.stderr).decode("utf-8", errors="replace")
print(f"stdout/stderr ({len(out)} bytes):", flush=True)
print(out[:2000], flush=True)
