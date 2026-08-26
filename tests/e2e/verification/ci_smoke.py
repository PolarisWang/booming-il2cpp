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

# ── Root cause fix: GBK-console crash on emoji (✅/❌/⚠️/…) prints ───────
# Force UTF-8 on stdout/stderr so emoji prints never crash on a GBK console.
# See _encoding.py for rationale (shared across all engine entry points).
from verification import _encoding as _console_encoding  # noqa: E402

import sys as _b_sys
from pathlib import Path as _b_Path
for _b_d in _b_Path(__file__).resolve().parents:
    if (_b_d / "_path.py").exists():
        if str(_b_d) not in _b_sys.path:
            _b_sys.path.insert(0, str(_b_d))
        break
from _path import foundation_root



# The full 10-stage product-verification set (build→fact→hotupdate→coverage-audit→
# profile→benchmark→managed_benchmark→benchmark_report→aggregate→reporting; chunk_pipeline
# topo-sorts by STAGE_DEPS).  Shared by both `full` (core+standard) and `extended`
# (all families) modes so every mode that claims comprehensive coverage really runs it.
_FULL_STAGES = ("build,fact,hotupdate,coverage-audit,profile,"
                "benchmark,managed_benchmark,benchmark_report,aggregate,reporting")


def main() -> int:
    import argparse

    parser = argparse.ArgumentParser(description="Foundation-DLL CI smoke test")
    parser.add_argument("--mode", default="smoke", choices=["smoke", "full", "extended"],
                        help="smoke=core families only (default), full=core+standard, extended=all families")
    parser.add_argument("--families", default=None,
                        help="Comma-separated override of specific families to run (e.g. 'System.Private.CoreLib,System.Linq')")
    parser.add_argument("--verbose", action="store_true", help="Verbose output")
    parser.add_argument("--check-regression", action="store_true",
                        help="Run benchmark regression check after pipeline")
    args = parser.parse_args()

    _HERE = Path(__file__).resolve().parent
    _FOUNDATION_DLL = foundation_root()
    os.chdir(str(_FOUNDATION_DLL))
    if str(_FOUNDATION_DLL) not in sys.path:
        sys.path.insert(0, str(_FOUNDATION_DLL))

    # Tiered family lists
    # Core: critical paths, runs every smoke and full
    CORE_FAMILIES = [
        "System.Private.CoreLib",
    ]
    # Standard: high-traffic assemblies, runs in `full` mode
    STANDARD_FAMILIES = [
        "System.Collections.NonGeneric",
        "System.Collections.Immutable",
        "System.Linq",
        "System.Linq.Expressions",
        "System.Runtime",
        "System.Runtime.InteropServices",
        "System.Text.Json",
        "System.Net.Http",
        "System.Net.Sockets",
        "System.ObjectModel",
        "System.IO.Compression.ZipFile",
        "System.Security.Cryptography",
        "System.Threading.Tasks.Parallel",
        "System.Xml.ReaderWriter",
    ]
    # Extended: remaining assemblies, runs with `--families extended` or `all`
    EXTENDED_FAMILIES = [
        "System.ComponentModel.TypeConverter",
        "System.Data.Common",
        "System.Diagnostics.DiagnosticSource",
        "System.Formats.Asn1",
        "System.IO.Compression.Brotli",
        "System.IO.Pipelines",
        "System.Net.ServerSentEvents",
        "System.Private.Xml",
        "System.Reflection.Metadata",
        "System.Runtime.Intrinsics",
        "System.Runtime.Serialization.Formatters",
        "System.Security.Claims",
        "System.Security.Principal.Windows",
    ]
    ALL_FAMILIES = CORE_FAMILIES + STANDARD_FAMILIES + EXTENDED_FAMILIES

    from verification.chunk_pipeline import main as pipeline_main
    from verification.benchmark_diff import _load_comparison, _diff_aggregate, _print_report

    overall_start = time.perf_counter()
    failures = []

    # Determine which families to run
    if args.mode == "smoke":
        families = CORE_FAMILIES
        stages = "build,fact,coverage-audit"
        print(f"CI Smoke mode: {len(families)} core family(ies), stages=[{stages}]")
    elif args.mode == "full":
        families = CORE_FAMILIES + STANDARD_FAMILIES
        stages = _FULL_STAGES
        print(f"CI Full mode: {len(families)} families (core+standard), stages=[{stages}]")
    elif args.mode == "extended":
        # Comprehensive: ALL families × the FULL stage set.  Pre-2026 extended only ran
        # build/fact/coverage-audit (no benchmark/hotupdate/profile/aggregate/reporting),
        # so "extended" covered more families but did NOT actually product-verify them.
        # Now it shares the same full stage list as `full`, just over all 29 families.
        families = ALL_FAMILIES
        stages = _FULL_STAGES
        print(f"CI Extended mode: {len(families)} families (all), stages=[{stages}]")

    # Override: --families takes precedence over --mode
    if args.families:
        families = [f.strip() for f in args.families.split(",") if f.strip()]
        print(f"CI override: {len(families)} specific family(ies)")

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
            # strict: coverage-audit fails only when >5% of a chunk's methods are
            # missing (coverage_audit.py:106).  Default (standard) reds on ANY missing
            # method, which false-flags nearly-complete families.  strict gives the
            # proper 5% tier so the gate shows real coverage gaps, not noise.
            "--mode", "strict",
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
