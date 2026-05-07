"""Debug L2 crash - the generated code's static initializers cause AV."""
import subprocess
import os
import sys

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

# Write the host file using Python to avoid batch file escaping issues
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

# Pre-compile generated code once
gen_obj = os.path.join(BUILD, "bg_gen.obj")
if os.path.exists(gen_obj): os.remove(gen_obj)
r = run(f'cl {FLAGS} {INCS} {DEFS} -Fo"{gen_obj}" "{GEN}"')
if r.returncode != 0:
    print("Gen compile failed")
    sys.exit(1)
print("Gen compiled OK")

# Compile host
host_obj = os.path.join(BUILD, "bg2_host.obj")
if os.path.exists(host_obj): os.remove(host_obj)
r = run(f'cl {FLAGS} {INCS} {DEFS} -Fo"{host_obj}" "{host_main}"')
if r.returncode != 0:
    print("Host compile failed:", r.stdout[-500:], r.stderr[-500:])
    sys.exit(1)
print("Host compiled OK")

# Link
exe = os.path.join(BUILD, "bg2_test.exe")
if os.path.exists(exe): os.remove(exe)
r = run(f'link /nologo /out:"{exe}" "{host_obj}" "{gen_obj}" {LINKLIBS}')
if r.returncode != 0:
    print("Link failed:", r.stdout[-500:], r.stderr[-500:])
    sys.exit(1)
print("Link OK")

# Run
r = run(f'"{exe}"')
print(f"Run RC={r.returncode}")
out = (r.stdout + r.stderr).decode("utf-8", errors="replace")
print(f"Output: {out}")
