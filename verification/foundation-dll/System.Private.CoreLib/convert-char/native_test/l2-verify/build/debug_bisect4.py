"""Bisect approach: keep all declarations, stub method bodies only."""
import subprocess, os, sys, re

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

def build_and_run(label, code_transform):
    """Apply transform, compile, link, run, return result string."""
    gen_path = os.path.join(BUILD, f"bs4_{label}.cpp")
    new_code = code_transform(original)
    with open(gen_path, "w") as f:
        f.write(new_code)

    gen_obj = os.path.join(BUILD, f"bs4_{label}.obj")
    r1 = run(f'cl {FLAGS} {INCS} {DEFS} -Fo"{gen_obj}" "{gen_path}"')
    if r1.returncode != 0:
        errs = [l for l in (r1.stdout + r1.stderr).splitlines() if "error" in l.lower()]
        return f"{label}: COMPILE FAIL: {errs[-1] if errs else '?'}"

    host_path = os.path.join(BUILD, "bs4_host.cpp")
    with open(host_path, "w") as f:
        f.write(HOST)
    host_obj = os.path.join(BUILD, "bs4_host.obj")
    r2 = run(f'cl {FLAGS} {INCS} {DEFS} -Fo"{host_obj}" "{host_path}"')
    if r2.returncode != 0:
        return f"{label}: HOST COMPILE FAIL"

    exe = os.path.join(BUILD, f"bs4_{label}.exe")
    r3 = run(f'link /nologo /out:"{exe}" "{host_obj}" "{gen_obj}" {LINKLIBS}')
    if r3.returncode != 0:
        return f"{label}: LINK FAILED"

    r4 = run(f'"{exe}"')
    out = (r4.stdout + r4.stderr).strip()
    return f"{label}: RC={r4.returncode} out={out[:500]}"

# Test 0: no modifications — full generated code
print("=== Test 0: full generated code (no modifications) ===")
result = build_and_run("0_full", lambda s: s)
print(f"  {result}")

# Test 1: stub ALL method bodies (keep declarations)
# Find all function bodies outside static initializers and replace with { return 0; }
def stub_method_bodies(code):
    """Replace method/runtime-call bodies with 'return 0;' stubs.

    Specifically targets:
    - CHAOS_IL2CPP_INT32 RunNativeAot(CHAOS_IL2CPP_INT32 entryIndex) { ... }
    - The 18 ConvertCharNativeEntry_ConvertCharNativeEntry_MethodN methods
    - extern "C" inline runtime wrappers (stub them too so they don't call into runtime)
    """
    # Strategy: replace function bodies using brace matching
    lines = code.split('\n')
    result = []
    i = 0
    in_method = False
    brace_depth = 0
    stubbed = 0

    while i < len(lines):
        line = lines[i]
        stripped = line.strip()

        # Detect function starts that we want to stub
        is_method = any([
            'RunNativeAot(CHAOS_IL2CPP_INT32' in stripped,
            'ConvertCharNativeEntry_ConvertCharNativeEntry_Method' in stripped,
        ])

        if is_method and '{' in stripped:
            # One-liner like "Type fn() { ... }"
            in_method = True
            brace_depth = stripped.count('{') - stripped.count('}')
            # Keep the opening part, replace body
            open_brace_idx = stripped.index('{')
            indent = line[:len(line) - len(line.lstrip())]
            if brace_depth <= 0:
                # Single-line function
                result.append(indent + stripped[:open_brace_idx+1] + ' return 0; }')
            else:
                result.append(indent + stripped[:open_brace_idx+1])
            stubbed += 1
            i += 1
            # Skip until we close all braces
            while i < len(lines) and brace_depth > 0:
                l = lines[i]
                brace_depth += l.count('{') - l.count('}')
                i += 1
            result.append(indent + '    return 0;')
            result.append(indent + '}')
            continue

        if is_method and not '{' in stripped:
            # Declaration on one line, body on next
            in_method = True
            brace_depth = 0
            result.append(line)
            i += 1
            # Skip any parameter continuation lines, then find {
            while i < len(lines) and '{' not in lines[i]:
                result.append(lines[i])
                i += 1
            # Now we're at the opening brace line
            if i < len(lines):
                open_line = lines[i]
                indent = open_line[:len(open_line) - len(open_line.lstrip())]
                result.append(open_line)  # the opening {
                brace_depth = 1
                i += 1
                # Skip until matching closing brace
                while i < len(lines) and brace_depth > 0:
                    l = lines[i]
                    brace_depth += l.count('{') - l.count('}')
                    i += 1
                result.append(indent + '    return 0;')
                result.append(indent + '}')
            stubbed += 1
            continue

        result.append(line)
        i += 1

    print(f"  Stubbed {stubbed} functions", flush=True)
    return '\n'.join(result)

print("\n=== Test 1: stub all method bodies ===")
result = build_and_run("1_stub", stub_method_bodies)
print(f"  {result}")

# Test 2: only stub RunNativeAot body (keep individual methods)
def stub_run_only(code):
    """Replace only RunNativeAot body with { return 0; }"""
    lines = code.split('\n')
    result = []
    i = 0
    while i < len(lines):
        line = lines[i]
        stripped = line.strip()
        if 'RunNativeAot(CHAOS_IL2CPP_INT32' in stripped:
            indent = line[:len(line) - len(line.lstrip())]
            if '{' in stripped:
                result.append(indent + 'RunNativeAot(CHAOS_IL2CPP_INT32 entryIndex) { return 0; }')
                i += 1
                # Skip the original body
                brace_depth = stripped.count('{') - stripped.count('}')
                if brace_depth <= 0:
                    continue
                while i < len(lines) and brace_depth > 0:
                    l = lines[i]
                    brace_depth += l.count('{') - l.count('}')
                    i += 1
                continue
            else:
                result.append(line)
                i += 1
                while i < len(lines) and '{' not in lines[i]:
                    result.append(lines[i])
                    i += 1
                if i < len(lines):
                    indent = lines[i][:len(lines[i]) - len(lines[i].lstrip())]
                    result.append(lines[i])  # {
                    brace_depth = 1
                    i += 1
                    while i < len(lines) and brace_depth > 0:
                        l = lines[i]
                        brace_depth += l.count('{') - l.count('}')
                        i += 1
                    result.append(indent + '    return 0;')
                    result.append(indent + '}')
                continue
        result.append(line)
        i += 1
    return '\n'.join(result)

print("\n=== Test 2: stub RunNativeAot only ===")
result = build_and_run("2_runonly", stub_run_only)
print(f"  {result}")
