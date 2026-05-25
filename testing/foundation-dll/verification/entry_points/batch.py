"""Batch runner — run full pipeline on all families and collect results.

Usage:
    python -m verification.entry_points.batch [--output REPORT_PATH]
"""

from __future__ import annotations

import json
import sys
import time
from pathlib import Path

from .._path import _HERE as _VERIFICATION_ROOT

from verification.orchestration.context import FamilyContext
from verification.orchestration.engine import VerificationPipeline
from verification.analysis.perf_store import auto_save_perf_data
from verification.reporting.dashboard import generate_html as generate_dashboard_html, parse_family

_TESTING_ROOT = _VERIFICATION_ROOT.parent

SKIP_SLUGS = {
    "aggregation", "filtering", "immutable-array", "immutable-list", "reports",
    "interface-dispatch,pinvoke-dllimport,primitive-numeric-conversions-core",
}


def discover_families(assembly: str = "System.Private.CoreLib") -> list[str]:
    families_dir = _TESTING_ROOT / "foundation-dll" / assembly
    slugs = sorted([
        d.name for d in families_dir.iterdir()
        if d.is_dir() and (d / "contract.json").exists()
    ])
    return [s for s in slugs if s not in SKIP_SLUGS]


def run_family(slug: str, assembly: str = "System.Private.CoreLib", skip_stages: set[str] | None = None) -> dict:
    family_dir = _TESTING_ROOT / "foundation-dll" / assembly / slug
    ctx = FamilyContext(
        slug=slug,
        assembly=assembly,
        family_dir=family_dir,
        skip_stages=skip_stages or set(),
    )

    print(f"\n{'='*60}")
    print(f"[BATCH] Running: {slug}")
    print(f"{'='*60}")

    pipeline = VerificationPipeline(ctx)
    start = time.perf_counter()
    try:
        report = pipeline.run()
        auto_save_perf_data(ctx, report.stages)
        duration = time.perf_counter() - start
        result = {
            "slug": slug,
            "status": report.overall_status,
            "duration_seconds": round(duration, 1),
            "stages": {
                name: {
                    "status": s.get("status", "?"),
                    "summary": s.get("summary", ""),
                    "details": s.get("details"),
                    "errors": s.get("errors", []),
                    "duration_ms": s.get("duration_ms", 0),
                }
                for name, s in report.stages.items()
            },
            "coverage": report.coverage,
            "dashboard": report.dashboard,
        }
        if report.coverage:
            print(f"  Coverage: {report.coverage.get('stagesPassed', 0)}/"
                  f"{report.coverage.get('stagesTotal', 0)} passed")
        print(f"  Duration: {duration:.1f}s, Status: {report.overall_status}")
        return result
    except Exception as e:
        import traceback
        duration = time.perf_counter() - start
        traceback.print_exc()
        return {
            "slug": slug,
            "status": "crashed",
            "duration_seconds": round(duration, 1),
            "error": str(e),
            "stages": {},
            "coverage": {"stagesPassed": 0, "stagesTotal": 15, "stagePassRate": 0},
        }


def main() -> None:
    import argparse

    parser = argparse.ArgumentParser(description="Batch-run pipeline on all families")
    parser.add_argument("--output", "-o", default=None,
                        help="Output report path (default: testing/results/batch-report.json)")
    parser.add_argument("--assembly", default="System.Private.CoreLib")
    parser.add_argument("--family", default=None,
                        help="Single family slug to run (run all families if omitted)")
    parser.add_argument("--skip-stages", default=None,
                        help="Comma-separated stages to skip: preflight,codegen,jit_codegen,fact,fact_jit,audit,asm_compare,microbench,benchmark,hotupdate")
    parser.add_argument("--resume", default=None,
                        help="Resume from a specific slug (skip families before this)")
    args = parser.parse_args()

    if args.family:
        slugs = [args.family]
    else:
        slugs = discover_families(args.assembly)

    skip_stages = set()
    if args.skip_stages:
        skip_stages = set(s.strip() for s in args.skip_stages.split(","))

    print(f"Discovered {len(slugs)} families to run")

    resume_from = args.resume
    if resume_from:
        if resume_from in slugs:
            idx = slugs.index(resume_from)
            slugs = slugs[idx:]
            print(f"Resuming from {resume_from} ({len(slugs)} remaining)")
        else:
            print(f"WARNING: resume slug '{resume_from}' not found, starting from beginning")

    if args.output:
        output_path = Path(args.output)
    else:
        output_path = _TESTING_ROOT / "results" / "batch-report.json"
    output_path.parent.mkdir(parents=True, exist_ok=True)

    results: list[dict] = []
    total_start = time.perf_counter()
    passed = 0
    failed = 0
    crashed = 0
    skipped_count = 0

    for i, slug in enumerate(slugs):
        print(f"\n[{i+1}/{len(slugs)}] ", end="")
        result = run_family(slug, args.assembly, skip_stages)
        results.append(result)

        if result["status"] == "passed":
            passed += 1
        elif result["status"] == "skipped":
            skipped_count += 1
        elif result["status"] == "crashed":
            crashed += 1
        else:
            failed += 1

        report = {
            "timestamp": time.strftime("%Y-%m-%dT%H:%M:%S"),
            "assembly": args.assembly,
            "total_families": len(slugs),
            "passed": passed,
            "failed": failed,
            "crashed": crashed,
            "skipped": skipped_count,
            "elapsed_seconds": round(time.perf_counter() - total_start, 1),
            "results": results,
        }
        output_path.write_text(json.dumps(report, indent=2, ensure_ascii=False), encoding="utf-8")

    total_time = time.perf_counter() - total_start
    print(f"\n{'='*60}")
    print(f"BATCH COMPLETE")
    print(f"  Total: {len(slugs)} families in {total_time:.0f}s")
    print(f"  Passed: {passed}")
    print(f"  Failed: {failed}")
    print(f"  Skipped: {skipped_count}")
    print(f"  Crashed: {crashed}")
    print(f"  Report: {output_path}")
    print(f"{'='*60}")

    report["elapsed_seconds"] = round(total_time, 1)
    report["passed"] = passed
    report["failed"] = failed
    report["skipped"] = skipped_count
    report["crashed"] = crashed
    output_path.write_text(json.dumps(report, indent=2, ensure_ascii=False), encoding="utf-8")

    try:
        report["parsed"] = [parse_family(r) for r in report.get("results", [])]
        dashboard_html = generate_dashboard_html(report)
        dashboard_path = output_path.with_name("deep-dashboard.html")
        dashboard_path.write_text(dashboard_html, encoding="utf-8")
        print(f"  Dashboard: {dashboard_path} ({len(dashboard_html) // 1024} KB)")
    except Exception as e:
        print(f"  WARNING: Dashboard generation failed: {e}")


if __name__ == "__main__":
    main()
