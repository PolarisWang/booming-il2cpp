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
from verification.orchestration.discovery import discover_families, SKIP_SLUGS
from verification.analysis.perf_store import auto_save_perf_data
from verification.reporting.dashboard import generate_html as generate_dashboard_html, parse_family

TESTING_ROOT = _VERIFICATION_ROOT.parent


def run_single_family(args: tuple) -> dict:
    """Run one family in a sub-process worker.  Each worker gets its own
    imports and stdout — safe for ProcessPoolExecutor."""
    slug, assembly, skip_stages, native_config, mode, timeout, resume = args
    family_dir = TESTING_ROOT / assembly / slug
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
               native_config: str = "check", mode: str = "standard",
               timeout: int = 0, resume: bool = False) -> dict:
    """Run one family (sequential, in-process).  Used both directly and by
    the parallel runner (which calls run_single_family in subprocesses)."""
    # Delegate to run_single_family for a consistent code path
    return run_single_family((slug, assembly, skip_stages or set(), native_config, mode, timeout, resume))


def run_family_validations(slug: str, assembly: str = "System.Private.CoreLib",
                           testing_root: Path | None = None) -> list[str]:
    """Run post-pipeline validations on a single family's unified report.

    Checks: benchmark timing > 0, vs .NET 8 slowdown ≤ 20%, hotupdate OK.

    Returns a list of failure messages (empty = all passed).
    """
    if testing_root is None:
        from verification._path import _HERE as _VERIFICATION_ROOT
        testing_root = _VERIFICATION_ROOT.parent.parent  # testing/

    report_path = (
        testing_root / "results" / "foundation-dll" / assembly / slug
        / "unified-verification-report.json"
    )
    if not report_path.exists():
        return [f"Report not found: {report_path}"]

    try:
        report = json.loads(report_path.read_text(encoding="utf-8"))
    except (OSError, json.JSONDecodeError) as e:
        return [f"Failed to read report: {e}"]

    failures: list[str] = []
    stages = report.get("stages", {})

    # 1. Perf timings: all benchmark elapsedMilliseconds > 0
    for stage_name, stage in stages.items():
        if "benchmark" not in stage_name:
            continue
        details = stage.get("details", {})
        for runtime in ["native-aot", "native-jit"]:
            results = details.get(runtime, {}).get("results", [])
            for r in results:
                t = r.get("elapsedMilliseconds", -1)
                m = r.get("methodSubjectId", "unknown")
                if t <= 0:
                    failures.append(f"[{stage_name}/{runtime}] {m}: elapsedMilliseconds={t}")

    # 2. .NET 8 slowdown ≤ 20%
    bench_stage = None
    for name, stage in stages.items():
        if "benchmark" in name and "hotupdate" not in name:
            bench_stage = stage
            break
    if bench_stage:
        details = bench_stage.get("details", {})
        for runtime_label, runtime_key in [("AOT", "native-aot"), ("JIT", "native-jit")]:
            rt_data = details.get(runtime_key, {})
            if not isinstance(rt_data, dict):
                continue
            results = rt_data.get("results", [])
            net8 = rt_data.get("net8Results", [])
            for i, r in enumerate(results):
                chaos_ms = r.get("elapsedMilliseconds", 0)
                net8_ms = net8[i].get("elapsedMilliseconds", 0) if i < len(net8) else 0
                method = r.get("methodSubjectId", f"method-{i}")
                if chaos_ms > 0 and net8_ms > 0:
                    slowdown = (chaos_ms / net8_ms - 1.0) * 100
                    if slowdown > 20:
                        failures.append(f"[.NET8] {runtime_label} {method}: {slowdown:.1f}% > 20%")
                elif chaos_ms <= 0:
                    failures.append(f"[.NET8] {runtime_label} {method}: chaos timing is zero")

    # 3. HotUpdate: semantic_changed > 0, overhead ≤ 100%
    hu_stage = stages.get("hotupdate")
    if hu_stage:
        hu_details = hu_stage.get("details", {})
        semantic_changed = hu_details.get("semanticChangedCount", 0)
        total = hu_details.get("totalMethods", 0)
        if semantic_changed <= 0:
            failures.append(f"[HotUpdate] semantic_changed_count={semantic_changed}")

        for bench_key in ["hotupdate_aot_benchmark", "hotupdate_jit_benchmark"]:
            bench_stage = stages.get(bench_key)
            if not bench_stage:
                continue
            details = bench_stage.get("details", {})
            results = details.get("results", [])
            for r in results:
                base_ns = r.get("baseElapsedNs", 0) or r.get("elapsedBeforePatchNs", 0)
                patch_ns = r.get("patchedElapsedNs", 0) or r.get("elapsedAfterPatchNs", 0)
                method = r.get("methodSubjectId", "unknown")
                if base_ns > 0 and patch_ns > 0:
                    overhead = (patch_ns / base_ns - 1.0) * 100
                    if overhead > 100:
                        failures.append(f"[HotUpdate] {method}: overhead {overhead:.1f}% > 100%")

    return failures


