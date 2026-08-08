"""Harness-readiness regression tests (T-track: harness core capability).

Verifies the fixes that make the verification engine trustworthy as a
development-driving harness:

  1. P0-A1 — _write_combined_perf_store aggregates ALL technologies into
     benchmark-history.jsonl in one "w" pass (no overwrite of chaos-aot by
     a later chaos-jit writer).
  2. P0-A2 — chaos-aot failure surfaces as a strict warning (not silently
     masked by a succeeding chaos-jit).
  3. P0-B — cache/fresh provenance (fastpath/cache_hit/miss) flows into the
     aggregate summary / durable reports.
  4. P1-C — comparison-summary.json and dashboard.json agree on the same
     benchmark comparison fields.
"""
from __future__ import annotations

import json
import sys
from pathlib import Path

import pytest

_HERE = Path(__file__).resolve().parent
for _d in (_HERE, _HERE.parent, _HERE.parent.parent):
    if str(_d) not in sys.path:
        sys.path.insert(0, str(_d))

from verification.orchestration.context import ChunkContext  # noqa: E402


def _make_ctx(tmp_path: Path, assembly: str = "System.Sample",
              slug: str = "global-ns") -> ChunkContext:
    """Build a ChunkContext isolated under tmp_path so writes never touch real tree."""
    foundation = tmp_path / assembly
    chunk = foundation / "chunks" / slug
    (chunk / "native").mkdir(parents=True, exist_ok=True)
    (chunk / "results").mkdir(parents=True, exist_ok=True)
    return ChunkContext(
        slug=slug, assembly=assembly,
        chunk_dir=chunk, foundation_dir=foundation,
    )


# ──────────────────────────────────────────────────────────────────────────
# P0-A1: combined perf-store aggregation (no chaos-aot overwrite)
# ──────────────────────────────────────────────────────────────────────────
def test_perf_store_aggregates_multiple_technologies(tmp_path: Path, monkeypatch) -> None:
    """Two technologies' records must both appear after a combined write.

    Before the fix, _write_perf_store opened benchmark-history.jsonl in "w"
    mode per technology, so chaos-aot was overwritten by the subsequent
    chaos-jit write — the root cause of missing chaos-aot benchmark data.
    """
    from verification.stages import benchmark_chunk as bc

    ctx = _make_ctx(tmp_path)
    # Redirect the module-level results base into tmp to keep test-hermetic.
    monkeypatch.setattr(bc, "_RESULTS_BASE", tmp_path / "results" / "foundation-dll")

    metadata_methods = [
        {"methodSubjectId": "M::A()"},
        {"methodSubjectId": "M::B()"},
    ]
    aot_stats = [
        {"meanDurationMs": 5.0, "meanOpsPerSecond": 200000.0},
        {"meanDurationMs": 6.0, "meanOpsPerSecond": 166666.0},
    ]
    jit_stats = [
        {"meanDurationMs": 4.0, "meanOpsPerSecond": 250000.0},
        {"meanDurationMs": 3.0, "meanOpsPerSecond": 333333.0},
    ]

    bc._write_combined_perf_store(
        [("chaos-aot", aot_stats, 100), ("chaos-jit", jit_stats, 100)],
        ctx, metadata_methods,
    )

    perf_path = bc._RESULTS_BASE / ctx.assembly / ctx.slug / "perf" / "benchmark-history.jsonl"
    assert perf_path.exists()

    techs: dict[str, int] = {}
    for line in perf_path.read_text(encoding="utf-8").splitlines():
        line = line.strip()
        if not line:
            continue
        rec = json.loads(line)
        techs[rec["technology"]] = techs.get(rec["technology"], 0) + 1

    # Both technologies present with their full method counts (not overwritten).
    assert techs.get("chaos-aot") == 2
    assert techs.get("chaos-jit") == 2


def test_perf_store_single_tech_backward_compat(tmp_path: Path, monkeypatch) -> None:
    """_write_perf_store (single-tech wrapper) still writes one tech's rows."""
    from verification.stages import benchmark_chunk as bc

    ctx = _make_ctx(tmp_path)
    monkeypatch.setattr(bc, "_RESULTS_BASE", tmp_path / "results" / "foundation-dll")

    bc._write_perf_store(
        [{"meanDurationMs": 1.0, "meanOpsPerSecond": 1e6}],
        ctx, "chaos-aot", [{"methodSubjectId": "M::A()"}], 10,
    )
    perf_path = bc._RESULTS_BASE / ctx.assembly / ctx.slug / "perf" / "benchmark-history.jsonl"
    recs = [json.loads(l) for l in perf_path.read_text(encoding="utf-8").splitlines() if l.strip()]
    assert len(recs) == 1
    assert recs[0]["technology"] == "chaos-aot"


