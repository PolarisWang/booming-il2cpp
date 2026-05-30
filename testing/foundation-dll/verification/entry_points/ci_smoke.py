"""CI smoke test — run foundation-dll verification on representative families.

Usage:
    python -m verification.entry_points.ci_smoke
    python -m verification.entry_points.ci_smoke --families convert-char,boxing-unboxing-casts
"""

from __future__ import annotations

import sys
import time
from concurrent.futures import ThreadPoolExecutor, as_completed
from pathlib import Path

from .._path import _HERE as _VERIFICATION_ROOT

from verification.orchestration.context import FamilyContext
from verification.orchestration.engine import VerificationPipeline
from verification.analysis.perf_store import auto_save_perf_data

_TESTING_ROOT = _VERIFICATION_ROOT.parent

DEFAULT_FAMILIES = [
    "convert-char",
    "boxing-unboxing-casts",
    "collections-generic-core",
    "codegen-edge-cases",
]

SKIP_STAGES = {"asm_compare", "microbench", "benchmark", "hotupdate"}


def run_smoke_family(slug: str, assembly: str = "System.Private.CoreLib") -> dict:
    family_dir = _TESTING_ROOT / assembly / slug
    if not family_dir.exists():
        return {
            "slug": slug, "status": "crashed", "error": f"Family directory not found: {family_dir}",
            "duration_seconds": 0, "coverage": {"stagesPassed": 0, "stagesTotal": 6, "stagePassRate": 0},
        }

    ctx = FamilyContext(
        slug=slug,
        assembly=assembly,
        family_dir=family_dir,
        skip_stages=SKIP_STAGES,
    )

    print(f"\n--- Smoke: {slug} ---")
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
            "coverage": report.coverage or {},
        }
        if report.coverage:
            cov = report.coverage
            print(f"  Coverage: {cov.get('stagesPassed', 0)}/{cov.get('stagesTotal', 0)} passed")
        print(f"  Duration: {duration:.1f}s, Status: {report.overall_status}")
        return result
    except Exception as e:
        import traceback
        traceback.print_exc()
        return {
            "slug": slug, "status": "crashed", "error": str(e),
            "duration_seconds": round(time.perf_counter() - start, 1),
            "coverage": {"stagesPassed": 0, "stagesTotal": 6, "stagePassRate": 0},
        }


def main(argv: list[str] | None = None) -> None:
    import argparse
    parser = argparse.ArgumentParser(description="CI smoke test for foundation-dll pipeline")
    parser.add_argument("--assembly", default="System.Private.CoreLib")
    parser.add_argument("--families", default=None, help="Comma-separated family slugs (default: 4 representative families)")
    args = parser.parse_args(argv)

    families = args.families.split(",") if args.families else DEFAULT_FAMILIES
    print(f"Foundation-DLL CI Smoke Test")
    print(f"  Assembly: {args.assembly}")
    print(f"  Families: {', '.join(families)}")
    print(f"  Stages: 6-stage quick mode (skipped: {', '.join(sorted(SKIP_STAGES))})")

    total_start = time.perf_counter()
    results = {}

    with ThreadPoolExecutor(max_workers=len(families)) as pool:
        future_map = {pool.submit(run_smoke_family, slug, args.assembly): slug for slug in families}
        for fut in as_completed(future_map):
            slug = future_map[fut]
            results[slug] = fut.result()

    passed = sum(1 for r in results.values() if r["status"] == "passed")
    failed = sum(1 for r in results.values() if r["status"] == "failed")
    crashed = sum(1 for r in results.values() if r["status"] == "crashed")

    total_time = time.perf_counter() - total_start
    print(f"\n{'='*60}")
    print(f"CI SMOKE SUMMARY")
    print(f"  Families: {len(families)}")
    print(f"  Passed:   {passed}")
    print(f"  Failed:   {failed}")
    print(f"  Crashed:  {crashed}")
    print(f"  Duration: {total_time:.0f}s")
    print(f"{'='*60}")

    if crashed > 0:
        print("FAILED: Some families crashed")
        sys.exit(2)
    if failed > 0:
        print("FAILED: Some families failed")
        sys.exit(1)
    print("ALL PASSED")
    sys.exit(0)


if __name__ == "__main__":
    main()
