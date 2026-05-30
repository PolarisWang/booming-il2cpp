"""CLI entry point for foundation DLL family verification.

Usage:
    python -m verification.entry_points.cli convert-char
    python -m verification.entry_points.cli convert-char --strict
    python -m verification.entry_points.cli convert-char --skip benchmark hotupdate
"""

from __future__ import annotations

import argparse
import json
import sys
from pathlib import Path

from .._path import _HERE as _VERIFICATION_ROOT

from verification.orchestration.context import FamilyContext
from verification.orchestration.engine import VerificationPipeline
from verification.analysis.perf_store import auto_save_perf_data

_TESTING_ROOT = _VERIFICATION_ROOT.parent  # testing/


def resolve_family_dir(slug: str, assembly: str) -> Path:
    return _TESTING_ROOT / assembly / slug


def _write_hotupdate_report_from_pipeline(report_path: Path, stages: dict[str, Any]) -> None:
    """Bridge: generate a D3-compatible hotupdate-verification-report.json from pipeline stage results.

    The pipeline's own hotupdate stages (hotupdate.py) produce rich data in
    unified-verification-report.json, but check-hotupdate.sh also reads the
    legacy hotupdate-verification-report.json format (originally from the
    standalone D3 runner).  This function bridges the gap so check scripts
    work without the D3 runner.

    Output fields:
        d3PatchApplied: True — the pipeline DID apply a patch (via its own
                         mechanism, not D3), so check-hotupdate.sh knows to
                         validate semantic_changed > 0.
        semanticChangedCount: from the AOT hotupdate fact stage.
        allMethodsSemanticVerified, allMethodsRevertVerified: derived from
                         the same stage.
        totalMethods: from the hotupdate fact stage.
        benchmarkResults: post-patch AOT/JIT benchmark timing from the
                          hotupdate_*_bench stages (for overhead comparison).
    """
    hu_stage = stages.get("hotupdate", {})
    hu_details = hu_stage.get("details", {}) if isinstance(hu_stage, dict) else {}

    huaot_bench = stages.get("hotupdate_aot_benchmark", {})
    huaot_details = huaot_bench.get("details", {}) if isinstance(huaot_bench, dict) else {}

    hujit_bench = stages.get("hotupdate_jit_benchmark", {})
    hujit_details = hujit_bench.get("details", {}) if isinstance(hujit_bench, dict) else {}

    # If no hotupdate stage ran at all, skip writing
    if not hu_stage or hu_stage.get("status") in (None, "skipped", "n/a"):
        # Still write a minimal report so check-hotupdate.sh doesn't error
        # — it will see d3PatchApplied=false and skip all checks.
        minimal = {
            "schemaVersion": 2,
            "d3PatchApplied": False,
            "d3PatchedCount": 0,
            "semanticChangedCount": 0,
            "totalMethods": 0,
            "passedMethods": 0,
            "failedMethods": 0,
            "verificationKind": "pipeline-bridge",
            "allMethodsSemanticVerified": False,
            "allMethodsRevertVerified": False,
        }
        report_path.write_text(json.dumps(minimal, indent=2), encoding="utf-8")
        return

    # Extract semantic data from AOT hotupdate fact stage
    semantic_changed = hu_details.get("semanticChangedCount", 0)
    total = hu_details.get("totalMethods", 0)
    passed = hu_details.get("passedMethods", 0)
    failed = hu_details.get("failedMethods", 0)
    all_revert = hu_details.get("allRevert", True)
    all_semantic = hu_details.get("allSemantic", semantic_changed > 0)

    report = {
        "schemaVersion": 2,
        "platform": sys.platform,
        "assemblyName": str(stages.get("assembly", "")),
        "familyId": "",
        "verificationKind": "pipeline-bridge",
        "d3PatchApplied": True,
        "d3PatchedCount": total,
        "semanticChangedCount": semantic_changed,
        "totalMethods": total,
        "passedMethods": passed,
        "failedMethods": failed,
        "allMethodsSemanticVerified": all_semantic,
        "allMethodsRevertVerified": all_revert,
        "summary": {
            "totalMethods": total,
            "passedMethods": passed,
            "failedMethods": failed,
            "unmatchedMethods": max(0, total - passed - failed),
        },
    }

    # Attach benchmark results if available (for overhead comparison in check-hotupdate.sh)
    if huaot_details:
        report["hotupdateAotBenchResults"] = huaot_details.get("results", [])
    if hujit_details:
        report["hotupdateJitBenchResults"] = hujit_details.get("results", [])

    report_path.write_text(json.dumps(report, indent=2), encoding="utf-8")
    print(f"[hotupdate-bridge] Wrote {report_path}")


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
    parser.add_argument("--timeout", type=int, default=0,
                        help="Per-stage timeout in seconds (0 = no timeout)")
    parser.add_argument("--resume", action="store_true",
                        help="Skip already-passed stages from previous run")

    args = parser.parse_args()
    family_dir = resolve_family_dir(args.family_slug, args.assembly)

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
        stage_timeout_seconds=args.timeout,
        resume=args.resume,
    )

    pipeline = VerificationPipeline(ctx)
    report = pipeline.run()

    auto_save_perf_data(ctx, report.stages)

    # Save unified report to results directory (Scheme C: single source of truth for dashboard)
    report_path = ctx.results_dir / "unified-verification-report.json"
    report_path.parent.mkdir(parents=True, exist_ok=True)
    report_path.write_text(report.to_json(), encoding="utf-8")

    # Bridge: generate D3-compatible hotupdate-verification-report.json from pipeline data
    _write_hotupdate_report_from_pipeline(
        ctx.results_dir / "hotupdate-verification-report.json",
        report.stages,
    )

    print(report.to_json())

    if report.overall_status == "passed":
        sys.exit(0)
    else:
        sys.exit(1)


if __name__ == "__main__":
    main()