# ──────────────────────────────────────────────────────────────────────────
# P0-A2: chaos-aot absence is a strict warning, not silent-masked by chaos-jit
# ──────────────────────────────────────────────────────────────────────────
def _fake_bench_result(tech: str, n: int = 2) -> dict:
    """A minimal _run_single_benchmark return for a technology."""
    stats = [{"meanDurationMs": 5.0, "meanOpsPerSecond": 2e5} for _ in range(n)]
    return {
        "per_method_stats": stats,
        "summary": {"totalOutliers": 0, "methodCount": n},
        "iterations": 100,
        "sample_rounds": 3,
        "method_count": n,
        "tech_duration_ms": 100,
        "technology": tech,
    }


def test_benchmark_aot_absent_is_warning_not_silent(tmp_path: Path, monkeypatch) -> None:
    """AOT (chaos-aot) producing no data must surface as a strict warning.

    Before the fix, a succeeding chaos-jit masked the AOT failure entirely
    (benchmark reported "passed"). After: status == "warning" (not silent),
    JIT data retained, pipeline would not hard-fail but harness sees it.
    """
    from verification.stages import benchmark_chunk as bc

    ctx = _make_ctx(tmp_path)
    monkeypatch.setattr(bc, "_RESULTS_BASE", tmp_path / "results" / "foundation-dll")

    # Fake both entry.exe (AOT) and entry-jit.exe as existing so both techs are
    # in the list; but make AOT return None (no data) and JIT succeed.
    monkeypatch.setattr(ctx.__class__, "entry_exe_path", property(lambda self: Path(tmp_path) / "entry.exe"))
    monkeypatch.setattr(ctx.__class__, "entry_jit_exe_path", property(lambda self: Path(tmp_path) / "entry-jit.exe"))
    (tmp_path / "entry.exe").touch()
    (tmp_path / "entry-jit.exe").touch()
    monkeypatch.setattr(bc, "_read_benchmark_metadata", lambda c: [])

    def fake_single(exe, tech, c, timeout, metadata):
        if tech == "chaos-aot":
            return None  # AOT silently fails
        if tech == "chaos-jit":
            return _fake_bench_result("chaos-jit")
        return _fake_bench_result(tech)

    monkeypatch.setattr(bc, "_run_single_benchmark", fake_single)
    monkeypatch.setattr(ctx.__class__, "results_dir", property(lambda self: Path(tmp_path) / "results"))
    monkeypatch.setattr(bc, "_write_combined_perf_store", lambda *a, **k: None)
    monkeypatch.setattr(bc, "_write_records_jsonl", lambda *a, **k: None)

    sr = bc.run_benchmark_chunk(ctx, {})

    assert sr.status == "warning", f"expected warning, got {sr.status!r}"
    assert sr.details.get("chaosAot") == "no_data"


def test_benchmark_aot_present_is_passed(tmp_path: Path, monkeypatch) -> None:
    """When AOT produces data, benchmark is passed (no spurious warning)."""
    from verification.stages import benchmark_chunk as bc

    ctx = _make_ctx(tmp_path)
    monkeypatch.setattr(bc, "_RESULTS_BASE", tmp_path / "results" / "foundation-dll")
    monkeypatch.setattr(ctx.__class__, "entry_exe_path", property(lambda self: Path(tmp_path) / "entry.exe"))
    monkeypatch.setattr(ctx.__class__, "entry_jit_exe_path", property(lambda self: Path(tmp_path) / "entry-jit.exe"))
    (tmp_path / "entry.exe").touch()
    (tmp_path / "entry-jit.exe").touch()
    monkeypatch.setattr(bc, "_read_benchmark_metadata", lambda c: [])
    monkeypatch.setattr(bc, "_run_single_benchmark",
                        lambda exe, tech, c, timeout, metadata: _fake_bench_result(tech))
    monkeypatch.setattr(ctx.__class__, "results_dir", property(lambda self: Path(tmp_path) / "results"))
    monkeypatch.setattr(bc, "_write_combined_perf_store", lambda *a, **k: None)
    monkeypatch.setattr(bc, "_write_records_jsonl", lambda *a, **k: None)

    sr = bc.run_benchmark_chunk(ctx, {})
    assert sr.status == "passed", f"expected passed, got {sr.status!r}"


