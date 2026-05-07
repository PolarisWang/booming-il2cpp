"""Minimal reproduction: directly call chaos_convert_tochar_char."""
import subprocess, os

REPO = r"D:\agent\booming-il2cpp"
BUILD = os.path.join(REPO, "verification", "foundation-dll", "System.Private.CoreLib",
                     "convert-char", "native_test", "l2-verify", "build")
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

# Test X: Direct call to chaos_convert_tochar_char
tests = {}

# X1: Direct call to chaos_convert_tochar_char
tests["x1_direct"] = """
#include <cstdio>
#include <chaos/native_types.h>
extern "C" CHAOS_IL2CPP_UINT16 chaos_convert_tochar_char(CHAOS_IL2CPP_INTPTR value);
int main() {
    printf("calling...\\n"); fflush(stdout);
    auto r = chaos_convert_tochar_char(65);
    printf("got %u\\n", (unsigned)r); fflush(stdout);
    return (r == 65) ? 0 : 1;
}
"""

# X2: Call through the inline wrapper pattern (no inline keyword)
tests["x2_via_wrapper"] = """
#include <cstdio>
#include <chaos/native_types.h>
extern "C" CHAOS_IL2CPP_UINT16 chaos_convert_tochar_char(CHAOS_IL2CPP_INTPTR value);
extern "C" CHAOS_IL2CPP_UINT16 chaos_external_runtime_System_Private_CoreLib_System_Convert__ToChar_System_Char_System_Char_(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
    return chaos_convert_tochar_char(chaos_fn_arg_0);
}
int main() {
    printf("calling...\\n"); fflush(stdout);
    auto r = chaos_external_runtime_System_Private_CoreLib_System_Convert__ToChar_System_Char_System_Char_(65);
    printf("got %u\\n", (unsigned)r); fflush(stdout);
    return (r == 65) ? 0 : 1;
}
"""

# X3: Call through extern "C" inline wrapper (exact match to generated code)
tests["x3_inline_wrapper"] = """
#include <cstdio>
#include <chaos/native_types.h>
extern "C" CHAOS_IL2CPP_UINT16 chaos_convert_tochar_char(CHAOS_IL2CPP_INTPTR value);
extern "C" inline CHAOS_IL2CPP_UINT16 chaos_external_runtime_System_Private_CoreLib_System_Convert__ToChar_System_Char_System_Char_(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
    return chaos_convert_tochar_char(chaos_fn_arg_0);
}
int main() {
    printf("calling...\\n"); fflush(stdout);
    auto r = chaos_external_runtime_System_Private_CoreLib_System_Convert__ToChar_System_Char_System_Char_(65);
    printf("got %u\\n", (unsigned)r); fflush(stdout);
    return (r == 65) ? 0 : 1;
}
"""

for label, code in tests.items():
    src = os.path.join(BUILD, f"rp_{label}.cpp")
    with open(src, "w") as f:
        f.write(code)
    obj = os.path.join(BUILD, f"rp_{label}.obj")
    r1 = run(f'cl {FLAGS} {INCS} {DEFS} -Fo"{obj}" "{src}"')
    if r1.returncode != 0:
        print(f"{label}: COMPILE FAIL")
        continue
    exe = os.path.join(BUILD, f"rp_{label}.exe")
    r2 = run(f'link /nologo /out:"{exe}" "{obj}" {LINKLIBS}')
    if r2.returncode != 0:
        errs = [l for l in (r2.stdout + r2.stderr).splitlines() if "error" in l.lower()]
        print(f"{label}: LINK FAIL: {errs[-1] if errs else '?'}")
        continue
    r3 = run(f'"{exe}"')
    out = (r3.stdout + r3.stderr).strip()
    print(f"{label}: RC={r3.returncode} out={out[:200]}")
