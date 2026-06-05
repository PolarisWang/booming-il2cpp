#!/usr/bin/env python3
"""CI smoke entry point for foundation-dll verification pipeline.

Runs a subset of key families for fast feedback in CI (smoke mode),
or the full pipeline (full mode, used on push to main).

Usage:
    # Smoke mode (default): key families only
    python verification/ci_smoke.py

    # Full mode: all families
    python verification/ci_smoke.py --mode=full

    # Run from testing/foundation-dll/ directory
    cd testing/foundation-dll && python verification/ci_smoke.py
"""

from __future__ import annotations

import os
import sys
import time
from pathlib import Path


def main() -> int:
    import argparse

    parser = argparse.ArgumentParser(description="Foundation-DLL CI smoke test")
    parser.add_argument("--mode", default="smoke", choices=["smoke", "full"],
                        help="smoke=key families only (default), full=all families")
    parser.add_argument("--verbose", action="store_true", help="Verbose output")
    args = parser.parse_args()

    _HERE = Path(__file__).resolve().parent
    _FOUNDATION_DLL = _HERE.parent
    os.chdir(str(_FOUNDATION_DLL))
    if str(_FOUNDATION_DLL) not in sys.path:
        sys.path.insert(0, str(_FOUNDATION_DLL))

    # Key families for smoke mode — these cover the most critical paths
    SMOKE_FAMILIES = [
        "System.Private.CoreLib",
    ]

    from verification.chunk_pipeline import main as pipeline_main

    overall_start = time.perf_counter()
    failures = []

    if args.mode == "smoke":
        families = SMOKE_FAMILIES
        stages = "build,fact,coverage-audit"
        print(f"CI Smoke mode: {len(families)} families, stages=[{stages}]")
    else:
        families = ["System.Private.CoreLib"]
        stages = "build,fact,hotupdate,coverage-audit"
        print(f"CI Full mode: all families, stages=[{stages}]")

    for family in families:
        print(f"\n{'=' * 60}")
        print(f"Family: {family}")
        print(f"{'=' * 60}")

        sys.argv = [
            "ci_smoke.py",
            "--assembly", family,
            "--stages", stages,
            "--all-chunks",
            "--native-config", "check",
        ]
        if args.verbose:
            sys.argv.append("--verbose")

        rc = pipeline_main()
        if rc != 0:
            failures.append(family)

    total_duration = time.perf_counter() - overall_start
    print(f"\n{'=' * 60}")
    print(f"CI {'Smoke' if args.mode == 'smoke' else 'Full'} complete: "
          f"{len(families) - len(failures)}/{len(families)} passed")
    print(f"Duration: {total_duration:.0f}s")

    if failures:
        print(f"FAILED families: {', '.join(failures)}")
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
