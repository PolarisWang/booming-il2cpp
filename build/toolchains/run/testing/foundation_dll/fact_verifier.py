"""Fact Verification: run il2cpp-translated native entry EXE directly.

No checksums, no C++ hosts, no CMake. The entry is a C# EXE translated
by il2cpp into a native executable -- just run it.

Usage:
    python fact_verifier.py <family-slug>
    python fact_verifier.py convert-char --verbose
"""

from __future__ import annotations

import argparse
import json as _json
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
    """Find the native entry EXE in native/entry.exe"""
    family_dir = _VERIFICATION_BASE / assembly / family_slug
    candidate = family_dir / "native" / "entry.exe"
    return candidate if candidate.exists() else None


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

    elapsed_ms = 0.0
    calibrated_ms = 0.0
    ops_per_sec = 0.0
    output = r.stdout.strip()
    # Parse JSON from last output line
    for line in output.splitlines():
        try:
            obj = _json.loads(line.strip())
            if "elapsedMilliseconds" in obj:
                elapsed_ms = obj["elapsedMilliseconds"]
                calibrated_ms = obj.get("calibratedMs", elapsed_ms)
                ops_per_sec = obj.get("opsPerSecond", 0.0)
                iterations = obj.get("iterations", iterations)
        except (_json.JSONDecodeError, ValueError):
            pass

    status = "completed" if r.returncode == 0 else "failed"
    cal_note = f" (cal={calibrated_ms:.3f}ms)" if calibrated_ms != elapsed_ms else ""
    print(f"  [Benchmark] {status}: {elapsed_ms:.3f}ms{cal_note}, {ops_per_sec:.0f} ops/s ({iterations} iterations)")
    return {"status": status, "elapsed_ms": elapsed_ms, "calibrated_ms": calibrated_ms,
            "iterations": iterations, "ops_per_sec": ops_per_sec, "exit_code": r.returncode}


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

    passed = total = 0
    output = r.stdout.strip()
    for line in output.splitlines():
        try:
            obj = _json.loads(line.strip())
            passed = obj.get("passedMethods", 0)
            total = obj.get("totalMethods", 0)
        except (_json.JSONDecodeError, ValueError):
            pass
    if total == 0:
        # Fallback: try parsing "Passed: N/M"
        for line in output.splitlines():
            m = re.search(r'Passed:\s*(\d+)/(\d+)', line)
            if m:
                passed, total = int(m.group(1)), int(m.group(2))

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