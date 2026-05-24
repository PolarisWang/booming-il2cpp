"""CI smoke test — run foundation-dll verification on representative families.

Picks 3-5 families that are known to pass reliably and runs them through
6-stage quick mode (preflight → codegen → jit_codegen → fact → fact_jit → audit).
Skips asm_compare, microbench, benchmark, and hotupdate to keep CI runtime under 5min.

Usage:
    python _core/python/ci_smoke.py
    python _core/python/ci_smoke.py --families convert-char,boxing-unboxing-casts
"""

from __future__ import annotations

import sys
import time
from pathlib import Path

_HERE = Path(__file__).resolve().parent
_CORE_PACKAGE = _HERE.parents[1]  # testing/foundation-dll/
if str(_CORE_PACKAGE) not in sys.path:
    sys.path.insert(0, str(_CORE_PACKAGE))

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

# Representative families for CI smoke testing.
# Picked for: diverse coverage, 100% pass rate in Batch 1, fast execution.
DEFAULT_FAMILIES = [
    "convert-char",            # type conversion — core IL semantics
    "boxing-unboxing-casts",   # boxing/unboxing — object model
    "collections-generic-core", # generics — collection types
    "codegen-edge-cases",      # edge cases — codegen stress
]

# Quick mode: skip perf-heavy and known-flaky stages
SKIP_STAGES = {"asm_compare", "microbench", "benchmark", "hotupdate"}


def run_smoke_family(slug: str, assembly: str = "System.Private.CoreLib") -> dict:
    """Run a single family through 6-stage quick mode."""
    family_dir = _TESTING_ROOT / "foundation-dll" / assembly / slug
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


def main() -> None:
    import argparse
    parser = argparse.ArgumentParser(description="CI smoke test for foundation-dll pipeline")
    parser.add_argument("--assembly", default="System.Private.CoreLib")
    parser.add_argument("--families", default=None, help="Comma-separated family slugs (default: 4 representative families)")
    args = parser.parse_args()

    families = args.families.split(",") if args.families else DEFAULT_FAMILIES
    print(f"Foundation-DLL CI Smoke Test")
    print(f"  Assembly: {args.assembly}")
    print(f"  Families: {', '.join(families)}")
    print(f"  Stages: 6-stage quick mode (skipped: {', '.join(sorted(SKIP_STAGES))})")

    total_start = time.perf_counter()
    passed = 0
    failed = 0
    crashed = 0

    for slug in families:
        result = run_smoke_family(slug, args.assembly)
        if result["status"] == "passed":
            passed += 1
        elif result["status"] == "crashed":
            crashed += 1
        else:
            failed += 1

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
