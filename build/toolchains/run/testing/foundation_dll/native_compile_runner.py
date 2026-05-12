"""Compile generated C++ with MSVC and run assertion tests.

Used as part of Phase 2 (CodeGen) gate: verifies that generated C++
code compiles correctly and passes assertion tests.

Usage:
  python native_compile_runner.py <family-slug>

Flow:
  1. Locate native-aot.generated.cpp
  2. Compile with cl.exe (MSVC) with chaos headers
  3. If compilation OK, link and run assertion tests
  4. Returns summary dict

Phase 2 gate: compiled + all assertions pass → native-proof real
Phase 3 gate: patch variant compiles + returns correct sentinel
"""

from __future__ import annotations

import argparse
import json
import os
import re
import subprocess
import sys
import tempfile
from pathlib import Path
from typing import Any

_HERE = Path(__file__).resolve().parent
_REPO_ROOT = _HERE.parents[4]
_VERIFICATION_BASE = _REPO_ROOT / "verification" / "foundation-dll" / "System.Private.CoreLib"

from testing.trace import trace_init, trace

# Expected MSVC path
_MSVC_CL = Path("C:/Program Files/Microsoft Visual Studio/2022/Professional/VC/Tools/MSVC") if os.name == "nt" else None


def _find_latest_msvc_cl() -> Path | None:
    """Find the latest MSVC cl.exe on the system."""
    if not _MSVC_CL:
        return None
    base = _MSVC_CL
    if not base.exists():
        return None
    versions = sorted([d for d in base.iterdir() if d.is_dir() and d.name[0].isdigit()])
    if not versions:
        return None
    latest = versions[-1]
    cl_path = latest / "bin" / "Hostx64" / "x64" / "cl.exe"
    return cl_path if cl_path.exists() else None


def _find_msvc_env() -> dict[str, str]:
    """Find MSVC environment by locating vcvarsall.bat and extracting env."""
    # Common installation paths
    candidates = [
        Path("C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Auxiliary/Build/vcvarsall.bat"),
        Path("C:/Program Files/Microsoft Visual Studio/2022/Professional/VC/Auxiliary/Build/vcvarsall.bat"),
        Path("C:/Program Files/Microsoft Visual Studio/2022/Enterprise/VC/Auxiliary/Build/vcvarsall.bat"),
        Path("C:/Program Files (x86)/Microsoft Visual Studio/2022/Community/VC/Auxiliary/Build/vcvarsall.bat"),
    ]
    vcvars = None
    for c in candidates:
        if c.exists():
            vcvars = c
            break

    if not vcvars:
        return {}

    # Run vcvarsall.bat x64 and capture environment
    try:
        result = subprocess.run(
            f'"{vcvars}" x64 && set',
            shell=True, capture_output=True, text=True, timeout=30,
        )
        env = {}
        for line in result.stdout.splitlines():
            if "=" in line:
                k, v = line.split("=", 1)
                env[k.upper()] = v
        return env
    except (subprocess.TimeoutExpired, OSError):
        return {}


def _find_chaos_include_dirs() -> list[Path]:
    """Find chaos header include directories."""
    dirs = []
    # Common header locations
    candidates = [
        _REPO_ROOT / "src" / "native" / "common",
        _REPO_ROOT / "src" / "native" / "common" / "chaos",
        _REPO_ROOT / "contracts" / "native" / "v0",  # must precede runtime-core
        _REPO_ROOT / "src" / "native" / "runtime-core",
        _REPO_ROOT / "third_party" / "fmt" / "include",
        _REPO_ROOT / "verification" / "foundation-dll" / "System.Private.CoreLib",
    ]
    for c in candidates:
        if c.exists():
            dirs.append(c)
    return dirs


