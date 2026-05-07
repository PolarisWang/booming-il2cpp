"""Targeted bisect: stub specific method bodies to find the crash culprit."""
import subprocess, os, re

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

with open(GEN, encoding="utf-8-sig") as f:
    original = f.read()

def stub_method_body(code, method_name):
    """Replace the body of a specific method with { return 0; }.
    method_name like 'Method2' or 'Run'. Uses brace matching."""
    lines = code.split('\n')
    result = []
    i = 0
    in_target = False
    brace_depth = 0

    # Pattern to match the method declaration line
    # extern "C" CHAOS_IL2CPP_INT32 ConvertCharNativeEntry_ConvertCharNativeEntry_Method2(void)
    target_pattern = f"ConvertCharNativeEntry_ConvertCharNativeEntry_{method_name}("

    while i < len(lines):
        line = lines[i]
        stripped = line.strip()

        if not in_target and target_pattern in stripped:
            indent = line[:len(line) - len(line.lstrip())]
            in_target = True

            if '{' in stripped:
                # body starts on same line
                result.append(indent + stripped[:stripped.index('{')+1] + ' return 0; }')
                i += 1
                brace_depth = stripped.count('{') - stripped.count('}')
                # skip remaining body if multi-line
                while i < len(lines) and brace_depth > 0:
                    l = lines[i]
                    brace_depth += l.count('{') - l.count('}')
                    i += 1
                in_target = False
                continue
            else:
                # body on next line(s)
                result.append(line)
                i += 1
                while i < len(lines) and '{' not in lines[i]:
                    result.append(lines[i])
                    i += 1
                if i < len(lines):
                    # Skip the entire body
                    brace_depth = lines[i].count('{') - lines[i].count('}')
                    i += 1
                    while i < len(lines) and brace_depth > 0:
                        l = lines[i]
                        brace_depth += l.count('{') - l.count('}')
                        i += 1
                    result.append(indent + '{ return 0; }')
                in_target = False
                continue

        result.append(line)
        i += 1

    return '\n'.join(result)


def build_and_run(label, code, host_code=None):
    gen_path = os.path.join(BUILD, f"bs5_{label}.cpp")
    with open(gen_path, "w") as f:
        f.write(code)

    gen_obj = os.path.join(BUILD, f"bs5_{label}.obj")
    r1 = run(f'cl {FLAGS} {INCS} {DEFS} -Fo"{gen_obj}" "{gen_path}"')
    if r1.returncode != 0:
        errs = [l for l in (r1.stdout + r1.stderr).splitlines() if "error" in l.lower()]
        return f"{label}: COMPILE FAIL: {errs[-1] if errs else '?'}"

    host_path = os.path.join(BUILD, "bs5_host.cpp")
    with open(host_path, "w") as f:
        f.write(host_code or HOST)
    host_obj = os.path.join(BUILD, "bs5_host.obj")
    r2 = run(f'cl {FLAGS} {INCS} {DEFS} -Fo"{host_obj}" "{host_path}"')
    if r2.returncode != 0:
        return f"{label}: HOST COMPILE FAIL"

    exe = os.path.join(BUILD, f"bs5_{label}.exe")
    r3 = run(f'link /nologo /out:"{exe}" "{host_obj}" "{gen_obj}" {LINKLIBS}')
    if r3.returncode != 0:
        return f"{label}: LINK FAILED"

    r4 = run(f'"{exe}"')
    out = (r4.stdout + r4.stderr).strip()
    return f"{label}: RC={r4.returncode} out={out[:800]}"

# Test: stub Method2 only
print("=== Test A: stub Method2 body only ===")
code_a = stub_method_body(original, "Method2")
result_a = build_and_run("a_stub_m2", code_a)
print(f"  {result_a}")

# If Test A still crashes, the crash is before the method body
# Test: stub both Method2 and its inline wrapper
# Actually let me just replace the ToChar_Char inline wrapper with a return 0 stub
def stub_inline_wrapper(code, wrapper_name):
    """Replace an extern C inline wrapper body with { return 0; }"""
    lines = code.split('\n')
    result = []
    i = 0
    in_target = False
    brace_depth = 0

    while i < len(lines):
        line = lines[i]
        stripped = line.strip()

        if not in_target and wrapper_name in stripped and 'extern "C" inline' in stripped:
            indent = line[:len(line) - len(line.lstrip())]
            in_target = True

            if '{' in stripped:
                brace_depth = stripped.count('{') - stripped.count('}')
                result.append(indent + stripped[:stripped.index('{')+1] + ' return 0; }')
                i += 1
                while i < len(lines) and brace_depth > 0:
                    l = lines[i]
                    brace_depth += l.count('{') - l.count('}')
                    i += 1
                in_target = False
                continue
            else:
                result.append(line)
                i += 1
                while i < len(lines) and '{' not in lines[i]:
                    result.append(lines[i])
                    i += 1
                if i < len(lines):
                    brace_depth = lines[i].count('{') - lines[i].count('}')
                    i += 1
                    while i < len(lines) and brace_depth > 0:
                        l = lines[i]
                        brace_depth += l.count('{') - l.count('}')
                        i += 1
                    result.append(indent + '{ return 0; }')
                in_target = False
                continue

        result.append(line)
        i += 1

    return '\n'.join(result)

# Test B: stub the ToChar_Char inline wrapper
print("\n=== Test B: stub ToChar_Char inline wrapper ===")
code_b = stub_inline_wrapper(original, "ToChar_System_Char_System_Char_")
result_b = build_and_run("b_stub_wrap", code_b)
print(f"  {result_b}")

# Test C: stub Method0 and Method1 to see if RC changes from 11
print("\n=== Test C: stub only RunNativeAot (verify Test 2 from bisect4) ===")
code_c = stub_method_body(original, "Run")
result_c = build_and_run("c_stub_run", code_c)
print(f"  {result_c}")

# Test D: stub all 18 methods
print("\n=== Test D: stub ALL 18 methods ===")
code_d = original
for m in [f"Method{i}" for i in range(18)]:
    code_d = stub_method_body(code_d, m)
result_d = build_and_run("d_stub_all_m", code_d)
print(f"  {result_d}")
