"""CLI entry point for foundation DLL family verification.

Usage:
    python -m verification.entry_points.cli convert-char
    python -m verification.entry_points.cli convert-char --strict
    python -m verification.entry_points.cli convert-char --skip benchmark hotupdate
"""

from __future__ import annotations

import argparse
import sys
from pathlib import Path

from .._path import _HERE as _VERIFICATION_ROOT

from verification.orchestration.context import FamilyContext
from verification.orchestration.engine import VerificationPipeline
from verification.analysis.perf_store import auto_save_perf_data

_TESTING_ROOT = _VERIFICATION_ROOT.parent  # testing/


def _resolve_family_dir(slug: str, assembly: str) -> Path:
    return _TESTING_ROOT / assembly / slug


def main() -> None:
    parser = argparse.ArgumentParser(description="Foundation DLL family verification")
    parser.add_argument("family_slug", help="Family slug (e.g., convert-char)")
    parser.add_argument("--assembly", default="System.Private.CoreLib",
                        help="Assembly name (default: System.Private.CoreLib)")
    parser.add_argument("--mode", choices=["standard", "strict"], default="standard",
                        help="Verification mode (default: standard)")
    parser.add_argument("--skip", nargs="*", default=[],
                        help="Stages to skip: preflight codegen jit_codegen fact fact_jit audit asm_compare microbench benchmark hotupdate")
    parser.add_argument("--verbose", "-v", action="store_true",
                        help="Verbose output")
    parser.add_argument("--native-config", choices=["check", "profile", "ship"], default="check",
                        help="Native build config (default: check)")

    args = parser.parse_args()
    family_dir = _resolve_family_dir(args.family_slug, args.assembly)

    if not family_dir.exists():
        print(f"ERROR: Family directory not found: {family_dir}")
        print(f"  Has the family been imported yet?")
        sys.exit(1)

    ctx = FamilyContext(
        slug=args.family_slug,
        assembly=args.assembly,
        family_dir=family_dir,
        mode=args.mode,
        skip_stages=set(args.skip),
        verbose=args.verbose,
        native_config=args.native_config,
    )

    pipeline = VerificationPipeline(ctx)
    report = pipeline.run()

    auto_save_perf_data(ctx, report.stages)

    # Save unified report to results directory (Scheme C: single source of truth for dashboard)
    report_path = ctx.results_dir / "unified-verification-report.json"
    report_path.parent.mkdir(parents=True, exist_ok=True)
    report_path.write_text(report.to_json(), encoding="utf-8")

    print(report.to_json())

    if report.overall_status == "passed":
        sys.exit(0)
    else:
        sys.exit(1)


if __name__ == "__main__":
    main()