# ──────────────────────────────────────────────────────────────────────────
# P0-B: cache/fresh provenance flows into aggregate durable reports
# ──────────────────────────────────────────────────────────────────────────
def _chunk_tree_with_provenance(tmp_path: Path, provenance: dict) -> list:
    """Build a one-chunk foundation tree with fact.json + provenance.json, run
    run_aggregate, and return [foundation, chunk_summaries]."""
    from verification.stages.aggregate import run_aggregate

    foundation = tmp_path / "System.Sample"
    chunk = foundation / "chunks" / "global-ns"
    (chunk / "results").mkdir(parents=True)
    # Minimal fact + benchmark so aggregate's primary keys resolve.
    (chunk / "results" / "fact.json").write_text(json.dumps({
        "passed": 4, "total": 4, "factMethodCount": 4, "metaTotal": 4,
    }), encoding="utf-8")
    (chunk / "results" / "benchmark.json").write_text(json.dumps({
        "methodCount": 4, "results": [{"methodIndex": i} for i in range(4)],
    }), encoding="utf-8")
    if provenance is not None:
        (chunk / "results" / "provenance.json").write_text(
            json.dumps(provenance), encoding="utf-8")

    ctx = ChunkContext(slug="global-ns", assembly="System.Sample",
                       chunk_dir=chunk, foundation_dir=foundation)
    sr = run_aggregate(ctx, {})
    # Read the written fact-summary to inspect durable output.
    fact_summary = json.loads(
        (foundation / "_dll" / "reports" / "latest" / "fact-summary.json")
        .read_text(encoding="utf-8"))
    dashboard = json.loads(
        (foundation / "_dll" / "reports" / "latest" / "dashboard.json")
        .read_text(encoding="utf-8"))
    return sr, fact_summary, dashboard


def test_aggregate_surfaces_fastpath_stale(tmp_path: Path) -> None:
    """Fastpath-reused entry.exe shows in staleChunks (not cache_fresh)."""
    sr, fact_summary, dashboard = _chunk_tree_with_provenance(tmp_path, {
        "runId": "fdn-test",
        "platform": "windows-x64",
        "build": {"status": "passed", "fastpath": True,
                  "cacheProvenance": None, "cacheKey": None, "cachedAt": ""},
    })
    assert sr.status == "passed"
    assert fact_summary["staleChunks"] == ["global-ns"]
    assert dashboard["summary"]["cacheProvenance"]["overallCacheFresh"] is False
    assert "global-ns" in dashboard["summary"]["cacheProvenance"]["staleChunks"]


def test_aggregate_fresh_miss_is_not_stale(tmp_path: Path) -> None:
    """A cache miss (fresh build) must NOT be flagged stale."""
    sr, fact_summary, dashboard = _chunk_tree_with_provenance(tmp_path, {
        "build": {"status": "passed", "fastpath": False,
                  "cacheProvenance": "miss", "cacheKey": "k1", "cachedAt": ""},
    })
    assert fact_summary["staleChunks"] == []
    assert dashboard["summary"]["cacheProvenance"]["overallCacheFresh"] is True


def test_aggregate_no_provenance_file_is_not_stale(tmp_path: Path) -> None:
    """Absent provenance.json (older runs) is not falsely flagged stale."""
    sr, fact_summary, dashboard = _chunk_tree_with_provenance(tmp_path, None)
    assert fact_summary.get("staleChunks") == []
    assert dashboard["summary"]["cacheProvenance"]["overallCacheFresh"] is True


