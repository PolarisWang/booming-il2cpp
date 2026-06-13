"""Aggregate stage — collects all chunk results into _dll/reports/.

Reads each chunk's results/ directory and produces:
  - _dll/reports/latest/fact-summary.json
  - _dll/reports/latest/benchmark-summary.json
  - _dll/reports/latest/coverage-audit.json
  - _dll/reports/latest/dashboard.json
  - _dll/reports/history/benchmark-YYYY-MM-DD.json
"""

from __future__ import annotations

import json
import time
from datetime import date
from pathlib import Path

from verification.orchestration.context import ChunkContext, StageResult


def run_aggregate(ctx: ChunkContext, stages: dict[str, StageResult]) -> StageResult:
    """Aggregate stage: collect all chunk results into _dll/reports/."""
    start = time.perf_counter()

    foundation_dir = ctx.foundation_dir
    assembly = ctx.assembly
    chunks_dir = foundation_dir / "chunks"
    reports_dir = foundation_dir / "_dll" / "reports"
    latest_dir = reports_dir / "latest"
    history_dir = reports_dir / "history"

    if not chunks_dir.exists():
        return StageResult(
            stage="aggregate", status="error",
            summary=f"chunks directory not found: {chunks_dir}",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    # ── Discover chunks ──
    chunk_slugs = sorted(d.name for d in chunks_dir.iterdir() if d.is_dir())
    print(f"  [aggregate] {len(chunk_slugs)} chunks found")

    # ── Collect per-chunk results ──
    all_fact: list[dict] = []
    all_benchmark: list[dict] = []
    chunk_summaries: list[dict] = []

    for slug in chunk_slugs:
        chunk_dir = chunks_dir / slug
        results_dir = chunk_dir / "results"

        summary: dict = {"slug": slug}

        # Fact results
        fact_path = results_dir / "fact.json"
        if fact_path.exists():
            fact_data = json.loads(fact_path.read_text(encoding="utf-8"))
            summary["fact"] = {
                "passed": fact_data.get("passed", 0),
                "total": fact_data.get("total", 0),
                "valueSuspicious": fact_data.get("valueSuspicious", False),
                "valueWarnings": fact_data.get("valueWarnings", 0),
                "metaTotal": fact_data.get("metaTotal"),
                "metaBenchmarkCount": fact_data.get("metaBenchmarkCount", 0),
            }
            all_fact.append({
                "chunk": slug,
                **fact_data,
            })
        else:
            summary["fact"] = {"status": "no_results"}

        # Benchmark results — collect both counts and performance metrics
        bench_path = results_dir / "benchmark.json"
        if bench_path.exists():
            bench_data = json.loads(bench_path.read_text(encoding="utf-8"))
            results_list = bench_data.get("results", [])
            method_count = bench_data.get("methodCount", len(results_list))
            chunk_benchmark = {
                "methodCount": method_count,
                "iterations": bench_data.get("iterations", 0),
            }
            perf_summary = bench_data.get("summary", {})
            if perf_summary:
                for perf_key in ("meanDurationMs", "meanOpsPerSecond", "minDurationMs",
                                 "maxDurationMs", "totalDurationMs", "totalAllocatedBytes",
                                 "meanSampleCount", "totalOutliers",
                                 "warmupRounds", "sampleRounds"):
                    if perf_key in perf_summary:
                        chunk_benchmark[perf_key] = perf_summary[perf_key]
            # Collect per-method stddev/cv for aggregate CV computation
            per_method_stats = bench_data.get("perMethodStats") or []
            if per_method_stats:
                cvs = [s.get("cv", 0) for s in per_method_stats if isinstance(s.get("cv"), (int, float))]
                if cvs:
                    chunk_benchmark["meanCv"] = sum(cvs) / len(cvs)
                    chunk_benchmark["maxCv"] = max(cvs)
            summary["benchmark"] = chunk_benchmark
            all_benchmark.append({
                "chunk": slug,
                **bench_data,
            })
        else:
            summary["benchmark"] = {"status": "no_results"}

        # HotUpdate results
        hu_path = results_dir / "hotupdate.json"
        if hu_path.exists():
            hu_data = json.loads(hu_path.read_text(encoding="utf-8"))
            summary["hotupdate"] = {
                "passed": hu_data.get("passed", 0),
                "failed": hu_data.get("failed", 0),
                "allSemantic": hu_data.get("allSemantic", False),
                "allRevert": hu_data.get("allRevert", False),
                "exitCode": hu_data.get("exitCode", 0),
                "patchDataUsed": hu_data.get("patchDataUsed", False),
                "patchFailed": hu_data.get("patchFailed", False),
                "truncated": hu_data.get("truncated", False),
                "crash": hu_data.get("crash", False),
                "assertFailed": hu_data.get("assertFailed", 0),
                "semanticChangedCount": hu_data.get("semanticChangedCount", 0),
            }
        else:
            summary["hotupdate"] = {"status": "no_results"}

        chunk_summaries.append(summary)

    # ── Build aggregate metrics ──
    total_passed = sum(s.get("fact", {}).get("passed", 0) for s in chunk_summaries)
    total_fact = sum(s.get("fact", {}).get("total", 0) for s in chunk_summaries)
    # Only count chunks that actually ran subjects (total > 0)
    chunks_with_fact = sum(
        1 for s in chunk_summaries
        if s.get("fact", {}).get("total", 0) > 0
    )
    # Track chunks with value warnings
    chunks_with_value_warnings = sum(
        1 for s in chunk_summaries
        if s.get("fact", {}).get("valueSuspicious", False)
    )
    # Track chunks with severe metadata mismatch (metaTotal >> total).
    # Small gaps are expected: void methods (A3 "no crash" assertions) contribute
    # to metaTotal but don't produce individual fact results.  Flag when
    # the gap exceeds 50% (fewer than half of declared methods produce results).
    # Also emit a per-chunk advisory WARN for any gap >20%.
    chunks_with_meta_mismatch = 0
    for s in chunk_summaries:
        fact = s.get("fact", {})
        meta = fact.get("factMethodCount") if fact.get("factMethodCount") is not None else fact.get("metaTotal")
        total = s.get("fact", {}).get("total")
        if meta is not None and meta > 0 and total is not None and total != meta:
            ratio = total / meta
            if ratio < 0.5:
                chunks_with_meta_mismatch += 1
            if ratio < 0.8:
                chunk_slug = s.get("info", {}).get("slug", "?")
                meta_label = "factMethodCount" if s.get("fact", {}).get("factMethodCount") else "metaTotal"
                print(f"  [aggregate] WARN: {chunk_slug} fact total={total} < {meta_label}={meta} (ratio={ratio:.0%})")

    # ── Compute aggregate benchmark performance ──
    chunks_with_benchmark = [s.get("benchmark", {}) for s in chunk_summaries if "methodCount" in s.get("benchmark", {})]
    total_benchmarked = sum(b.get("methodCount", 0) for b in chunks_with_benchmark)
    aggregate_perf: dict[str, float] = {}
    if chunks_with_benchmark:
        duration_values = [b["meanDurationMs"] for b in chunks_with_benchmark if "meanDurationMs" in b]
        ops_values = [b["meanOpsPerSecond"] for b in chunks_with_benchmark if "meanOpsPerSecond" in b]
        if duration_values:
            aggregate_perf["meanDurationMs"] = sum(duration_values) / len(duration_values)
        if ops_values:
            aggregate_perf["meanOpsPerSecond"] = sum(ops_values) / len(ops_values)
        # Aggregate statistical QC (M1)
        cv_values = [b["meanCv"] for b in chunks_with_benchmark if "meanCv" in b]
        if cv_values:
            aggregate_perf["aggregateMeanCv"] = sum(cv_values) / len(cv_values)
        total_outliers = sum(b.get("totalOutliers", 0) for b in chunks_with_benchmark)
        if total_outliers:
            aggregate_perf["totalOutliers"] = total_outliers

    # ── Compute aggregate hotupdate metrics ──
    chunks_with_patch_data = sum(
        1 for s in chunk_summaries
        if s.get("hotupdate", {}).get("patchDataUsed", False)
    )
    chunks_with_patch_failed = sum(
        1 for s in chunk_summaries
        if s.get("hotupdate", {}).get("patchFailed", False)
    )
    chunks_with_revert_failure = sum(
        1 for s in chunk_summaries
        if not s.get("hotupdate", {}).get("allRevert", True)
    )
    total_hu_passed = sum(
        s.get("hotupdate", {}).get("passed", 0) for s in chunk_summaries
    )
    total_hu_failed = sum(
        s.get("hotupdate", {}).get("failed", 0) for s in chunk_summaries
    )
    # Count hotupdate skip-status breakdowns for observability
    hotupdate_skip_statuses: dict[str, int] = {}
    for s in chunk_summaries:
        hu_status = s.get("hotupdate", {}).get("status")
        if hu_status and hu_status.startswith("skipped_"):
            hotupdate_skip_statuses[hu_status] = hotupdate_skip_statuses.get(hu_status, 0) + 1

    # ── Write reports ──
    latest_dir.mkdir(parents=True, exist_ok=True)
    history_dir.mkdir(parents=True, exist_ok=True)

    # fact-summary.json
    fact_summary = {
        "assemblyName": ctx.assembly,
        "timestamp": time.strftime("%Y-%m-%dT%H:%M:%S"),
        "totalChunks": len(chunk_slugs),
        "chunksWithFacts": chunks_with_fact,
        "chunksWithValueWarnings": chunks_with_value_warnings,
        "totalPassed": total_passed,
        "totalFactMethods": total_fact,
        "chunkSummaries": chunk_summaries,
    }
    (latest_dir / "fact-summary.json").write_text(
        json.dumps(fact_summary, indent=2), encoding="utf-8")

    # benchmark-summary.json
    bench_summary = {
        "assemblyName": ctx.assembly,
        "timestamp": time.strftime("%Y-%m-%dT%H:%M:%S"),
        "totalChunks": len(chunk_slugs),
        "totalBenchmarkedMethods": total_benchmarked,
        "aggregatePerformance": aggregate_perf,
        "chunkSummaries": chunk_summaries,
    }
    (latest_dir / "benchmark-summary.json").write_text(
        json.dumps(bench_summary, indent=2), encoding="utf-8")

    # coverage-audit.json
    coverage_audit = {
        "assemblyName": ctx.assembly,
        "timestamp": time.strftime("%Y-%m-%dT%H:%M:%S"),
        "totalChunks": len(chunk_slugs),
        "chunksWithResults": chunks_with_fact,
        "chunksWithValueWarnings": chunks_with_value_warnings,
        "chunksWithMetaMismatch": chunks_with_meta_mismatch,
        "totalDeclaredMethods": total_fact,
    }
    (latest_dir / "coverage-audit.json").write_text(
        json.dumps(coverage_audit, indent=2), encoding="utf-8")

    # dashboard.json
    dashboard = {
        "assemblyName": ctx.assembly,
        "timestamp": time.strftime("%Y-%m-%dT%H:%M:%S"),
        "summary": {
            "chunks": len(chunk_slugs),
            "chunksVerified": chunks_with_fact,
            "chunksWithValueWarnings": chunks_with_value_warnings,
            "chunksWithMetaMismatch": chunks_with_meta_mismatch,
            "factPassRate": round(total_passed / total_fact * 100, 1) if total_fact else 0,
            "totalBenchmarkedMethods": total_benchmarked,
            "aggregatePerformance": aggregate_perf,
            "hotupdate": {
                "chunksWithPatchData": chunks_with_patch_data,
                "chunksPatchFailed": chunks_with_patch_failed,
                "chunksWithRevertFailure": chunks_with_revert_failure,
                "totalPassed": total_hu_passed,
                "totalFailed": total_hu_failed,
                "skipBreakdown": hotupdate_skip_statuses,
            },
        },
    }

    # Read benchmark comparison summary if available
    comparison_path = latest_dir / "comparison-summary.json"
    if comparison_path.exists():
        cs = json.loads(comparison_path.read_text(encoding="utf-8"))
        dashboard["summary"]["benchmarkComparison"] = {
            "methodsAnalyzed": cs.get("totalMethods", 0),
            "methodsWithNet8": cs.get("methodsWithNet8", 0),
            "chaosAotVsNet8": cs.get("aggregate", {}).get("chaosAotVsNet8Pct", {}),
            "chaosJitVsNet8": cs.get("aggregate", {}).get("chaosJitVsNet8Pct", {}),
            "net10VsNet8": cs.get("aggregate", {}).get("net10VsNet8Pct", {}),
            "highValueMethods_betterThanNet8": cs.get("aggregate", {}).get("highValueMethods_betterThanNet8", 0),
        }
    (latest_dir / "dashboard.json").write_text(
        json.dumps(dashboard, indent=2), encoding="utf-8")

    # History: benchmark data for regression detection
    today = date.today().isoformat()
    history_entry = {
        "date": today,
        "assemblyName": ctx.assembly,
        "chunks": chunk_summaries,
    }
    (history_dir / f"benchmark-{today}.json").write_text(
        json.dumps(history_entry, indent=2), encoding="utf-8")

    duration_ms = int((time.perf_counter() - start) * 1000)
    print(f"  [aggregate] Reports written to {latest_dir}")
    print(f"  [aggregate] Fact: {total_passed}/{total_fact} passed across {chunks_with_fact} chunks")
    if chunks_with_value_warnings:
        print(f"  [aggregate] Value warnings: {chunks_with_value_warnings} chunk(s)")
    if chunks_with_meta_mismatch:
        print(f"  [aggregate] Metadata mismatches: {chunks_with_meta_mismatch} chunk(s)")
    print(f"  [aggregate] Benchmark: {total_benchmarked} methods")
    print(f"  [aggregate] Done ({duration_ms}ms)")

    # UPGRADE: severe metadata mismatch (>50% gap) or value warnings
    # make aggregate partial rather than hiding issues behind a clean "passed".
    aggregate_status = "passed"
    if chunks_with_meta_mismatch > 0:
        aggregate_status = "partial"
    if chunks_with_value_warnings > 0 and aggregate_status == "passed":
        aggregate_status = "partial"

    # Build summary suffix for partial status details
    partial_reasons = []
    if chunks_with_meta_mismatch:
        partial_reasons.append(f"{chunks_with_meta_mismatch} meta-mismatch")
    if chunks_with_value_warnings:
        partial_reasons.append(f"{chunks_with_value_warnings} value-warn")

    return StageResult(
        stage="aggregate", status=aggregate_status,
        summary=f"aggregated {chunks_with_fact}/{len(chunk_slugs)} chunks, {total_passed}/{total_fact} passed"
                + (f" ({', '.join(partial_reasons)})" if partial_reasons else ""),
        details=fact_summary,
        duration_ms=duration_ms,
    )