def main(argv: list[str] | None = None) -> None:
    import argparse

    parser = argparse.ArgumentParser(description="Batch-run pipeline on all families")
    parser.add_argument("--output", "-o", default=None,
                        help="Output report path (default: testing/results/batch-report.json)")
    parser.add_argument("--assembly", default="System.Private.CoreLib")
    parser.add_argument("--family", default=None,
                        help="Single family slug to run (run all families if omitted)")
    parser.add_argument("--skip-stages", default=None,
                        help="Comma-separated stages to skip: preflight,codegen,jit_codegen,fact,fact_jit,audit,asm_compare,microbench,benchmark,hotupdate")
    parser.add_argument("--from-slug", default=None,
                        help="Skip families before this slug (resume from here)")
    parser.add_argument("--concurrency", "-j", type=int, default=1,
                        help="Number of families to run in parallel (default: 1, sequential)")
    parser.add_argument("--native-config", choices=["check", "profile", "ship"], default="check",
                        help="Native build config (default: check)")
    parser.add_argument("--mode", choices=["standard", "strict"], default="standard",
                        help="Verification mode (default: standard)")
    parser.add_argument("--stage-resume", action="store_true",
                        help="Skip already-passed stages within each family from previous run")
    parser.add_argument("--timeout", type=int, default=0,
                        help="Per-stage timeout in seconds (0 = no timeout)")
    parser.add_argument("--validate", action="store_true",
                        help="Run post-pipeline validation checks (perf, slowdown, hotupdate)")
    parser.add_argument("--verbose", "-v", action="store_true",
                        help="Verbose discovery logging")
    args = parser.parse_args(argv)

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

    from_slug = args.from_slug
    if from_slug:
        if from_slug in slugs:
            idx = slugs.index(from_slug)
            slugs = slugs[idx:]
            print(f"Resuming from {from_slug} ({len(slugs)} remaining)")
        else:
            print(f"WARNING: resume slug '{from_slug}' not found, starting from beginning")

    if args.output:
        output_path = Path(args.output)
    else:
        output_path = TESTING_ROOT / "results" / "batch-report.json"
    output_path.parent.mkdir(parents=True, exist_ok=True)

    total_start = time.perf_counter()
    results: list[dict] = []
    passed = 0
    failed = 0
    crashed = 0
    skipped_count = 0

    def save_report_snapshot() -> None:
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

    def accumulate_result(r: dict) -> None:
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
            result = run_single_family(wargs)
            results.append(result)
            accumulate_result(result)
            save_report_snapshot()
    else:
        # ── Parallel ──
        results = [None] * len(worker_args)
        next_to_report = 0

        with ProcessPoolExecutor(max_workers=args.concurrency) as executor:
            fut_map = {executor.submit(run_single_family, wargs): i
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
                    accumulate_result(r)
                    next_to_report += 1
                save_report_snapshot()

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

    # ── Post-run validation ──────────────────────────────────────────
    if args.validate and results:
        print(f"\n{'─' * 50}")
        print(f"Post-Run Validations")
        print(f"{'─' * 50}")
        validation_failed = 0
        for r in results:
            if r["status"] != "passed":
                continue
            slug = r["slug"]
            vf = run_family_validations(slug, args.assembly)
            if vf:
                validation_failed += 1
                for msg in vf:
                    print(f"  FAIL [{slug}] {msg}")
            else:
                print(f"  PASS [{slug}] all checks")
        if validation_failed:
            print(f"\n  {validation_failed} family/families failed validation")
        else:
            print(f"\n  All validations passed")
        print(f"{'─' * 50}")


if __name__ == "__main__":
    main()