# ──────────────────────────────────────────────────────────────────────────
# P1-C: dashboard benchmarkComparison contract matches comparison-summary.json
# ──────────────────────────────────────────────────────────────────────────
def test_dashboard_benchmark_contract_matches_comparison(tmp_path: Path) -> None:
    """dashboard.benchmarkComparison must mirror comparison-summary.json exactly."""
    from verification.stages.aggregate import run_aggregate

    sr, _, dashboard = _chunk_tree_with_provenance(tmp_path, None)
    # The helper runs aggregate WITHOUT a comparison-summary.json -> not_run.
    assert dashboard["summary"]["benchmarkComparison"]["status"] == "not_run"

    # Now write a comparison-summary.json and re-run aggregate.
    latest = (tmp_path / "System.Sample" / "_dll" / "reports" / "latest")
    latest.mkdir(parents=True, exist_ok=True)
    (latest / "comparison-summary.json").write_text(json.dumps({
        "totalMethods": 5,
        "methodsWithNet8": 2,
        "aggregate": {
            "chaosAotVsNet8Pct": {"mean": -12.5, "median": -10.0},
            "net10VsNet8Pct": {"mean": 0.0},
            "highValueMethods_betterThanNet8": 50.0,
        },
    }), encoding="utf-8")

    foundation = tmp_path / "System.Sample"
    chunk = foundation / "chunks" / "global-ns"
    ctx = ChunkContext(slug="global-ns", assembly="System.Sample",
                       chunk_dir=chunk, foundation_dir=foundation)
    run_aggregate(ctx, {})
    dashboard2 = json.loads((latest / "dashboard.json").read_text(encoding="utf-8"))
    comp = dashboard2["summary"]["benchmarkComparison"]
    assert comp["status"] == "completed"
    assert comp["methodsWithNet8"] == 2
    assert comp["methodsAnalyzed"] == 5
    assert comp["chaosAotVsNet8"]["mean"] == -12.5
    assert comp["highValueMethods_betterThanNet8"] == 50.0


# ──────────────────────────────────────────────────────────────────────────
# P1-D: stage dependency topological sort
# ──────────────────────────────────────────────────────────────────────────
def test_toposort_orders_dependencies_first() -> None:
    """Chunks in arbitrary order are reordered so deps run before dependents."""
    from verification.chunk_pipeline import _toposort_stages

    deps = {
        "fact": ["build"], "benchmark": ["build", "fact"],
        "managed_benchmark": ["build"],
        "benchmark_report": ["benchmark", "managed_benchmark"],
        "hotupdate": ["build", "fact"], "coverage-audit": ["build", "fact"],
        "profile": ["build"], "aggregate": ["build", "fact", "coverage-audit"],
        "reporting": ["aggregate"],
    }
    # reporting listed before aggregate must be corrected to aggregate, reporting.
    assert _toposort_stages(["reporting", "aggregate"], deps) == ["aggregate", "reporting"]
    # coverage-audit before aggregate stays valid and ordering-preserving.
    assert _toposort_stages(["coverage-audit", "aggregate"], deps) == ["coverage-audit", "aggregate"]
    # A dependency not requested is ignored, not injected.
    assert _toposort_stages(["aggregate", "reporting"], deps) == ["aggregate", "reporting"]
    # Every dep precedes dependents for a full realistic run.
    full = _toposort_stages(
        ["reporting", "aggregate", "hotupdate", "coverage-audit", "benchmark", "fact", "build"],
        deps)
    for stage in full:
        for dep in deps.get(stage, []):
            assert dep in full, f"{stage} requires {dep} but it's absent"
            assert full.index(dep) < full.index(stage), f"{dep} not before {stage}"


# ──────────────────────────────────────────────────────────────────────────
# P2-E: three-state pipeline exit model (passed / warning / failed)
# ──────────────────────────────────────────────────────────────────────────
def test_exit_code_maps_status() -> None:
    """passed->0, warning->2, failed/error->1 (distinct, harness-detectable)."""
    from verification.chunk_pipeline import _exit_code_for_status

    assert _exit_code_for_status("passed") == 0
    assert _exit_code_for_status("warning") == 2
    assert _exit_code_for_status("failed") == 1
    assert _exit_code_for_status("error") == 1
    assert _exit_code_for_status("") == 1  # any unknown/tombstone -> fail


# ──────────────────────────────────────────────────────────────────────────
# 9th finding: AOT vs managed benchmark coverage-asymmetry diagnostic
# ──────────────────────────────────────────────────────────────────────────
def _rec(elapsed: float) -> dict:
    """A benchmark-history record with TOTAL elapsedMilliseconds (per the contract)."""
    return {"metrics": {"elapsedMilliseconds": elapsed, "opsPerSecond": 1e6},
            "iterations": 100}


