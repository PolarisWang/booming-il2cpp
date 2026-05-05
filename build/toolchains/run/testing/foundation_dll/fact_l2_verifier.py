"""Fact L2: Semantic Correctness Verification.

Verifies that the generated native AOT C++ code produces correct results
by comparing the output of the native executable against the expected
checksums from the managed C# entrypoint.

Usage:
    python fact_l2_verifier.py <family-slug>
    python fact_l2_verifier.py convert-char --verbose
    python fact_l2_verifier.py reflection-type --assembly System.Private.CoreLib

Flow:
  1. Build & run C# entrypoint -> expected_checksums[]
  2. Build native AOT executable -> actual_checksums[]
  3. Compare: expected[N] == actual[N] for all N
"""

from __future__ import annotations

import argparse
import json
import os
import re
import shutil
import subprocess
import sys
import tempfile
import time
from pathlib import Path
from typing import Any

_HERE = Path(__file__).resolve().parent
_REPO_ROOT = _HERE.parents[4]
_VERIFICATION_BASE = _REPO_ROOT / "verification" / "foundation-dll"


# ── Step 1: Get expected checksums from C# entrypoint ────────────────────

def _get_method_count(family_slug: str, *, assembly: str) -> int:
    """Read method count from the C# entrypoint source (count cases in switch)."""
    entry_dir = _VERIFICATION_BASE / assembly / family_slug / "il2cpp_dist" / "entrypoint"
    cs_files = list(entry_dir.glob("*NativeEntry.cs")) + list(entry_dir.glob("*Entry.cs"))
    if not cs_files:
        return 0
    source = cs_files[0].read_text(encoding="utf-8")
    # Count the switch cases (case N: return MethodN())
    cases = re.findall(r'case (\d+):', source)
    return max(int(c) for c in cases) + 1 if cases else 0


_L2_HARNESS_DIR = _HERE / "l2-harness"
_L2_HARNESS_BUILT: bool = False


def _ensure_l2_harness() -> bool:
    """Build the L2 reflection-based harness once and reuse."""
    global _L2_HARNESS_BUILT
    if _L2_HARNESS_BUILT:
        return True
    result = subprocess.run(
        ["dotnet", "build", str(_L2_HARNESS_DIR / "L2Harness.csproj"), "--nologo", "-v", "q"],
        capture_output=True, text=True, timeout=60,
    )
    _L2_HARNESS_BUILT = result.returncode == 0
    return _L2_HARNESS_BUILT


def _get_expected_checksums(family_slug: str, *, assembly: str) -> list[int]:
    """Run the C# entrypoint DLL via reflection harness and collect expected checksums."""
    entry_dir = _VERIFICATION_BASE / assembly / family_slug / "il2cpp_dist" / "entrypoint"
    cs_path = next(entry_dir.glob("*NativeEntry.cs"), None)
    dll_candidates = list(entry_dir.glob("build-output/*.dll"))
    if not dll_candidates:
        dll_candidates = list(entry_dir.glob("*.dll"))
    dll_path = dll_candidates[0] if dll_candidates else None

    if not dll_path or not dll_path.exists() or cs_path is None:
        print(f"  [L2] Entrypoint DLL not found for {family_slug}")
        return []

    method_count = _get_method_count(family_slug, assembly=assembly)
    if method_count == 0:
        return []

    if not _ensure_l2_harness():
        print(f"  [L2] Failed to build L2 harness")
        return []

    class_name = cs_path.stem
    result = subprocess.run(
        ["dotnet", "run", "--project", str(_L2_HARNESS_DIR / "L2Harness.csproj"),
         "--no-build", "--", str(dll_path), class_name, str(method_count)],
        capture_output=True, text=True, timeout=120,
    )
    if result.returncode != 0:
        print(f"  [L2] Runner failed: {result.stderr[:300]}")
        return []
    try:
        return json.loads(result.stdout.strip())
    except json.JSONDecodeError as e:
        print(f"  [L2] JSON parse error: {e}")
        print(f"  stdout: {result.stdout[:500]}")
        return []


# ── Step 2: Get actual checksums from native executable ────────────────

def _find_latest_msvc_cl() -> Path | None:
    """Find the latest MSVC cl.exe."""
    base_paths = [
        Path("C:/Program Files/Microsoft Visual Studio/2022/Professional/VC/Tools/MSVC"),
        Path("C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC"),
    ]
    for base in base_paths:
        if base.exists():
            versions = sorted([d for d in base.iterdir() if d.is_dir() and d.name[0].isdigit()])
            if versions:
                cl = versions[-1] / "bin" / "Hostx64" / "x64" / "cl.exe"
                if cl.exists():
                    return cl
    return None


