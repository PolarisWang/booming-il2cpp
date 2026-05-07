"""Test single-TU compilation (host + generated code in one TU)."""
import subprocess, os

REPO = r"D:\agent\booming-il2cpp"
BUILD = os.path.join(REPO, "verification", "foundation-dll", "System.Private.CoreLib",
                     "convert-char", "native_test", "l2-verify", "build")
GEN = os.path.join(REPO, "verification", "foundation-dll", "System.Private.CoreLib",
                   "convert-char", "il2cpp_dist", "genuine",
                   "ConvertCharNativeEntry", "generated", "native-aot.generated.cpp")
VERIFY_HOST = os.path.join(REPO, "src", "native", "benchmark-host", "native_verify_main.cpp")
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

def run(cmd):
    full = f'call "{VCVARS}" x64 -vcvars_ver=14.42 >nul 2>nul && {cmd}'
    r = subprocess.run(full, shell=True, capture_output=True, text=True, timeout=120)
    return r

# Single TU: combined source includes verify host + generated code
combined = os.path.join(BUILD, "tu_combined.cpp")
with open(combined, "w") as f:
    f.write(f'#include "{VERIFY_HOST}"\n')
    f.write(f'#include "{GEN}"\n')

combined_obj = os.path.join(BUILD, "tu_combined.obj")
r1 = run(f'cl {FLAGS} {INCS} {DEFS} -Fo"{combined_obj}" "{combined}"')
if r1.returncode != 0:
    errs = [l for l in (r1.stdout + r1.stderr).splitlines() if "error" in l.lower()]
    print(f"SINGLE-TU: COMPILE FAIL: {errs[-1] if errs else '?'}")
else:
    exe = os.path.join(BUILD, "tu_combined.exe")
    r2 = run(f'link /nologo /out:"{exe}" "{combined_obj}" {LINKLIBS}')
    if r2.returncode != 0:
        errs = [l for l in (r2.stdout + r2.stderr).splitlines() if "error" in l.lower()]
        print(f"SINGLE-TU: LINK FAIL: {errs[-1] if errs else '?'}")
    else:
        r3 = run(f'"{exe}"')
        out = (r3.stdout + r3.stderr).strip()
        print(f"SINGLE-TU: RC={r3.returncode} out={out[:500]}")

# Single TU with GC init
combined2 = os.path.join(BUILD, "tu_gcinit.cpp")
with open(combined2, "w") as f:
    f.write('#include <cstdio>\n')
    f.write('#include <csetjmp>\n')
    f.write('extern "C" int __chaos_assert_failures;\n')
    f.write('extern "C" void SetExceptionFallback(void (*fn)());\n')
    f.write('extern "C" int RunNativeAot(int);\n')
    # Also init GC
    f.write('extern "C" void GC_init(void);\n')
    f.write('#include "expected_checksums.h"\n')
    f.write('static jmp_buf buf;\n')
    f.write('static void fb() { longjmp(buf, 1); }\n')
    f.write('int main() {\n')
    f.write('    printf("main_started\\n"); fflush(stdout);\n')
    f.write('    GC_init();\n')
    f.write('    __chaos_assert_failures = 0;\n')
    f.write('    SetExceptionFallback(fb);\n')
    f.write('    int rf = 0;\n')
    f.write('    for (int i = 0; i < kExpectedCount; i++) {\n')
    f.write('        printf("[%d] call\\n", i); fflush(stdout);\n')
    f.write('        int a = 0; bool t = false;\n')
    f.write('        if (setjmp(buf) == 0) { a = RunNativeAot(i); } else { t = true; }\n')
    f.write('        if (kExpectedChecksums[i] == -1) continue;\n')
    f.write('        if (t) { printf("FAIL[%d]:threw %d\\n", i, kExpectedChecksums[i]); rf++; continue; }\n')
    f.write('        if (a != kExpectedChecksums[i]) { printf("FAIL[%d]:exp %d got %d\\n", i, kExpectedChecksums[i], a); rf++; }\n')
    f.write('    }\n')
    f.write('    SetExceptionFallback(nullptr);\n')
    f.write('    printf("L2: %d/%d passed\\n", kExpectedCount-rf, kExpectedCount);\n')
    f.write('    fflush(stdout);\n')
    f.write('    return __chaos_assert_failures + rf;\n')
    f.write('}\n')
    f.write(f'#include "{GEN}"\n')

combined2_obj = os.path.join(BUILD, "tu_gcinit.obj")
r1 = run(f'cl {FLAGS} {INCS} {DEFS} -Fo"{combined2_obj}" "{combined2}"')
if r1.returncode != 0:
    errs = [l for l in (r1.stdout + r1.stderr).splitlines() if "error" in l.lower()]
    print(f"TU+GCINIT: COMPILE FAIL: {errs[-1] if errs else '?'}")
else:
    exe = os.path.join(BUILD, "tu_gcinit.exe")
    r2 = run(f'link /nologo /out:"{exe}" "{combined2_obj}" {LINKLIBS}')
    if r2.returncode != 0:
        errs = [l for l in (r2.stdout + r2.stderr).splitlines() if "error" in l.lower()]
        print(f"TU+GCINIT: LINK FAIL: {errs[-1] if errs else '?'}")
    else:
        r3 = run(f'"{exe}"')
        out = (r3.stdout + r3.stderr).strip()
        print(f"TU+GCINIT: RC={r3.returncode} out={out[:500]}")
