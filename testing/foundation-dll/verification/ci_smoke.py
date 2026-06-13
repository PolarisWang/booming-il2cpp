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
    parser.add_argument("--check-regression", action="store_true",
                        help="Run benchmark regression check after pipeline")
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
    from verification.benchmark_diff import _load_comparison, _diff_aggregate, _print_report

    overall_start = time.perf_counter()
    failures = []

    if args.mode == "smoke":
        families = SMOKE_FAMILIES
        stages = "build,fact,coverage-audit"
        print(f"CI Smoke mode: {len(families)} families, stages=[{stages}]")
    else:
        families = ["System.Private.CoreLib"]
        stages = "build,fact,hotupdate,coverage-audit,benchmark,benchmark_report"
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

    # ── Post-pipeline: benchmark regression check ──
    regression_failures = []
    if args.check_regression:
        print(f"\n{'=' * 60}")
        print(f"Benchmark Regression Check")
        print(f"{'=' * 60}")
        for family in families:
            results_dir = _FOUNDATION_DLL / family / "_dll" / "reports" / "history"
            if not results_dir.is_dir():
                print(f"  [regression] No history dir for {family}: {results_dir}")
                continue
            history_files = sorted(results_dir.glob("comparison-*.json"))
            if len(history_files) < 2:
                print(f"  [regression] Need at least 2 history files for {family} (found {len(history_files)})")
                continue
            baseline = _load_comparison(history_files[-2])
            target = _load_comparison(history_files[-1])
            diff_result = _diff_aggregate(baseline, target)
            regressed = [d for d in diff_result["perChunk"]
                         if d.get("deltaChaosAotPct") is not None
                         and d["deltaChaosAotPct"] <= -10]
            if regressed:
                regression_failures.append(family)
                print(f"  [regression] ❌ {family}: {len(regressed)} chunk(s) regressed")
                for r in regressed:
                    print(f"    - {r['slug']}: {r['baselineChaosAotMeanPct']}% -> {r['targetChaosAotMeanPct']}% ({r['deltaChaosAotPct']:+.2f}pp)")
            else:
                print(f"  [regression] ✅ {family}: no regression detected")

    total_duration = time.perf_counter() - overall_start
    print(f"\n{'=' * 60}")
    print(f"CI {'Smoke' if args.mode == 'smoke' else 'Full'} complete: "
          f"{len(families) - len(failures)}/{len(families)} passed")
    if regression_failures:
        print(f"Regression failures: {', '.join(regression_failures)}")
    print(f"Duration: {total_duration:.0f}s")

    if failures:
        print(f"FAILED families: {', '.join(failures)}")
        return 1
    if regression_failures:
        print(f"REGRESSED families: {', '.join(regression_failures)}")
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
