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
    chunks_dir = foundation_dir / assembly / "chunks"
    reports_dir = foundation_dir / assembly / "_dll" / "reports"
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
            }
        else:
            summary["hotupdate"] = {"status": "no_results"}

        chunk_summaries.append(summary)

    # ── Build aggregate metrics ──
    total_passed = sum(s.get("fact", {}).get("passed", 0) for s in chunk_summaries)
    total_fact = sum(s.get("fact", {}).get("total", 0) for s in chunk_summaries)
    total_benchmarked = sum(s.get("benchmark", {}).get("methodCount", 0) for s in chunk_summaries)
    chunks_with_fact = sum(1 for s in chunk_summaries if "passed" in s.get("fact", {}))

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

    # ── Write reports ──
    latest_dir.mkdir(parents=True, exist_ok=True)
    history_dir.mkdir(parents=True, exist_ok=True)

    # fact-summary.json
    fact_summary = {
        "assemblyName": ctx.assembly,
        "timestamp": time.strftime("%Y-%m-%dT%H:%M:%S"),
        "totalChunks": len(chunk_slugs),
        "chunksWithFacts": chunks_with_fact,
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
            "factPassRate": round(total_passed / total_fact * 100, 1) if total_fact else 0,
            "totalBenchmarkedMethods": total_benchmarked,
            "aggregatePerformance": aggregate_perf,
        },
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
    print(f"  [aggregate] Benchmark: {total_benchmarked} methods")
    print(f"  [aggregate] Done ({duration_ms}ms)")

    return StageResult(
        stage="aggregate", status="passed",
        summary=f"aggregated {chunks_with_fact}/{len(chunk_slugs)} chunks, {total_passed}/{total_fact} passed",
        details=fact_summary,
        duration_ms=duration_ms,
    )