def _find_vcvars() -> Path | None:
    """Find vcvarsall.bat path."""
    candidates = [
        Path("C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Auxiliary/Build/vcvarsall.bat"),
        Path("C:/Program Files/Microsoft Visual Studio/2022/Professional/VC/Auxiliary/Build/vcvarsall.bat"),
    ]
    return next((c for c in candidates if c.exists()), None)


def _find_msvc_env() -> dict[str, str]:
    """Find MSVC environment via vcvarsall.bat."""
    vcvars = _find_vcvars()
    if not vcvars:
        return {}
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


def _build_and_run_native(
    family_slug: str,
    *,
    assembly: str,
    verbose: bool = False,
) -> list[int]:
    """Build the native AOT executable and run each entry to get actual checksums."""
    family_dir = _VERIFICATION_BASE / assembly / family_slug
    generated_cpp = family_dir / "il2cpp_dist" / "genuine" / "generated" / "native-aot.generated.cpp"
    runtime_stubs = _REPO_ROOT / "src" / "native" / "runtime-core" / "runtime_stubs.cpp"
    benchmark_host = _REPO_ROOT / "src" / "native" / "benchmark-host" / "native_aot_main.cpp"

    if not generated_cpp.exists():
        print(f"  [L2] Generated C++ not found at {generated_cpp}")
        return []

    method_count = _get_method_count(family_slug, assembly=assembly)
    if method_count == 0:
        return []

    vcvars = _find_vcvars()
    if not vcvars:
        print("  [L2] MSVC not found. Install Visual Studio 2022 with C++ workload.")
        return []
    vcvars_prefix = f'"{vcvars}" x64 >nul 2>nul &&'

    include_dirs = [
        _REPO_ROOT / "src" / "native" / "common",
        _REPO_ROOT / "src" / "native" / "common" / "chaos",
        _REPO_ROOT / "src" / "native" / "runtime-core",
        _REPO_ROOT / "contracts" / "native" / "v0",
        _REPO_ROOT / "third_party" / "fmt" / "include",
    ]
    include_flags = " ".join(f'-I"{d}"' for d in include_dirs)

    build_dir = family_dir / "native_test" / "l2-verify" / "build"
    build_dir.mkdir(parents=True, exist_ok=True)
    exe_path = build_dir / f"verify_{family_slug}.exe"

    compile_flags = "/nologo /std:c++20 /c /EHsc /W3 /utf-8 /O2"
    link_flags = "/nologo /EHsc /utf-8 /O2"
    defines = "-DCHAOS_IL2CPP_CHECK -DCHAOS_IL2CPP_TRACE_ENABLED"

    # Compile each source with vcvars in same shell
    source_files = [benchmark_host, generated_cpp, runtime_stubs]
    obj_files = []
    for src in source_files:
        obj = build_dir / f"{src.stem}.obj"
        obj_files.append(obj)
        cmd = f'{vcvars_prefix} cl {compile_flags} {include_flags} {defines} -Fo"{obj}" "{src}"'
        r = subprocess.run(cmd, shell=True, capture_output=True, text=True, timeout=120)
        if r.returncode != 0:
            err = [l for l in (r.stdout + r.stderr).splitlines() if l.strip() and "Microsoft" not in l]
            print(f"  [L2] Compile failed for {src.name}:\n" + "\n".join(err[-5:]))
            return []
        if verbose:
            print(f"  [L2]   Compiled {src.name}")

    # Link (find link.exe next to cl.exe, use captured env)
    cl_exe = _find_latest_msvc_cl()
    link_exe = cl_exe.parent / "link.exe" if cl_exe else None
    if not link_exe or not link_exe.exists():
        print("  [L2] MSVC linker not found")
        return []
    msvc_env = _find_msvc_env()
    obj_list = " ".join(f'"{o}"' for o in obj_files)
    link_cmd = f'"{link_exe}" /nologo /out:"{exe_path}" {obj_list}'
    r = subprocess.run(link_cmd, shell=True, capture_output=True, text=True, timeout=60,
                       env={**os.environ, **msvc_env})
    if r.returncode != 0:
        err = [l for l in r.stderr.splitlines() if l.strip() and "Microsoft" not in l]
        print(f"  [L2] Link failed:\n" + "\n".join(err[-5:]))
        return []

    if verbose:
        size = exe_path.stat().st_size if exe_path.exists() else 0
        print(f"  [L2]   Linked: {exe_path.name} ({size} bytes)")

    # Run each method
    results: list[int] = []
    for idx in range(method_count):
        cmd = [str(exe_path), "--entry-index", str(idx), "--iterations", "1"]
        try:
            r = subprocess.run(cmd, capture_output=True, text=True, timeout=30)
            if r.returncode == 0:
                data = json.loads(r.stdout.strip())
                checksum = data.get("checksum", -1)
                results.append(checksum)
                if verbose:
                    print(f"  [L2]   [{idx}] checksum={checksum}")
            else:
                results.append(-1)
                if verbose:
                    print(f"  [L2]   [{idx}] FAILED (rc={r.returncode})")
        except (subprocess.TimeoutExpired, json.JSONDecodeError, OSError) as e:
            results.append(-1)
            if verbose:
                print(f"  [L2]   [{idx}] ERROR: {e}")

    return results


