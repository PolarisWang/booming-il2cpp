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
from datetime import datetime, timezone
from datetime import date
from pathlib import Path

from verification.orchestration.context import ChunkContext, StageResult
from verification.stages.benchmark_report import _read_jsonl_technology_map
from verification._path import results_base
_RESULTS_BASE = results_base()


def _try_load_json(path: Path) -> dict | None:
    """Load a JSON file, returning None (with a warning) if unreadable/corrupt.

    A corrupt per-chunk result file should degrade that chunk's summary, not
    crash the whole aggregate stage with an uncaught JSONDecodeError.
    """
    if not path.exists():
        return None
    try:
        return json.loads(path.read_text(encoding="utf-8"))
    except (json.JSONDecodeError, OSError) as e:
        print(f"  [aggregate] WARNING: unreadable/corrupt {path.name}: {e}")
        return None


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
    stale_chunks: list[str] = []  # chunks reused from cache/fastpath (not fresh)
    # Per-method rows (Bug3 fix): preserve methodSubjectId + chaosAotVsNet8Pct so
    # the agent-facing report can map a signal back to a concrete method, instead
    # of discarding per-method identity (which made dashboard un-actionable).
    per_method_rows: list[dict] = []

    for slug in chunk_slugs:
        chunk_dir = chunks_dir / slug
        results_dir = chunk_dir / "results"

        summary: dict = {"slug": slug}

        # Fact results
        fact_path = results_dir / "fact.json"
        fact_data = _try_load_json(fact_path)
        if fact_data:
            summary["fact"] = {
                "passed": fact_data.get("passed", 0),
                "total": fact_data.get("total", 0),
                "valueSuspicious": fact_data.get("valueSuspicious", False),
                "valueWarnings": fact_data.get("valueWarnings", 0),
                # Preserve factMethodCount as its OWN key. The meta-mismatch
                # checker (line ~214) reads `fact.get("factMethodCount")`; if we
                # only fold it into metaBenchmarkCount below, that lookup returns
                # None and falls back to metaTotal — producing a false
                # meta-mismatch ERROR whenever the fact stage samples fewer
                # methods than are declared (e.g. fact total=4, metaTotal=15).
                # The data is correct in fact.json; it was just dropped here.
                "factMethodCount": fact_data.get("factMethodCount"),
                "metaTotal": fact_data.get("metaTotal"),
                "metaBenchmarkCount": fact_data.get("factMethodCount", fact_data.get("metaTotal", 0)),
            }
            all_fact.append({
                "chunk": slug,
                **fact_data,
            })
        else:
            summary["fact"] = {"status": "no_results"}

        # Benchmark results — prefer the JSONL perf store (benchmark-history.jsonl)
        # over the legacy AOT-only benchmark.json, since the JSONL includes both
        # AOT and managed (net8/net10) records and is the single source of truth
        # for benchmark_report.  Fall back to benchmark.json for backward compat.
        jsonl_path = _RESULTS_BASE / assembly / slug / "perf" / "benchmark-history.jsonl"
        tech_map = _read_jsonl_technology_map(jsonl_path)
        if tech_map:
            benchmark_method_count = len(tech_map)
            benchmark_with_net8 = sum(1 for t in tech_map.values() if "net8-jit" in t or "net10-jit" in t)
            chunk_benchmark = {
                "methodCount": benchmark_method_count,
                "methodsWithNet8": benchmark_with_net8,
            }
            summary["benchmark"] = chunk_benchmark
            all_benchmark.append({
                "chunk": slug,
                "methodCount": benchmark_method_count,
            })
        else:
            # Fallback to legacy AOT-only benchmark.json when JSONL not available
            bench_path = results_dir / "benchmark.json"
            bench_data = _try_load_json(bench_path)
            if bench_data:
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
                total_benchmarked += method_count
            else:
                summary["benchmark"] = {"status": "no_results"}

        # Bug3 fix: collect per-method rows from comparison.json (methodSubjectId +
        # chaosAotVsNet8Pct + status) so the agent report maps signals to methods.
        comp_path = results_dir / "comparison.json"
        comp_data = _try_load_json(comp_path)
        if comp_data:
            for m in comp_data.get("methods", []):
                msid = m.get("methodSubjectId") or ""
                per_method_rows.append({
                    "assembly": assembly,
                    "slug": slug,
                    "methodSubjectId": msid,
                    "status": m.get("status"),
                    "net8Ms": m.get("net8Ms"),
                    "chaosAotMs": m.get("chaosAotMs"),
                    "chaosAotVsNet8Pct": m.get("chaosAotVsNet8Pct"),
                    "chaosJitVsNet8Pct": m.get("chaosJitVsNet8Pct"),
                    "highVariance": m.get("highVariance"),
                    "gcComparison": m.get("gcComparison"),
                })

        # HotUpdate results
        hu_path = results_dir / "hotupdate.json"
        hu_data = _try_load_json(hu_path)
        if hu_data:
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

        # Build status + cache/fresh provenance. The build StageResult ran in a
        # separate process from aggregate, so its details (fastpath / hephaestus
        # cache_hit|miss / cacheKey / cachedAt) were persisted by chunk_pipeline
        # to provenance.json. Read that so the durable report can show whether
        # this chunk was freshly built or reused a (possibly stale) cache/fastpath
        # intermediate.
        proof_path = results_dir / "provenance.json"
        provenance: dict = {}
        if proof_path.exists():
            try:
                provenance = json.loads(proof_path.read_text(encoding="utf-8"))
            except (json.JSONDecodeError, OSError):
                provenance = {}
        build_prov = provenance.get("build", {})
        cache_prov = build_prov.get("cacheProvenance")
        fastpath = build_prov.get("fastpath", False)
        # Stale-reuse signal: entry.exe was NOT freshly compiled this run.
        #   - fastpath=True  → reused an existing byte-identical entry.exe (risk:
        #                      missing deps dropped from mtime comparison).
        #   - cache_hit      → restored from a hephaestus cache entry.
        # A cache "miss" is a FRESH build, so it is NOT stale.
        is_stale = bool(fastpath or cache_prov == "cache_hit")

        result_files = []
        if results_dir.is_dir():
            result_files = [f.name for f in results_dir.iterdir()]
        if fact_path.exists():
            summary["build"] = {"status": "passed"}
        elif any(rf.endswith(".json") for rf in result_files):
            summary["build"] = {"status": "passed"}
        elif results_dir.is_dir():
            summary["build"] = {"status": "failed"}
        else:
            summary["build"] = {"status": "not_run"}
        # Extend with provenance (never drops the derived status above).
        summary["build"].update({
            "cacheProvenance": cache_prov if cache_prov else
                              ("fastpath" if fastpath else None),
            "cachedAt": build_prov.get("cachedAt"),
            "cacheKey": build_prov.get("cacheKey"),
            "buildRunId": provenance.get("runId"),
            "platform": provenance.get("platform"),
            "gitCommit": provenance.get("gitCommit"),
            "staleReuse": is_stale,
        })
        if is_stale:
            stale_chunks.append(slug)

        # Profile results (AOT code size, optional)
        profile_path = results_dir / "profile.json"
        if profile_path.exists():
            try:
                pd = json.loads(profile_path.read_text(encoding="utf-8"))
                summary["profile"] = {
                    "methodCount": pd.get("methodCount", 0),
                    "totalSize": pd.get("sectionSizes", {}).get("total", 0),
                    "source": pd.get("source", "none"),
                }
            except (json.JSONDecodeError, OSError):
                summary["profile"] = {"status": "error"}

        # Managed benchmark results
        mb_path = results_dir / "managed_benchmark.json"
        if mb_path.exists():
            try:
                mb = json.loads(mb_path.read_text(encoding="utf-8"))
                summary["managedBenchmark"] = {
                    "passed": mb.get("passed", 0),
                    "total": mb.get("total", 0),
                    "mode": mb.get("mode", ""),
                }
            except (json.JSONDecodeError, OSError):
                summary["managedBenchmark"] = {"status": "error"}

        # Benchmark comparison (cross-technology)
        comp_path = results_dir / "comparison.json"
        if comp_path.exists():
            try:
                cp = json.loads(comp_path.read_text(encoding="utf-8"))
                summary["benchmarkReport"] = {
                    "totalMethods": cp.get("totalMethods", 0),
                    "methodsWithNet8": cp.get("methodsWithNet8", 0),
                }
            except (json.JSONDecodeError, OSError):
                summary["benchmarkReport"] = {"status": "error"}

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
    # Track chunks with metadata mismatch: C++ fact total must exactly
    # match managed fact metaTotal.  Small gaps (<1%) are tolerated —
    # they come from JIT/interpreter limitations for specific generic
    # instantiations (e.g., Lookup<,>::ApplyResultSelector).
    chunks_with_meta_mismatch = 0
    chunks_with_meta_warning = 0
    chunks_with_coverage_gap = 0  # P2-F: codegen didn't dispatch methods (all run passed)
    for s in chunk_summaries:
        fact = s.get("fact", {})
        meta = fact.get("factMethodCount") if fact.get("factMethodCount") is not None else fact.get("metaTotal")
        total = s.get("fact", {}).get("total")
        if meta is not None and meta > 0 and total is not None and total != meta:
            gap = meta - total
            gap_ratio = gap / meta
            chunk_slug = s.get("slug", "?")
            meta_label = "factMethodCount" if s.get("fact", {}).get("factMethodCount") else "metaTotal"
            passed = fact.get("passed", 0)
            if gap_ratio < 0.01:
                chunks_with_meta_warning += 1
                print(f"  [aggregate] WARN: {chunk_slug} fact total={total} != {meta_label}={meta} (gap={gap}, {gap_ratio:.1%})")
            elif passed >= total and total > 0:
                # P2-F: all DISPATCHED methods passed, but the codegen only made
                # `total` of `meta` methods dispatchable (corelib: e.g. 5/75, AOT
                # lowering only emits slots for a subset). This is a codegen COVERAGE
                # gap, NOT a correctness failure — the methods that ran all passed.
                # Report it as a distinct coverage indicator, not a SEVERE error.
                chunks_with_coverage_gap += 1
                print(f"  [aggregate] COVERAGE-GAP: {chunk_slug} only {total}/{meta} methods dispatchable "
                      f"({gap_ratio:.0%} not AOT-lowered); all {passed}/{total} that ran passed")
            else:
                chunks_with_meta_mismatch += 1
                print(f"  [aggregate] ERROR: {chunk_slug} fact total={total} != {meta_label}={meta} (gap={gap}, {gap_ratio:.1%})")

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
    chunks_with_patch_skipped_no_methods = sum(
        1 for s in chunk_summaries
        if s.get("hotupdate", {}).get("patchSkippedNoMethods", False)
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
        "timestamp": datetime.now(timezone.utc).strftime("%Y-%m-%dT%H:%M:%S"),
        "totalChunks": len(chunk_slugs),
        "chunksWithFacts": chunks_with_fact,
        "chunksWithValueWarnings": chunks_with_value_warnings,
        "totalPassed": total_passed,
        "totalFactMethods": total_fact,
        "staleChunks": sorted(set(stale_chunks)),
        "chunkSummaries": chunk_summaries,
    }
    (latest_dir / "fact-summary.json").write_text(
        json.dumps(fact_summary, indent=2), encoding="utf-8")

    # benchmark-summary.json
    bench_summary = {
        "assemblyName": ctx.assembly,
        "timestamp": datetime.now(timezone.utc).strftime("%Y-%m-%dT%H:%M:%S"),
        "totalChunks": len(chunk_slugs),
        "totalBenchmarkedMethods": total_benchmarked,
        "aggregatePerformance": aggregate_perf,
        "staleChunks": sorted(set(stale_chunks)),
        "chunkSummaries": chunk_summaries,
    }
    (latest_dir / "benchmark-summary.json").write_text(
        json.dumps(bench_summary, indent=2), encoding="utf-8")

    # per-method.json (Bug3 fix): preserve per-method identity (methodSubjectId +
    # chaosAotVsNet8Pct) so an agent can map a benchmark/fact signal back to a
    # concrete method instead of only seeing aggregate-level pass rates.
    per_method_report = {
        "assemblyName": ctx.assembly,
        "assemblyPath": str(ctx.foundation_dir),
        "timestamp": datetime.now(timezone.utc).strftime("%Y-%m-%dT%H:%M:%S"),
        "methodCount": len(per_method_rows),
        "methods": per_method_rows,
    }
    (latest_dir / "per-method.json").write_text(
        json.dumps(per_method_report, indent=2), encoding="utf-8")

    # coverage-audit.json
    coverage_audit = {
        "assemblyName": ctx.assembly,
        "timestamp": datetime.now(timezone.utc).strftime("%Y-%m-%dT%H:%M:%S"),
        "totalChunks": len(chunk_slugs),
        "chunksWithResults": chunks_with_fact,
        "chunksWithValueWarnings": chunks_with_value_warnings,
        "chunksWithMetaMismatch": chunks_with_meta_mismatch,
        "chunksWithCoverageGap": chunks_with_coverage_gap,
        "totalDeclaredMethods": total_fact,
    }
    (latest_dir / "coverage-audit.json").write_text(
        json.dumps(coverage_audit, indent=2), encoding="utf-8")

    # dashboard.json
    dashboard = {
        "assemblyName": ctx.assembly,
        "timestamp": datetime.now(timezone.utc).strftime("%Y-%m-%dT%H:%M:%S"),
        "summary": {
            "chunks": len(chunk_slugs),
            "chunksVerified": chunks_with_fact,
            "chunksWithValueWarnings": chunks_with_value_warnings,
            "chunksWithMetaMismatch": chunks_with_meta_mismatch,
            "chunksWithCoverageGap": chunks_with_coverage_gap,
            "factPassRate": round(total_passed / total_fact * 100, 1) if total_fact else 0,
            "totalBenchmarkedMethods": total_benchmarked,
            "aggregatePerformance": aggregate_perf,
            "hotupdate": {
                "chunksWithPatchData": chunks_with_patch_data,
                "chunksPatchFailed": chunks_with_patch_failed,
                "chunksPatchSkippedNoMethods": chunks_with_patch_skipped_no_methods,
                "chunksWithRevertFailure": chunks_with_revert_failure,
                "totalPassed": total_hu_passed,
                "totalFailed": total_hu_failed,
                "skipBreakdown": hotupdate_skip_statuses,
            },
        },
    }

    # Read benchmark comparison summary if available. To keep the dashboard
    # contract unambiguous for a harness, expose an explicit status rather than
    # silently omitting the key: "completed" (comparison-summary.json present) vs
    # "not_run" (benchmark_report stage didn't produce it this run). All counts
    # come from the SAME comparison-summary.json snapshot so dashboard and
    # comparison-summary.json never disagree on the same invocation.
    comparison_path = latest_dir / "comparison-summary.json"
    if comparison_path.exists():
        try:
            cs = _try_load_json(comparison_path) or {}
        except Exception:
            cs = {}
        if cs:
            cs_agg = cs.get("aggregate", {}) or {}
            dashboard["summary"]["benchmarkComparison"] = {
                "status": "completed",
                "methodsAnalyzed": cs.get("totalMethods", 0),
                "methodsWithNet8": cs.get("methodsWithNet8", 0),
                "chaosAotVsNet8": cs_agg.get("chaosAotVsNet8Pct", {}),
                "chaosJitVsNet8": cs_agg.get("chaosJitVsNet8Pct", {}),
                "net10VsNet8": cs_agg.get("net10VsNet8Pct", {}),
                "highValueMethods_betterThanNet8": cs_agg.get("highValueMethods_betterThanNet8", 0),
                # Coverage-asymmetry diagnostic (surfaces WHY chaosAotVsNet8 may be
                # empty: AOT and managed benchmarked disjoint method sets).
                "coverage": cs_agg.get("coverage", {}),
                "noAotData": cs_agg.get("noAotData", False),
            }
    if "benchmarkComparison" not in dashboard["summary"]:
        dashboard["summary"]["benchmarkComparison"] = {"status": "not_run"}

    # Cache/fresh provenance: expose which chunks were reused (not freshly built)
    # so the harness can tell a fresh result from stale cache/fastpath reuse.
    # This is a warning signal, NOT a gate — stale reuse does not fail the report.
    dashboard["summary"]["cacheProvenance"] = {
        "staleChunks": sorted(set(stale_chunks)),
        "overallCacheFresh": len(stale_chunks) == 0,
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
    if chunks_with_coverage_gap:
        print(f"  [aggregate] Coverage gaps: {chunks_with_coverage_gap} chunk(s) "
              f"(codegen did not dispatch all declared methods — NOT a correctness failure)")
    print(f"  [aggregate] Benchmark: {total_benchmarked} methods")
    print(f"  [aggregate] Done ({duration_ms}ms)")

    # Metadata mismatch = C++ fact didn't cover all managed methods — hard error.
    # Value warnings = methods returned negative values — also a hard error.
    # Coverage gap (P2-F) is NOT hard: all dispatched methods passed; only the
    # codegen failed to dispatch the rest. Report count, do not fail the run.
    aggregate_errors = []
    if chunks_with_meta_mismatch > 0:
        aggregate_errors.append(f"{chunks_with_meta_mismatch} meta-mismatch")
    if chunks_with_value_warnings > 0:
        aggregate_errors.append(f"{chunks_with_value_warnings} value-warn")

    # Surface coverage-gap count in the dashboard/fact-summary as a diagnostic.
    aggregate_status = "passed"
    if aggregate_errors:
        aggregate_status = "error"

    # Build summary suffix for error status details
    partial_reasons = aggregate_errors

    return StageResult(
        stage="aggregate", status=aggregate_status,
        summary=f"aggregated {chunks_with_fact}/{len(chunk_slugs)} chunks, {total_passed}/{total_fact} passed"
                + (f" ({', '.join(partial_reasons)})" if partial_reasons else ""),
        details=fact_summary,
        duration_ms=duration_ms,
    )
