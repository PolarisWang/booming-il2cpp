"""Incremental elimination: add sections of generated code one at a time."""
import subprocess, os, sys

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

PREAMBLE = """#include <chaos/common.h>
#include "runtime_core.h"
#include "codegen_bridge.h"
#include "module_registry.h"
#include "abi_manifest.h"
#include "dispatch_table.h"
#include "runtime_vtable.h"
#include "runtime_instantiation.h"
extern "C" void InterpreterEntryDirect(CHAOS_IL2CPP_UINTPTR, void*, void*) noexcept;
#pragma warning(push)
#pragma warning(disable: 4065 4244)
#include <chaos/common.h>
#include <chaos/type_info.h>
"""

POSTAMBLE = """
#pragma warning(pop)
extern "C" int __chaos_assert_failures = 0;
extern "C" int RunNativeAot(int i) { return 0; }
"""

def run(cmd):
    full = f'call "{VCVARS}" x64 -vcvars_ver=14.42 >nul 2>nul && {cmd}'
    r = subprocess.run(full, shell=True, capture_output=True, text=True, timeout=120)
    return r

def test_variant(code, label):
    gen_path = os.path.join(BUILD, f"el_{label}.cpp")
    full_code = PREAMBLE + code + POSTAMBLE
    with open(gen_path, "w") as f:
        f.write(full_code)
    gen_obj = os.path.join(BUILD, f"el_{label}.obj")
    r1 = run(f'cl {FLAGS} {INCS} {DEFS} -Fo"{gen_obj}" "{gen_path}"')
    if r1.returncode != 0:
        errs = [l for l in (r1.stdout + r1.stderr).splitlines() if "error" in l.lower()]
        return f"{label}: COMPILE FAIL: {errs[-1] if errs else '?'}"

    host_src = os.path.join(BUILD, "el_host.cpp")
    with open(host_src, "w") as f:
        f.write("""#include <cstdio>
#include <csetjmp>
extern "C" int __chaos_assert_failures;
extern "C" void SetExceptionFallback(void (*fn)());
extern "C" int RunNativeAot(int);
#include "expected_checksums.h"
static jmp_buf buf;
static void fb() { longjmp(buf, 1); }
int main() {
    __chaos_assert_failures = 0;
    SetExceptionFallback(fb);
    int rf = 0;
    for (int i = 0; i < kExpectedCount; i++) {
        int a = 0; bool t = false;
        if (setjmp(buf) == 0) { a = RunNativeAot(i); } else { t = true; }
        if (kExpectedChecksums[i] == -1) continue;
        if (t) { printf("FAIL[%d]:threw %d\\n", i, kExpectedChecksums[i]); rf++; continue; }
        if (a != kExpectedChecksums[i]) { printf("FAIL[%d]:exp %d got %d\\n", i, kExpectedChecksums[i], a); rf++; }
    }
    SetExceptionFallback(nullptr);
    printf("L2: %d/%d passed\\n", kExpectedCount-rf, kExpectedCount);
    return __chaos_assert_failures + rf;
}
""")
    host_obj = os.path.join(BUILD, "el_host.obj")
    r2 = run(f'cl {FLAGS} {INCS} {DEFS} -Fo"{host_obj}" "{host_src}"')
    if r2.returncode != 0:
        return f"{label}: HOST COMPILE FAIL"

    exe = os.path.join(BUILD, f"el_{label}.exe")
    r3 = run(f'link /nologo /out:"{exe}" "{host_obj}" "{gen_obj}" {LINKLIBS}')
    if r3.returncode != 0:
        return f"{label}: LINK FAILED"

    r4 = run(f'"{exe}"')
    out = (r4.stdout + r4.stderr).strip()
    return f"{label}: RC={r4.returncode} out={out[:200]}"

with open(GEN, encoding="utf-8-sig") as f:
    lines = f.readlines()

print("=== Binary search: which line range causes the crash? ===")

# We know the preamble is lines 0-18 and postamble (after pragma pop) is lines ~699-735
# Let's find the crash by adding lines one section at a time

# Find key line markers
key_lines = {}
for i, line in enumerate(lines):
    if "constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_managed_array" in line:
        key_lines["type_id_array"] = i
    elif "chaos_normalize_native_int_argument" in line:
        key_lines["normalize_fn"] = i
    elif "s_abi_manifest" in line and "{" in line:
        key_lines["abi_manifest"] = i
    elif "ModuleDescriptor" in line and "s_native_aot_module" in line:
        key_lines["module_desc"] = i
    elif "s_name_index_registered" in line:
        key_lines["name_index"] = i
    elif "ConvertCharNativeEntry_ConvertCharNativeEntry_Method0" in line:
        key_lines["method0"] = i
    elif "extern \"C\" CHAOS_IL2CPP_INT32 Run" in line:
        key_lines["run_fn"] = i

print("Key line markers:", {k: v+1 for k, v in key_lines.items()})

# Test: add the minimal data sections incrementally
# Section 0: just the inline helpers + type info (lines 19 to just before string table)
# Section A: inline external runtime helpers only
# Let's extract these as text ranges

# Lines 19-337: basic setup (type info, managed array type, normalize fn, string table data)
# Lines 338-434: extern "C" inline chaos helpers
# Lines 435-598: object model section (kGenericTypeEntries, type registration, module descriptor)
# Lines 599-695: module registration, name index registration
# Lines 699-733: generic registration init
# Lines 735-end: actual method implementations

tests = [
    ("1_typeinfos_only", 19, 337),      # type definitions + string table
    ("2_extern_inline", 338, 434),       # extern "C" inline functions
    ("3_objmodel", 435, 598),            # object model, module descriptor
    ("4_registration", 599, 695),        # registration
    ("5_genreginit", 699, 733),          # generic registration init
    ("6_methods", 735, len(lines)),      # actual methods
]

# Test each section individually (with preamble + postamble)
# If a section compiles and runs, it's safe
# If a section crashes, we found the culprit
for label, start, end in tests:
    code = ''.join(lines[start:end])
    result = test_variant(code, label)
    print(f"  {result}")

# Test all sections together excluding one (inverse test)
all_section_codes = ''.join(lines[19:])  # everything after preamble
result_all = test_variant(all_section_codes, "0_all")
print(f"  0_all: {result_all}")