def test_comparison_reports_disjoint_coverage(tmp_path: Path) -> None:
    """AOT and managed benchmarking disjoint methods -> overlap=0, noAotNet8Overlap.

    Mirrors the ServerSentEvents case: SseFormatter only has AOT data, SseParser
    only has managed data. The comparison must not crash and must surface WHY
    chaosAotVsNet8 is empty via the coverage diagnostic.
    """
    from verification.stages.benchmark_report import _build_method_comparison

    tech_map = {
        "M::SseFormatter::WriteAsync": {
            "chaos-aot": _rec(5.0),     # AOT covered
            # no net8/net10 record -> managed did not cover
        },
        "M::SseParser::Enumerate": {
            "net10-jit": _rec(1.0),     # managed covered
            # no chaos-aot record -> AOT did not cover
        },
    }
    methods, aggregate = _build_method_comparison(tech_map, "global-ns")

    cov = aggregate["coverage"]
    assert cov["overlappingMethods"] == 0
    assert cov["noAotNet8Overlap"] is True
    assert cov["aotCoveredMethods"] == 1
    assert cov["managedCoveredMethods"] == 1
    # No AOT pct because there's no overlap.
    assert "chaosAotVsNet8Pct" not in aggregate


def test_comparison_reports_overlap_and_ratio(tmp_path: Path) -> None:
    """A method benchmarked by BOTH AOT and managed yields a chaosAotVsNet8Pct."""
    from verification.stages.benchmark_report import _build_method_comparison

    tech_map = {
        "M::SharedMethod()": {
            "chaos-aot": _rec(5.0),      # AOT 5ms total / 100 iters = 0.05/iter
            "net8-jit": _rec(10.0),      # managed net8 10ms
        },
    }
    methods, aggregate = _build_method_comparison(tech_map, "global-ns")

    cov = aggregate["coverage"]
    assert cov["overlappingMethods"] == 1
    assert cov["noAotNet8Overlap"] is False
    # AOT is FASTER than net8 (5 < 10) -> positive pct (speedup).
    assert "chaosAotVsNet8Pct" in aggregate
    assert aggregate["chaosAotVsNet8Pct"]["mean"] > 0
    assert methods[0]["status"] == "completed"
    assert methods[0]["chaosAotVsNet8Pct"] is not None


# ──────────────────────────────────────────────────────────────────────────
# X-fixes: AI-export routeHint + schema-drift
# ──────────────────────────────────────────────────────────────────────────
def test_route_hint_maps_bottleneck_to_skill_name() -> None:
    """route_for_bottleneck_str must emit real skill names, not raw bottlenecks.

    Before: returned the raw bottleneck string (e.g. "gc_pause"), which no skill
    is named after, so the AI-export routeHint was non-routable.
    """
    from verification.tools.ai_export import route_for_bottleneck_str

    assert route_for_bottleneck_str("gc_pause") == "dev-il2cpp-gc-expert"
    assert route_for_bottleneck_str("alloc_hot") == "dev-il2cpp-codegen-expert"
    assert route_for_bottleneck_str("dispatch_overhead") == "dev-il2cpp-runtime-expert"
    assert route_for_bottleneck_str("") == "dev-il2cpp-runtime-expert"  # default


def test_enrichment_loads_benchmark_per_method_stats(tmp_path: Path) -> None:
    """reporting enrichment must surface benchmark/per-method into the run record.

    Before: the run record omitted benchmark.perMethodStats + fact.metadataMismatch,
    so ai_export's benchmark-regressions/memory-targets exports were always empty.
    """
    from verification.stages.reporting import _enrich_with_chunk_facts

    # Build a fake family chunk with comparison.json + fact.json (sampling gap).
    chunk = tmp_path / "chunks" / "global-ns"
    (chunk / "results").mkdir(parents=True)
    (chunk / "results" / "fact.json").write_text(json.dumps({
        "passed": 4, "total": 4, "factMethodCount": 4, "metaTotal": 15,
    }), encoding="utf-8")
    (chunk / "results" / "comparison.json").write_text(json.dumps({
        "methods": [
            {"methodSubjectId": "M::Regressed()", "status": "completed",
             "chaosAotVsNet8Pct": -40.0, "highVariance": False},
            {"methodSubjectId": "M::Faster()", "status": "completed",
             "chaosAotVsNet8Pct": 30.0, "highVariance": False},
        ],
    }), encoding="utf-8")

    asm_data: dict = {}
    _enrich_with_chunk_facts(asm_data, tmp_path / "chunks")
    chunk_entry = asm_data["chunks"][0]

    assert chunk_entry["fact"]["metadataMismatch"] is True
    stats = chunk_entry["benchmark"].get("perMethodStats", [])
    assert len(stats) == 2
    # The regressed method (pct<0) must carry a positive regressionDelta + bottleneck.
    regressed = next(s for s in stats if s["methodSubjectId"] == "M::Regressed()")
    assert regressed["regressionDelta"] == 40.0
    assert regressed["bottleneck"]  # non-empty route source
    # The faster method must NOT be flagged as a regression.
    faster = next(s for s in stats if s["methodSubjectId"] == "M::Faster()")
    assert faster["regressionDelta"] is None


