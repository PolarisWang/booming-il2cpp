"""Build and run isolation tests to narrow down fact_static crash."""
import subprocess
import sys
from pathlib import Path

VCVARS = r"C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars64.bat"
BD = Path(r"D:\agent\booming-il2cpp\verification\foundation-dll\System.Private.CoreLib\convert-char\native_test\fact-verify\build")
REPO = Path(r"D:\agent\booming-il2cpp")

INCL = (
    f'-I{REPO / "src" / "native" / "common"} '
    f'-I{REPO / "src" / "native" / "common" / "chaos"} '
    f'-I{REPO / "contracts" / "native" / "v0"} '
    f'-I{REPO / "src" / "native" / "runtime-core"} '
    f'-I{REPO / "src" / "native" / "interpreter"} '
    f'-I{REPO / "src" / "native" / "bootstrap"} '
    f'-I{REPO / "third_party" / "bdwgc" / "include"} '
    f'-I{REPO / "third_party" / "fmt" / "include"} '
    f'-I{BD}'
)
DEFS = "-DCHAOS_IL2CPP_CHECK -DCHAOS_IL2CPP_TRACE_ENABLED -DCHAOS_IL2CPP_VERIFY_MODE -DCHAOS_RUNTIME_ABI_STATIC -DGC_NOT_DLL"
CFLAGS = "/nologo /std:c++20 /c /EHsc /utf-8 /Od /MD"


def run(cmd: str) -> subprocess.CompletedProcess:
    full_cmd = f'call "{VCVARS}" x64 >nul 2>nul && {cmd}'
    return subprocess.run(["cmd.exe", "/c", full_cmd], capture_output=True, text=True, shell=False)


def build_and_run(name: str) -> int:
    src = BD / f"{name}.cpp"
    obj = BD / f"{name}.obj"
    exe = BD / f"{name}.exe"

    print(f"\n{'='*50}")
    print(f"  {name}")
    print(f"{'='*50}")

    # Compile
    compile_cmd = f'cl {CFLAGS} {INCL} {DEFS} -Fo"{obj}" "{src}"'
    r = run(compile_cmd)
    if r.returncode != 0:
        print(f"  COMPILE FAILED (RC={r.returncode})")
        print(r.stdout[-1500:])
        print(r.stderr[-1500:])
        return r.returncode
    print(f"  Compile OK")

    # Link (no libs - standalone)
    link_cmd = f'link /nologo /out:"{exe}" "{obj}"'
    r = run(link_cmd)
    if r.returncode != 0:
        print(f"  LINK FAILED (RC={r.returncode})")
        print(r.stdout[-1500:])
        print(r.stderr[-1500:])
        return r.returncode
    print(f"  Link OK")

    # Run
    r = run(f'"{exe}"')
    out = (r.stdout + r.stderr)[:2000]
    print(f"  Output: {out}")
    print(f"  RC={r.returncode}")
    return r.returncode


if __name__ == "__main__":
    tests = ["isolate_static_init", "isolate_crash1", "isolate_crash"]
    all_ok = True
    for t in tests:
        rc = build_and_run(t)
        if rc != 0:
            all_ok = False
            print(f"  ** {t} FAILED **")

    if all_ok:
        # If isolate_crash passed (no crash from just including generated code),
        # build and run minimal_debug to test dispatch
        print("\n" + "="*50)
        print("  All basic isolates passed. Now minimal_debug...")
        print("="*50)
        rc = build_and_run("minimal_debug")
        all_ok = (rc == 0)

    sys.exit(0 if all_ok else 1)