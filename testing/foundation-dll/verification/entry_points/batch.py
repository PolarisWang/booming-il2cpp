"""Batch runner — run full pipeline on all families and collect results.

Usage:
    python -m verification.entry_points.batch [--output REPORT_PATH] [--concurrency N]

Runs families in parallel when --concurrency > 1.
"""

from __future__ import annotations

import json
import sys
import time
from concurrent.futures import ProcessPoolExecutor, as_completed
from pathlib import Path

from .._path import _HERE as _VERIFICATION_ROOT

from verification.orchestration.context import FamilyContext
from verification.orchestration.engine import VerificationPipeline
from verification.analysis.perf_store import auto_save_perf_data
from verification.reporting.dashboard import generate_html as generate_dashboard_html, parse_family

_TESTING_ROOT = _VERIFICATION_ROOT.parent

SKIP_SLUGS = {
    "aggregation", "filtering", "immutable-array", "immutable-list", "reports",
    "interface-dispatch", "pinvoke-dllimport", "primitive-numeric-conversions-core",
}


def discover_families(assembly: str = "System.Private.CoreLib", verbose: bool = False) -> list[str]:
    families_dir = _TESTING_ROOT / assembly
    if not families_dir.is_dir():
        print(f"WARNING: Assembly directory not found: {families_dir}")
        return []

    all_dirs = sorted(d for d in families_dir.iterdir() if d.is_dir())
    slugs: list[str] = []

    for d in all_dirs:
        has_cap = (d / "capability-family-contract.json").exists()
        has_legacy = (d / "contract.json").exists()
        slug = d.name

        if not has_cap and not has_legacy:
            if verbose:
                print(f"  [discover] skip {slug}/ — no contract file found")
            continue

        if slug in SKIP_SLUGS:
            if verbose:
                print(f"  [discover] skip {slug}/ — in SKIP_SLUGS")
            continue

        if has_legacy and not has_cap:
            if verbose:
                print(f"  [discover] {slug}/ — using legacy contract.json (not yet migrated)")
        elif verbose:
            print(f"  [discover] {slug}/ — capability-family-contract.json")
        slugs.append(slug)

    return slugs


def _run_single_family(args: tuple) -> dict:
    """Run one family in a sub-process worker.  Each worker gets its own
    imports and stdout — safe for ProcessPoolExecutor."""
    slug, assembly, skip_stages, native_config, mode, timeout, resume = args
    family_dir = _TESTING_ROOT / assembly / slug
    ctx = FamilyContext(
        slug=slug,
        assembly=assembly,
        family_dir=family_dir,
        skip_stages=skip_stages or set(),
        native_config=native_config,
        mode=mode,
        stage_timeout_seconds=timeout,
        resume=resume,
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
            "coverage": {"stagesPassed": 0, "stagesTotal": 16, "stagePassRate": 0},
        }


