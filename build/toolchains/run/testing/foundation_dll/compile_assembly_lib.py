"""Compile per-assembly IL2CPP generated C++ into a static library (.lib).

For each foundation DLL assembly that has been translated via convert-to-cpp,
compiles native-aot.generated.cpp + runtime_stubs.cpp into an assembly-level
.lib that per-family test executables can link against.

Usage:
    python compile_assembly_lib.py --assembly System.Linq
    python compile_assembly_lib.py --all
"""

from __future__ import annotations

import argparse
import glob
import os
import subprocess
import sys
from pathlib import Path

_HERE = Path(__file__).resolve().parent
_REPO_ROOT = _HERE.parents[4]  # D:/agent/booming-il2cpp
_VERIFICATION_BASE = _REPO_ROOT / "verification" / "foundation-dll"


def _find_vcvars() -> Path | None:
    candidates = [
        Path("C:/Program Files/Microsoft Visual Studio/2022/Professional/VC/Auxiliary/Build/vcvarsall.bat"),
        Path("C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Auxiliary/Build/vcvarsall.bat"),
    ]
    return next((c for c in candidates if c.exists()), None)


def compile_assembly_lib(assembly_name: str) -> bool:
    """Compile an assembly's generated C++ into a .lib."""
    genuine_dir = _VERIFICATION_BASE / assembly_name / "il2cpp_dist" / "genuine"
    gen_dir = genuine_dir / "generated"
    cpp_path = gen_dir / "native-aot.generated.cpp"
    runtime_stubs = _REPO_ROOT / "src" / "native" / "runtime-core" / "runtime_stubs.cpp"

    if not cpp_path.exists():
        print(f"  [SKIP] {assembly_name}: no generated C++ at {cpp_path}")
        return False

    vcvars = _find_vcvars()
    if not vcvars:
        print("  [ERROR] MSVC not found")
        return False

    include_dirs = [
        _REPO_ROOT / "src" / "native" / "common",
        _REPO_ROOT / "src" / "native" / "common" / "chaos",
        _REPO_ROOT / "contracts" / "native" / "v0",  # must come BEFORE runtime-core
        _REPO_ROOT / "src" / "native" / "runtime-core",
        _REPO_ROOT / "contracts",  # for nested include paths
        _REPO_ROOT / "third_party" / "fmt" / "include",
        gen_dir,  # for types.h
    ]
    inc_flags = " ".join(f'-I"{d}"' for d in include_dirs)
    defines = "-DCHAOS_IL2CPP_CHECK -DCHAOS_IL2CPP_TRACE_ENABLED"
    compile_flags = "/nologo /std:c++20 /c /EHsc /W3 /utf-8 /O2"

    build_dir = genuine_dir / "build"
    build_dir.mkdir(parents=True, exist_ok=True)
    lib_path = genuine_dir / f"{assembly_name}.il2cpp.lib"
    obj1 = build_dir / "native-aot.generated.obj"
    obj2 = build_dir / "runtime_stubs.obj"

    vcvars_prefix = f'call "{vcvars}" x64 >nul 2>nul &&'

    # Step 1: Compile native-aot.generated.cpp
    print(f"  Compiling {assembly_name}/native-aot.generated.cpp...")
    r = subprocess.run(
        f'{vcvars_prefix} cl {compile_flags} {inc_flags} {defines} -Fo"{obj1}" "{cpp_path}"',
        shell=True, capture_output=True, text=True, timeout=120,
    )
    if r.returncode != 0:
        err = [l for l in (r.stdout + r.stderr).splitlines() if l.strip() and "Microsoft" not in l and "chaos_ip" not in l][-5:]
        for e in err:
            print(f"    {e}")
        return False

    # Step 2: Compile runtime_stubs.cpp
    print(f"  Compiling runtime_stubs.cpp...")
    r = subprocess.run(
        f'{vcvars_prefix} cl {compile_flags} {inc_flags} {defines} -Fo"{obj2}" "{runtime_stubs}"',
        shell=True, capture_output=True, text=True, timeout=120,
    )
    if r.returncode != 0:
        err = [l for l in (r.stdout + r.stderr).splitlines() if l.strip() and "Microsoft" not in l][-3:]
        for e in err:
            print(f"    {e}")
        return False

    # Step 3: Link into .lib
    print(f"  Creating {assembly_name}.il2cpp.lib...")
    r = subprocess.run(
        f'{vcvars_prefix} lib /nologo /out:"{lib_path}" "{obj1}" "{obj2}"',
        shell=True, capture_output=True, text=True, timeout=60,
    )
    if r.returncode != 0 or not lib_path.exists():
        print(f"    LIB failed: {r.stderr[:200]}")
        return False

    size = lib_path.stat().st_size
    print(f"  OK: {assembly_name}.il2cpp.lib ({size} bytes)")
    return True


def main():
    parser = argparse.ArgumentParser(description="Compile assembly IL2CPP .lib")
    parser.add_argument("--assembly", help="Assembly name to compile")
    parser.add_argument("--all", action="store_true", help="Compile all assemblies with generated C++")
    args = parser.parse_args()

    assemblies = []
    if args.assembly:
        assemblies = [args.assembly]
    elif args.all:
        for d in sorted(_VERIFICATION_BASE.iterdir()):
            if d.is_dir() and (d / "il2cpp_dist" / "genuine" / "generated" / "native-aot.generated.cpp").exists():
                assemblies.append(d.name)
    else:
        print("Specify --assembly <name> or --all")
        sys.exit(1)

    print(f"Compiling IL2CPP .lib for {len(assemblies)} assemblies...")
    passed = 0
    failed = 0
    for asm in assemblies:
        print(f"\n{asm}:")
        if compile_assembly_lib(asm):
            passed += 1
        else:
            failed += 1

    print(f"\n{'='*60}")
    print(f"{passed} passed, {failed} failed, {len(assemblies)} total")
    return 0 if failed == 0 else 1


if __name__ == "__main__":
    main()
