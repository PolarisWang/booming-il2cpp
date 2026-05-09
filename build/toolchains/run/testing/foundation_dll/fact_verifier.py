"""Fact Verification: run il2cpp-translated native entry EXE directly.

No checksums, no C++ hosts, no CMake. The entry is a C# EXE translated
by il2cpp into a native executable -- just run it.

Usage:
    python fact_verifier.py <family-slug>
    python fact_verifier.py convert-char --verbose
"""

from __future__ import annotations

import argparse
import re
import subprocess
import sys
from pathlib import Path
from typing import Any

_HERE = Path(__file__).resolve().parent
_REPO_ROOT = _HERE.parents[4]
_VERIFICATION_BASE = _REPO_ROOT / "verification" / "foundation-dll"

try:
    from testing.trace import trace
except ImportError:
    def trace(*args, **kwargs):
        pass


def _locate_entry_exe(family_slug: str, *, assembly: str) -> Path | None:
    """Find the native entry EXE in il2cpp_dist/genuine/<AssemblyName>/generated/"""
    family_dir = _VERIFICATION_BASE / assembly / family_slug
    genuine_dir = family_dir / "il2cpp_dist" / "genuine"
    if not genuine_dir.exists():
        return None
    # Scan for per-assembly subdirectory
    for d in genuine_dir.iterdir():
        if d.is_dir():
            candidate = d / "generated" / "entry.exe"
            if candidate.exists():
                return candidate
    return None


def verify_fact(family_slug: str, *, assembly: str = "System.Private.CoreLib",
                method_count: int | None = None,
                verbose: bool = False) -> dict[str, Any]:
    """Run il2cpp-translated native entry EXE, verify all Assert pass."""
    print(f"=== Fact Verify: {family_slug} ===")
    exe_path = _locate_entry_exe(family_slug, assembly=assembly)
    if exe_path is None:
        print(f"  [Fact] Entry EXE not found")
        return {"status": "skip", "reason": "entry EXE not found"}

    r = subprocess.run([str(exe_path)], capture_output=True, text=True, timeout=120)
    output = r.stdout + r.stderr

    # Parse "Passed: N/M" from output
    passed = total = 0
    for line in output.splitlines():
        m = re.search(r'Passed:\s*(\d+)/(\d+)', line)
        if m:
            passed, total = int(m.group(1)), int(m.group(2))
        if "FAIL" in line:
            print(f"  {line}")

    # Fallback: if no "Passed: N/M" found and we know total, derive from bitmask exit code
    if passed == 0 and total == 0 and method_count is not None:
        total = method_count
        mask = (1 << total) - 1
        failures = (r.returncode & mask).bit_count()
        passed = total - failures

    status = "passed" if r.returncode == 0 else "failed"
    trace("fact.verify", stage="proof", family=family_slug,
          status=status, passed=passed, total=total)

    print(f"  [Fact] Native verify: {status} ({passed}/{total})")
    if r.returncode != 0 and verbose:
        print(output)

    return {
        "status": status,
        "passed": passed,
        "total": total,
        "exit_code": r.returncode,
    }


def verify_benchmark(family_slug: str, *, assembly: str = "System.Private.CoreLib",
                     entry_index: int = 0, iterations: int = 1000,
                     verbose: bool = False) -> dict[str, Any]:
    """Run benchmark via il2cpp-translated entry EXE."""
    print(f"=== Fact Benchmark: {family_slug} ===")
    exe_path = _locate_entry_exe(family_slug, assembly=assembly)
    if exe_path is None:
        return {"status": "skip", "reason": "entry EXE not found"}

    r = subprocess.run(
        [str(exe_path), "--benchmark", str(entry_index), str(iterations)],
        capture_output=True, text=True, timeout=300)

    output = r.stdout + r.stderr
    elapsed_ms = 0.0
    for line in output.splitlines():
        if "Benchmark" in line:
            m = re.search(r'([\d.]+)\s*ms', line)
            if m:
                elapsed_ms = float(m.group(1))

    status = "completed" if r.returncode == 0 else "failed"
    print(f"  [Benchmark] {status}: {elapsed_ms:.1f}ms ({iterations} iterations)")
    return {"status": status, "elapsed_ms": elapsed_ms, "iterations": iterations,
            "exit_code": r.returncode}


def verify_hotupdate(family_slug: str, *, assembly: str = "System.Private.CoreLib",
                     verbose: bool = False) -> dict[str, Any]:
    """Run hotupdate verification via il2cpp-translated entry EXE."""
    print(f"=== Fact HotUpdate: {family_slug} ===")
    exe_path = _locate_entry_exe(family_slug, assembly=assembly)
    if exe_path is None:
        return {"status": "skip", "reason": "entry EXE not found"}

    r = subprocess.run(
        [str(exe_path), "--hotupdate"],
        capture_output=True, text=True, timeout=120)

    output = r.stdout + r.stderr
    passed = total = 0
    for line in output.splitlines():
        m = re.search(r'Passed:\s*(\d+)/(\d+)', line)
        if m:
            passed, total = int(m.group(1)), int(m.group(2))
        if "FAIL" in line:
            print(f"  {line}")

    status = "passed" if r.returncode == 0 else "failed"
    print(f"  [HotUpdate] Native verify: {status} ({passed}/{total})")
    return {"status": status, "passed": passed, "total": total,
            "exit_code": r.returncode}


def main() -> None:
    parser = argparse.ArgumentParser(description="Fact Verification")
    parser.add_argument("family_slug", help="Family slug (e.g., convert-char)")
    parser.add_argument("--assembly", default="System.Private.CoreLib")
    parser.add_argument("--verbose", "-v", action="store_true")
    parser.add_argument("--benchmark", action="store_true")
    parser.add_argument("--hotupdate", action="store_true")
    parser.add_argument("--entry-index", type=int, default=0)
    parser.add_argument("--iterations", type=int, default=1000)
    args = parser.parse_args()

    if args.hotupdate:
        result = verify_hotupdate(args.family_slug, assembly=args.assembly, verbose=args.verbose)
    elif args.benchmark:
        result = verify_benchmark(args.family_slug, assembly=args.assembly,
                                  entry_index=args.entry_index, iterations=args.iterations,
                                  verbose=args.verbose)
    else:
        result = verify_fact(args.family_slug, assembly=args.assembly, verbose=args.verbose)
    sys.exit(0 if result.get("status") in ("passed", "completed") else 1)


if __name__ == "__main__":
    main()