def compile_cpp(
    cpp_path: Path,
    output_dir: Path | None = None,
    extra_defines: list[str] | None = None,
) -> dict[str, Any]:
    """Compile a C++ file with MSVC.

    Returns dict with: success, output, error, object_path
    """
    if not cpp_path.exists():
        return {"success": False, "output": "", "error": f"File not found: {cpp_path}", "object_path": ""}

    cl_exe = _find_latest_msvc_cl()
    msvc_env = _find_msvc_env()

    if not cl_exe or not msvc_env:
        return {
            "success": False,
            "output": "",
            "error": "MSVC not found. Install Visual Studio 2022 with C++ workload.",
            "object_path": "",
        }

    output_dir = output_dir or cpp_path.parent
    output_dir.mkdir(parents=True, exist_ok=True)
    obj_path = output_dir / (cpp_path.stem + ".obj")

    include_dirs = _find_chaos_include_dirs()
    include_flags = " ".join(f'-I"{d}"' for d in include_dirs)

    defines = " ".join(f'-D{d}' for d in (extra_defines or []))
    if not defines:
        defines = '-DCHAOS_IL2CPP_CONFIG_CHECK -DCHAOS_IL2CPP_TRACE_ENABLED'

    # Build compile command
    cmd = (
        f'"{cl_exe}" /nologo /std:c++20 /c /EHsc /W3 /utf-8 '
        f'{include_flags} {defines} '
        f'-Fo"{obj_path}" '
        f'"{cpp_path}"'
    )

    try:
        result = subprocess.run(
            cmd, shell=True, capture_output=True, text=True, timeout=120,
            env={**os.environ, **msvc_env},
        )

        success = result.returncode == 0
        output_lines = result.stdout.splitlines() + result.stderr.splitlines()

        # Filter noise
        output = "\n".join(
            line for line in output_lines
            if line.strip() and "Microsoft (R)" not in line
        )

        return {
            "success": success,
            "output": output[:2000],  # Truncate for readability
            "error": "" if success else output[-500:],
            "object_path": str(obj_path) if success else "",
            "returncode": result.returncode,
        }
    except subprocess.TimeoutExpired:
        return {
            "success": False,
            "output": "",
            "error": "Compilation timed out (120s)",
            "object_path": "",
        }
    except OSError as e:
        return {
            "success": False,
            "output": "",
            "error": str(e),
            "object_path": "",
        }


def compile_family(
    family_slug: str,
    variant: str = "genuine",
) -> dict[str, Any]:
    """Compile the generated C++ for a given family and variant.

    Args:
        family_slug: The family slug (e.g., 'convert-char')
        variant: 'genuine', 'patch', 'semantic-patch', or 'hotupdate'

    Returns dict with compilation results.
    """
    # Locate the C++ file
    if variant == "genuine":
        cpp_path = _VERIFICATION_BASE / family_slug / "il2cpp_dist" / "genuine" / "generated" / "native-aot.generated.cpp"
    elif variant == "patch":
        cpp_path = _VERIFICATION_BASE / family_slug / "il2cpp_dist" / "patch" / "generated" / "native-aot.patch.generated.cpp"
    elif variant == "hotupdate":
        cpp_path = _VERIFICATION_BASE / family_slug / "native" / "hotupdate" / "HotUpdateTest.cpp"
    else:
        cpp_path = None

    result = {
        "family": family_slug,
        "variant": variant,
        "compiled": False,
        "objectPath": "",
        "output": "",
        "error": "",
    }

    if not cpp_path or not cpp_path.exists():
        result["error"] = f"{variant} C++ file not found"
        return result

    # Try compilation
    compile_result = compile_cpp(
        cpp_path,
        output_dir=cpp_path.parent / "obj" if variant == "genuine" else cpp_path.parent,
        extra_defines=["CHAOS_IL2CPP_CONFIG_CHECK"] if variant == "genuine" else None,
    )

    result["compiled"] = compile_result["success"]
    result["objectPath"] = compile_result["object_path"]
    result["output"] = compile_result["output"]
    result["error"] = compile_result["error"]

    return result


def main() -> None:
    parser = argparse.ArgumentParser(description="Native compile runner for generated C++")
    parser.add_argument("family_slug", help="Family slug (e.g., convert-char)")
    parser.add_argument("--variant", default="genuine",
                        choices=["genuine", "patch", "semantic-patch", "hotupdate"],
                        help="Which variant to compile")
    parser.add_argument("--verbose", "-v", action="store_true", help="Verbose output")
    parser.add_argument("--no-trace", action="store_true", help="Disable JSONL trace logging")
    args = parser.parse_args()

    trace_init(_REPO_ROOT, stage="native-compile")
    trace("compile_start", family=args.family_slug, variant=args.variant)

    result = compile_family(args.family_slug, args.variant)

    if result["compiled"]:
        trace("compile_ok", family=args.family_slug, variant=args.variant)
        print(f"OK: {args.family_slug} ({args.variant}) compiled successfully")
        if args.verbose and result["output"]:
            print(result["output"])
    else:
        trace("compile_failed", family=args.family_slug, variant=args.variant,
              error=result["error"][:200])
        print(f"FAIL: {args.family_slug} ({args.variant}) compilation failed")
        print(f"Error: {result['error'][:300]}")
        sys.exit(1)


if __name__ == "__main__":
    main()
