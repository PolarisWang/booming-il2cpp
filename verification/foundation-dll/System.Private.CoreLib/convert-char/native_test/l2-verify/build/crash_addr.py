"""Get crash address and try to identify the faulting instruction."""
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

def run(cmd):
    full = f'call "{VCVARS}" x64 -vcvars_ver=14.42 >nul 2>nul && {cmd}'
    r = subprocess.run(full, shell=True, capture_output=True, text=True, timeout=120)
    return r

# Build with /Od + debug info
obj = os.path.join(BUILD, "crash_gen.obj")
r1 = run(f'cl /nologo /std:c++20 /c /EHsc /W3 /utf-8 /Od /MD /Z7 {INCS} {DEFS} -Fo"{obj}" "{GEN}"')
if r1.returncode != 0:
    print(f"COMPILE FAIL")
else:
    host_src = os.path.join(BUILD, "crash_host.cpp")
    with open(host_src, "w") as f:
        f.write("""#include <cstdio>
#include <windows.h>
#include <csetjmp>
extern "C" int __chaos_assert_failures;
extern "C" void SetExceptionFallback(void (*fn)());
extern "C" int RunNativeAot(int);
#include "expected_checksums.h"
static jmp_buf buf;
static void fb() { longjmp(buf, 1); }
static LONG WINAPI VectoredHandler(PEXCEPTION_POINTERS ep) {
    printf("CRASH: code=0x%08X addr=%p\\n",
           ep->ExceptionRecord->ExceptionCode,
           ep->ExceptionRecord->ExceptionAddress);
    fflush(stdout);
    return EXCEPTION_CONTINUE_SEARCH;
}
int main() {
    AddVectoredExceptionHandler(1, VectoredHandler);
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
""")
    host_obj = os.path.join(BUILD, "crash_host.obj")
    r2 = run(f'cl /nologo /std:c++20 /c /EHsc /W3 /utf-8 /Od /MD /Z7 {INCS} {DEFS} -Fo"{host_obj}" "{host_src}"')
    if r2.returncode != 0:
        print("HOST COMPILE FAIL")
    else:
        exe = os.path.join(BUILD, "crash_test.exe")
        r3 = run(f'link /nologo /out:"{exe}" "{host_obj}" "{obj}" {LINKLIBS}')
        if r3.returncode != 0:
            print("LINK FAIL")
        else:
            r4 = run(f'"{exe}"')
            out = (r4.stdout + r4.stderr).strip()
            print(f"RC={r4.returncode}")
            print(f"Output:\n{out}")
