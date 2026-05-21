"""Batch runner — run full pipeline on all families and collect results.

Usage:
    python batch_run_all.py [--output REPORT_PATH]
"""

from __future__ import annotations

import json
import sys
import time
from pathlib import Path

# ── Path setup (same as cli.py) ──────────────────────────────────────
_HERE = Path(__file__).resolve().parent
_CORE_PACKAGE = _HERE.parents[1]  # testing/foundation-dll/
if str(_CORE_PACKAGE) not in sys.path:
    sys.path.insert(0, str(_CORE_PACKAGE))

# Ensure testing/ is importable for _pipeline shared components
_TESTING_ROOT = _CORE_PACKAGE.parent  # testing/
if str(_TESTING_ROOT) not in sys.path:
    sys.path.insert(0, str(_TESTING_ROOT))

_TESTING_SRC_PYTHON = _CORE_PACKAGE.parent / "src" / "python"
if str(_TESTING_SRC_PYTHON) not in sys.path:
    sys.path.insert(0, str(_TESTING_SRC_PYTHON))
_OLD_PIPELINE = _HERE.parents[4] / "build" / "toolchains" / "run" / "testing" / "foundation_dll"
if str(_OLD_PIPELINE) not in sys.path:
    sys.path.insert(0, str(_OLD_PIPELINE))

from _core.python.models import FamilyContext
from _core.python.orchestrator import VerificationPipeline
from _core.python.perf_store import auto_save_perf_data
from _core.python.deep_dashboard import generate_html as generate_dashboard_html

_TESTING_ROOT = _CORE_PACKAGE.parent

SKIP_SLUGS = {
    "aggregation", "filtering", "immutable-array", "immutable-list", "reports",
}

# Families that need special handling (comma in path)
COMBINED_FAMILIES = {
    "interface-dispatch,pinvoke-dllimport,primitive-numeric-conversions-core",
}


def discover_families(assembly: str = "System.Private.CoreLib") -> list[str]:
    """Discover all family slugs in the testing framework."""
    families_dir = _TESTING_ROOT / "foundation-dll" / assembly
    slugs = sorted([
        d.name for d in families_dir.iterdir()
        if d.is_dir() and (d / "contract.json").exists()
    ])
    return [s for s in slugs if s not in SKIP_SLUGS]


def run_family(slug: str, assembly: str = "System.Private.CoreLib") -> dict:
    """Run the full pipeline for one family. Returns result dict."""
    family_dir = _TESTING_ROOT / "foundation-dll" / assembly / slug
    ctx = FamilyContext(
        slug=slug,
        assembly=assembly,
        family_dir=family_dir,
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
                name: {"status": s.get("status", "?"), "summary": s.get("summary", "")}
                for name, s in report.stages.items()
            },
            "coverage": report.coverage,
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
            "coverage": {"stagesPassed": 0, "stagesTotal": 13, "stagePassRate": 0},
        }


def main() -> None:
    import argparse

    parser = argparse.ArgumentParser(description="Batch-run pipeline on all families")
    parser.add_argument("--output", "-o", default=None,
                        help="Output report path (default: testing/results/batch-report.json)")
    parser.add_argument("--assembly", default="System.Private.CoreLib")
    parser.add_argument("--resume", default=None,
                        help="Resume from a specific slug (skip families before this)")
    args = parser.parse_args()

    slugs = discover_families(args.assembly)
    print(f"Discovered {len(slugs)} families to run")

    # Resume support
    resume_from = args.resume
    if resume_from:
        if resume_from in slugs:
            idx = slugs.index(resume_from)
            slugs = slugs[idx:]
            print(f"Resuming from {resume_from} ({len(slugs)} remaining)")
        else:
            print(f"WARNING: resume slug '{resume_from}' not found, starting from beginning")

    # Default output path
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

    for i, slug in enumerate(slugs):
        print(f"\n[{i+1}/{len(slugs)}] ", end="")
        result = run_family(slug, args.assembly)
        results.append(result)

        if result["status"] == "passed":
            passed += 1
        elif result["status"] == "crashed":
            crashed += 1
        else:
            failed += 1

        # Save incremental results after each family
        report = {
            "timestamp": time.strftime("%Y-%m-%dT%H:%M:%S"),
            "assembly": args.assembly,
            "total_families": len(slugs),
            "passed": passed,
            "failed": failed,
            "crashed": crashed,
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
    print(f"  Crashed: {crashed}")
    print(f"  Report: {output_path}")
    print(f"{'='*60}")

    # Final write
    report["elapsed_seconds"] = round(total_time, 1)
    output_path.write_text(json.dumps(report, indent=2, ensure_ascii=False), encoding="utf-8")

    # Generate deep dashboard HTML
    try:
        dashboard_html = generate_dashboard_html(report)
        dashboard_path = output_path.with_name("deep-dashboard.html")
        dashboard_path.write_text(dashboard_html, encoding="utf-8")
        print(f"  Dashboard: {dashboard_path} ({len(dashboard_html) // 1024} KB)")
    except Exception as e:
        print(f"  WARNING: Dashboard generation failed: {e}")


if __name__ == "__main__":
    main()