# ── Step 3: Compare ─────────────────────────────────────────────────────

def _compare_checksums(expected: list[int], actual: list[int], *, verbose: bool = False) -> dict[str, Any]:
    """Compare expected vs actual checksums. Return per-method and aggregate results."""
    results = []
    passed = 0
    failed = 0
    max_len = min(len(expected), len(actual))

    for i in range(max_len):
        match = expected[i] == actual[i]
        if match:
            passed += 1
        else:
            failed += 1
        results.append({
            "index": i,
            "expected": expected[i],
            "actual": actual[i],
            "passed": match,
        })

    # Handle length mismatch
    for i in range(max_len, len(expected)):
        failed += 1
        results.append({"index": i, "expected": expected[i], "actual": None, "passed": False})
    for i in range(max_len, len(actual)):
        failed += 1
        results.append({"index": i, "expected": None, "actual": actual[i], "passed": False})

    return {
        "total": len(expected),
        "passed": passed,
        "failed": failed,
        "passed_pct": round(passed / len(expected) * 100, 1) if expected else 0.0,
        "results": results,
    }


# ── Main ────────────────────────────────────────────────────────────────

def verify_family(family_slug: str, *, assembly: str = "System.Private.CoreLib",
                  verbose: bool = False) -> dict[str, Any]:
    """Run L2 verification for a single family."""

    print(f"=== Fact L2: {family_slug} ===")
    trace("fact_l2.verify", stage="proof", family=family_slug)

    method_count = _get_method_count(family_slug, assembly=assembly)
    if method_count == 0:
        print(f"  No methods found for {family_slug}")
        return {"family": family_slug, "status": "skip", "reason": "no methods"}

    print(f"  Methods: {method_count}")

    # Step 1: Expected
    print(f"  [1/3] Getting expected checksums from C# entrypoint...")
    expected = _get_expected_checksums(family_slug, assembly=assembly)
    if not expected:
        print(f"  SKIP: could not get expected checksums")
        trace("fact_l2.skip", family=family_slug, reason="no expected checksums")
        return {"family": family_slug, "status": "skip", "reason": "no expected checksums"}
    print(f"  [1/3] Got {len(expected)} expected checksums")

    # Step 2: Actual
    print(f"  [2/3] Building native AOT and running...")
    actual = _build_and_run_native(family_slug, assembly=assembly, verbose=verbose)
    if not actual:
        print(f"  SKIP: could not run native executable")
        trace("fact_l2.skip", family=family_slug, reason="native build/run failed")
        return {"family": family_slug, "status": "skip", "reason": "native build/run failed"}
    print(f"  [2/3] Got {len(actual)} native checksums")

    # Step 3: Compare
    print(f"  [3/3] Comparing checksums...")
    comparison = _compare_checksums(expected, actual, verbose=verbose)
    trace("fact_l2.done", family=family_slug, passed=comparison["passed"],
          failed=comparison["failed"], total=comparison["total"])

    print(f"  [3/3] Result: {comparison['passed']}/{comparison['total']} passed "
          f"({comparison['passed_pct']}%)")
    if comparison["failed"] > 0:
        print(f"  FAILED methods:")
        for r in comparison["results"]:
            if not r["passed"]:
                print(f"    [{r['index']}] expected={r['expected']} actual={r['actual']}")

    status = "passed" if comparison["failed"] == 0 else "failed"
    return {
        "family": family_slug,
        "status": status,
        **comparison,
    }


try:
    from testing.trace import trace
except ImportError:
    def trace(*args, **kwargs):
        pass


def main() -> None:
    parser = argparse.ArgumentParser(description="Fact L2: Semantic Correctness Verification")
    parser.add_argument("family_slug", help="Family slug (e.g., convert-char)")
    parser.add_argument("--assembly", default="System.Private.CoreLib", help="Assembly name")
    parser.add_argument("--verbose", "-v", action="store_true", help="Verbose output")
    args = parser.parse_args()

    result = verify_family(args.family_slug, assembly=args.assembly, verbose=args.verbose)
    print(f"\n=== L2 Result: {result['status']} ({result.get('passed',0)}/{result.get('total',0)}) ===")
    sys.exit(0 if result["status"] == "passed" else 1)


if __name__ == "__main__":
    main()