def test_aggregate_writes_per_method_json(tmp_path: Path) -> None:
    """aggregate must preserve per-method identity in latest/per-method.json.

    Before: only aggregate-level pass rates survived; an agent couldn't map a
    signal (perf/fact) back to a concrete method. Bug3 fix writes per-method rows
    with methodSubjectId + chaosAotVsNet8Pct.
    """
    from verification.stages.aggregate import run_aggregate

    foundation = tmp_path / "System.Sample"
    chunk = foundation / "chunks" / "global-ns"
    (chunk / "results").mkdir(parents=True)
    (chunk / "results" / "fact.json").write_text(json.dumps({
        "passed": 1, "total": 1, "factMethodCount": 1, "metaTotal": 1,
    }), encoding="utf-8")
    (chunk / "results" / "benchmark.json").write_text(json.dumps({
        "methodCount": 1, "results": [], "summary": {"meanDurationMs": 1.0},
    }), encoding="utf-8")
    (chunk / "results" / "comparison.json").write_text(json.dumps({
        "methods": [
            {"methodSubjectId": "System.Sample/AM::Do()", "status": "completed",
             "chaosAotVsNet8Pct": -25.0, "net8Ms": 1.0, "chaosAotMs": 1.25},
        ],
    }), encoding="utf-8")

    ctx = ChunkContext(slug="global-ns", assembly="System.Sample",
                       chunk_dir=chunk, foundation_dir=foundation)
    run_aggregate(ctx, {})

    per_path = foundation / "_dll" / "reports" / "latest" / "per-method.json"
    assert per_path.exists()
    report = json.loads(per_path.read_text(encoding="utf-8"))
    assert report["methodCount"] == 1
    row = report["methods"][0]
    assert row["methodSubjectId"] == "System.Sample/AM::Do()"
    assert row["chaosAotVsNet8Pct"] == -25.0  # identity + signal both preserved
    assert row["slug"] == "global-ns"


# ──────────────────────────────────────────────────────────────────────────
# P2-F: codegen coverage gap classified separately from correctness failure
# ──────────────────────────────────────────────────────────────────────────
def test_aggregate_coverage_gap_not_correctness_failure(tmp_path: Path) -> None:
    """corelib-style partial dispatch (all run passed) = coverage gap, not error.

    Mirrors corelib numerics: factMethodCount=75 but only 5 dispatchable, all 5
    pass. Before P2-F this was a SEVERE meta-mismatch ERROR that failed the run;
    now it's a coverage-gap diagnostic, and aggregate stays passed.
    """
    from verification.stages.aggregate import run_aggregate

    foundation = tmp_path / "System.Sample"
    chunk = foundation / "chunks" / "global-ns"
    (chunk / "results").mkdir(parents=True)
    (chunk / "results" / "fact.json").write_text(json.dumps({
        "passed": 5, "total": 5, "factMethodCount": 75, "metaTotal": 75,
    }), encoding="utf-8")
    (chunk / "results" / "benchmark.json").write_text(json.dumps({
        "methodCount": 5, "results": [], "summary": {"meanDurationMs": 1.0},
    }), encoding="utf-8")

    ctx = ChunkContext(slug="global-ns", assembly="System.Sample",
                       chunk_dir=chunk, foundation_dir=foundation)
    sr = run_aggregate(ctx, {})

    # Not a correctness failure: run stays passed, and the gap is surfaced.
    assert sr.status == "passed"
    assert "meta-mismatch" not in sr.summary

    dashboard = json.loads(
        (foundation / "_dll" / "reports" / "latest" / "dashboard.json")
        .read_text(encoding="utf-8"))
    assert dashboard["summary"]["chunksWithCoverageGap"] == 1
    assert dashboard["summary"]["chunksWithMetaMismatch"] == 0