def run_family(slug: str, assembly: str = "System.Private.CoreLib", skip_stages: set[str] | None = None,
               native_config: str = "check", mode: str = "standard") -> dict:
    """Run one family (sequential, in-process).  Used both directly and by
    the parallel runner (which calls _run_single_family in subprocesses)."""
    # Delegate to _run_single_family for a consistent code path
    return _run_single_family((slug, assembly, skip_stages or set(), native_config, mode))


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
    parser.add_argument("--concurrency", "-j", type=int, default=1,
                        help="Number of families to run in parallel (default: 1, sequential)")
    parser.add_argument("--native-config", choices=["check", "profile", "ship"], default="check",
                        help="Native build config (default: check)")
    parser.add_argument("--resume", default=None,
                        help="Resume from a specific slug (skip families before this)")
    parser.add_argument("--stage-resume", action="store_true",
                        help="Skip already-passed stages within each family from previous run")
    parser.add_argument("--timeout", type=int, default=0,
                        help="Per-stage timeout in seconds (0 = no timeout)")
    parser.add_argument("--verbose", "-v", action="store_true",
                        help="Verbose discovery logging")
    args = parser.parse_args()

    if args.family:
        slugs = [args.family]
    else:
        slugs = discover_families(args.assembly, verbose=args.verbose)

    skip_stages = set()
    if args.skip_stages:
        skip_stages = set(s.strip() for s in args.skip_stages.split(","))

    print(f"Discovered {len(slugs)} families to run"
          f"{'' if args.concurrency <= 1 else f' (concurrency={args.concurrency})'}"
          f"{' [STAGE-RESUME]' if args.stage_resume else ''}"
          f"{' [TIMEOUT=' + str(args.timeout) + 's]' if args.timeout > 0 else ''}")
    if args.verbose:
        for s in slugs:
            print(f"  - {s}")

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

    total_start = time.perf_counter()
    results: list[dict] = []
    passed = 0
    failed = 0
    crashed = 0
    skipped_count = 0

    def _save_snapshot() -> None:
        report_snapshot = {
            "timestamp": time.strftime("%Y-%m-%dT%H:%M:%S"),
            "assembly": args.assembly,
            "total_families": len(slugs),
            "passed": passed,
            "failed": failed,
            "crashed": crashed,
            "skipped": skipped_count,
            "elapsed_seconds": round(time.perf_counter() - total_start, 1),
            "results": [r for r in results if r is not None],
        }
        output_path.write_text(json.dumps(report_snapshot, indent=2, ensure_ascii=False), encoding="utf-8")

    def _accumulate(r: dict) -> None:
        nonlocal passed, failed, crashed, skipped_count
        if r["status"] == "passed":
            passed += 1
        elif r["status"] == "skipped":
            skipped_count += 1
        elif r["status"] == "crashed":
            crashed += 1
        else:
            failed += 1

    worker_args = [
        (slug, args.assembly, skip_stages, args.native_config, args.mode, args.timeout, args.stage_resume)
        for slug in slugs
    ]

    if args.concurrency <= 1:
        # ── Sequential (original mode) ──
        for i, wargs in enumerate(worker_args):
            print(f"\n[{i+1}/{len(slugs)}] ", end="")
            result = _run_single_family(wargs)
            results.append(result)
            _accumulate(result)
            _save_snapshot()
    else:
        # ── Parallel ──
        results = [None] * len(worker_args)
        next_to_report = 0

        with ProcessPoolExecutor(max_workers=args.concurrency) as executor:
            fut_map = {executor.submit(_run_single_family, wargs): i
                       for i, wargs in enumerate(worker_args)}
            for fut in as_completed(fut_map):
                idx = fut_map[fut]
                try:
                    result = fut.result()
                except Exception as e:
                    slug = slugs[idx]
                    result = {
                        "slug": slug, "status": "crashed", "duration_seconds": 0,
                        "error": str(e), "stages": {},
                        "coverage": {"stagesPassed": 0, "stagesTotal": 16, "stagePassRate": 0},
                    }
                results[idx] = result

                # Report progress in order
                while next_to_report < len(results) and results[next_to_report] is not None:
                    r = results[next_to_report]
                    _accumulate(r)
                    next_to_report += 1
                _save_snapshot()

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

    final_report = {
        "timestamp": time.strftime("%Y-%m-%dT%H:%M:%S"),
        "assembly": args.assembly,
        "total_families": len(slugs),
        "passed": passed,
        "failed": failed,
        "skipped": skipped_count,
        "crashed": crashed,
        "elapsed_seconds": round(total_time, 1),
        "results": [r for r in results if r is not None],
    }
    output_path.write_text(json.dumps(final_report, indent=2, ensure_ascii=False), encoding="utf-8")

    try:
        final_report["parsed"] = [parse_family(r) for r in final_report.get("results", [])]
        dashboard_html = generate_dashboard_html(final_report)
        dashboard_path = output_path.with_name("deep-dashboard.html")
        dashboard_path.write_text(dashboard_html, encoding="utf-8")
        print(f"  Dashboard: {dashboard_path} ({len(dashboard_html) // 1024} KB)")
    except Exception as e:
        print(f"  WARNING: Dashboard generation failed: {e}")


if __name__ == "__main__":
    main()
