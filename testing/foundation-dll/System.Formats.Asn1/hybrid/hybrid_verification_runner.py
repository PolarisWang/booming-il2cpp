"""Hybrid dispatch verification runner — stage 14 integration.

Validates that AOT-compiled methods function correctly through the
HybridPrecode dispatch pipeline:
  Phase 1 (AOT):      Cold calls produce correct AOT results
  Phase 2 (Upgrade):  Counter exhaustion triggers JIT compilation
  Phase 3 (JIT):      Subsequent calls return JIT-compiled code

Usage:
  python hybrid_verification_runner.py --family <family_path> [--verbose]
"""

from __future__ import annotations

import argparse
import json
import os
import subprocess
import sys
import time
from pathlib import Path

_HERE = Path(__file__).resolve().parent

# Expected results by phase
PHASE_AOT_RESULT = 42    # AotReturn42
PHASE_JIT_RESULT = 77    # MakeReturnConstantMethod(77)


def verify_hybrid_dispatch(test_exe: Path) -> dict:
    """Run the hybrid mode end-to-end tests and return verification results."""
    if not test_exe.exists():
        return {
            "status": "skip",
            "reason": f"Test executable not found: {test_exe}",
        }

    start = time.perf_counter()
    result = subprocess.run(
        [str(test_exe)],
        capture_output=True,
        text=True,
        timeout=120,
    )
    elapsed = time.perf_counter() - start

    passed = "[  PASSED  ]" in result.stdout
    failed_count = 0
    for line in result.stdout.splitlines():
        if "FAILED" in line or "FAILED TEST" in line:
            try:
                failed_count = int(line.split()[0])
            except (ValueError, IndexError):
                failed_count = 1

    return {
        "status": "pass" if passed else "fail",
        "stdout": result.stdout[-2000:] if result.stdout else "",
        "stderr": result.stderr[-2000:] if result.stderr else "",
        "elapsed_seconds": round(elapsed, 3),
        "exit_code": result.returncode,
        "failed_tests": failed_count,
    }


def main() -> int:
    parser = argparse.ArgumentParser(description="Hybrid dispatch verification runner")
    parser.add_argument("--family", type=str, default=str(_HERE),
                        help="Path to family directory")
    parser.add_argument("--verbose", action="store_true",
                        help="Verbose output")
    args = parser.parse_args()

    family_path = Path(args.family)
    results = {}

    # Locate pre-built hybrid test executables
    test_exes = [
        family_path / "native" / "test_jit_hybrid_mode.exe",
        family_path / "native" / "test_jit_hybrid_e2e.exe",
        family_path / "native" / "test_jit_hybrid_race.exe",
    ]

    for test_exe in test_exes:
        test_name = test_exe.name.replace(".exe", "")
        if args.verbose:
            print(f"  Running {test_name}...", file=sys.stderr)
        results[test_name] = verify_hybrid_dispatch(test_exe)

    # Write verification report
    report = {
        "family": str(family_path),
        "verification_type": "hybrid-dispatch",
        "phases_verified": ["aot", "upgrade", "jit"],
        "timestamp": time.strftime("%Y-%m-%dT%H:%M:%S"),
        "results": results,
        "summary": {
            "total": len(results),
            "passed": sum(1 for r in results.values() if r["status"] == "pass"),
            "failed": sum(1 for r in results.values() if r["status"] == "fail"),
            "skipped": sum(1 for r in results.values() if r["status"] == "skip"),
        },
    }

    report_path = family_path / "hybrid-verification-report.json"
    with open(report_path, "w") as f:
        json.dump(report, f, indent=2)

    if args.verbose:
        print(json.dumps(report, indent=2), file=sys.stderr)

    overall = report["summary"]["failed"] == 0 and report["summary"]["passed"] > 0
    return 0 if overall else 1


if __name__ == "__main__":
    sys.exit(main())
