"""Benchmark report stage — cross-technology comparison with .NET 8 baseline.

Reads benchmark-history.jsonl (produced by managed_benchmark and benchmark_chunk),
aligns records by methodSubjectId across 4 technologies (net8-jit, net10-jit,
chaos-aot, chaos-jit), computes % faster vs .NET 8, and writes:

  - {chunk}/results/comparison.json          — per-chunk detail
  - _dll/reports/latest/comparison-summary.json — aggregate across all chunks
  - _dll/reports/history/comparison-YYYY-MM-DD.json — snapshot for cross-run diff
"""

from __future__ import annotations

import json
import math
import time
from collections import defaultdict
from datetime import date
from pathlib import Path
from typing import Any

from verification.orchestration.context import ChunkContext, StageResult

_RESULTS_BASE = Path(__file__).resolve().parent.parent / "results" / "foundation-dll"


def _median(sorted_vals: list[float]) -> float:
    n = len(sorted_vals)
    if n == 0:
        return 0.0
    mid = n // 2
    return sorted_vals[mid] if n % 2 else (sorted_vals[mid - 1] + sorted_vals[mid]) / 2


def _percentile(sorted_vals: list[float], p: float) -> float:
    n = len(sorted_vals)
    if n == 0:
        return 0.0
    idx = p / 100.0 * (n - 1)
    lo = int(idx)
    hi = lo + 1
    frac = idx - lo
    if hi >= n:
        return sorted_vals[lo]
    return sorted_vals[lo] * (1 - frac) + sorted_vals[hi] * frac


def _compute_aggregate_stats(values: list[float]) -> dict[str, float] | None:
    """Compute mean/median/min/max/p25/p75 for a list of values."""
    if not values:
        return None
    sorted_vals = sorted(values)
    mean_val = sum(values) / len(values)
    return {
        "mean": round(mean_val, 2),
        "median": round(_median(sorted_vals), 2),
        "min": round(sorted_vals[0], 2),
        "max": round(sorted_vals[-1], 2),
        "p25": round(_percentile(sorted_vals, 25), 2),
        "p75": round(_percentile(sorted_vals, 75), 2),
    }


def _compute_pct(net8_ms: float, tech_ms: float | None) -> float | None:
    """Compute (net8_ms - tech_ms) / net8_ms * 100. Positive = faster than net8."""
    if net8_ms is None or net8_ms <= 0 or tech_ms is None or tech_ms < 0:
        return None
    return round((net8_ms - tech_ms) / net8_ms * 100, 2)


def _read_jsonl_technology_map(jsonl_path: Path) -> dict[str, dict[str, Any]]:
    """Read JSONL and build {methodSubjectId: {technology: latest_record}}.

    For each methodSubjectId, keeps only the latest timestamp per technology.
    Records with status='error' or elapsedMilliseconds < 0 are kept but marked.
    """
    if not jsonl_path.exists():
        return {}

    tech_map: dict[str, dict[str, Any]] = defaultdict(dict)

    with open(jsonl_path, encoding="utf-8") as f:
        for line in f:
            line = line.strip()
            if not line:
                continue
            try:
                rec = json.loads(line)
            except json.JSONDecodeError:
                continue

            msid = rec.get("methodSubjectId", "")
            tech = rec.get("technology", "")
            if not msid or not tech:
                continue

            # Keep the record with the latest timestamp per technology.
            # When timestamps tie (same run, appended JSONL), the later
            # occurrence wins — it came from a more recent append.
            existing = tech_map[msid].get(tech)
            if existing and rec.get("timestamp", "") < existing.get("timestamp", ""):
                continue
            tech_map[msid][tech] = rec

    return dict(tech_map)


def _build_method_comparison(
    tech_map: dict[str, dict[str, Any]],
    slug: str,
) -> tuple[list[dict], dict[str, Any]]:
    """Build per-method comparison records and chunk-level aggregate stats.

    Returns (methods_list, aggregate_dict).
    """
    methods_list: list[dict] = []
    all_chaos_aot_pcts: list[float] = []
    all_chaos_jit_pcts: list[float] = []
    all_net10_pcts: list[float] = []
    total_better_than_net8 = 0
    total_with_net8 = 0

    for msid, techs in tech_map.items():
        net8_rec = techs.get("net8-jit")
        net8_ms = _get_elapsed(net8_rec)
        net8_error = net8_rec is not None and (net8_rec.get("status") == "error" or (net8_ms is not None and net8_ms < 0))
        net8_high_var = _is_high_variance(net8_rec)

        net10_ms = _get_elapsed(techs.get("net10-jit"))
        chaos_aot_ms = _get_elapsed(techs.get("chaos-aot"))
        chaos_jit_ms = _get_elapsed(techs.get("chaos-jit"))

        method_entry: dict[str, Any] = {
            "methodSubjectId": msid,
            "net8Ms": net8_ms,
            "net10Ms": net10_ms,
            "chaosAotMs": chaos_aot_ms,
            "chaosJitMs": chaos_jit_ms,
            "net8Error": net8_error,
            "highVariance": net8_high_var,
        }

        if net8_ms and net8_ms > 0 and not net8_error:
            method_entry["net10VsNet8Pct"] = _compute_pct(net8_ms, net10_ms)
            method_entry["chaosAotVsNet8Pct"] = _compute_pct(net8_ms, chaos_aot_ms)
            method_entry["chaosJitVsNet8Pct"] = _compute_pct(net8_ms, chaos_jit_ms)
            method_entry["status"] = "completed"
            total_with_net8 += 1

            pct = method_entry.get("chaosAotVsNet8Pct")
            if pct is not None:
                all_chaos_aot_pcts.append(pct)
                if pct > 0:
                    total_better_than_net8 += 1

            pct_jit = method_entry.get("chaosJitVsNet8Pct")
            if pct_jit is not None:
                all_chaos_jit_pcts.append(pct_jit)

            pct_net10 = method_entry.get("net10VsNet8Pct")
            if pct_net10 is not None:
                all_net10_pcts.append(pct_net10)
        else:
            method_entry.update({
                "net10VsNet8Pct": None,
                "chaosAotVsNet8Pct": None,
                "chaosJitVsNet8Pct": None,
                "status": "missing_net8" if net8_rec is None else "net8_error",
            })

        methods_list.append(method_entry)

    # Build aggregate stats
    aggregate: dict[str, Any] = {
        "methodCount": len(methods_list),
        "methodsWithNet8": total_with_net8,
        "methodsBetterThanNet8": total_better_than_net8,
    }
    if all_chaos_aot_pcts:
        aggregate["chaosAotVsNet8Pct"] = _compute_aggregate_stats(all_chaos_aot_pcts)
        aggregate["highValueMethods_betterThanNet8"] = round(
            total_better_than_net8 / len(all_chaos_aot_pcts) * 100, 1
        ) if all_chaos_aot_pcts else 0
    if all_chaos_jit_pcts:
        aggregate["chaosJitVsNet8Pct"] = _compute_aggregate_stats(all_chaos_jit_pcts)
    if all_net10_pcts:
        aggregate["net10VsNet8Pct"] = _compute_aggregate_stats(all_net10_pcts)

    return methods_list, aggregate


def _get_elapsed(rec: dict | None) -> float | None:
    """Extract elapsedMilliseconds from a record."""
    if rec is None:
        return None
    metrics = rec.get("metrics") or {}
    val = metrics.get("elapsedMilliseconds")
    if val is None:
        return None
    return float(val)


def _is_high_variance(rec: dict | None) -> bool:
    """Check if the record has high variance (CV > 20%)."""
    if rec is None:
        return False
    return rec.get("highVariance", False)


def run_benchmark_report(ctx: ChunkContext, stages: dict[str, StageResult]) -> StageResult:
    """Benchmark report stage: cross-technology comparison with .NET 8 baseline."""
    start = time.perf_counter()
    foundation_dir = ctx.foundation_dir
    assembly = ctx.assembly
    chunks_dir = foundation_dir / "chunks"
    reports_dir = foundation_dir / "_dll" / "reports"
    latest_dir = reports_dir / "latest"
    history_dir = reports_dir / "history"

    if not chunks_dir.exists():
        return StageResult(
            stage="benchmark_report", status="error",
            summary=f"chunks directory not found: {chunks_dir}",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    # ── Discover chunks ──
    chunk_slugs = sorted(d.name for d in chunks_dir.iterdir() if d.is_dir())
    print(f"  [benchmark-report] {len(chunk_slugs)} chunks found")

    # ── Process each chunk ──
    all_chunk_comparisons: list[dict] = []
    all_chaos_aot_pcts: list[float] = []
    all_chaos_jit_pcts: list[float] = []
    all_net10_pcts: list[float] = []
    total_methods = 0
    total_with_net8 = 0
    total_better_than_net8 = 0

    for slug in chunk_slugs:
        jsonl_path = _RESULTS_BASE / assembly / slug / "perf" / "benchmark-history.jsonl"
        chunk_dir = chunks_dir / slug
        results_dir = chunk_dir / "results"

        tech_map = _read_jsonl_technology_map(jsonl_path)
        if not tech_map:
            print(f"  [benchmark-report] {slug}: no benchmark data (JSONL not found or empty)")
            continue

        methods_list, aggregate = _build_method_comparison(tech_map, slug)
        if not methods_list:
            continue

        # Per-chunk comparison output
        chunk_comparison = {
            "slug": slug,
            "methodCount": aggregate["methodCount"],
            "methodsWithNet8": aggregate["methodsWithNet8"],
            "aggregate": {
                k: v for k, v in aggregate.items() if k not in ("methodCount", "methodsWithNet8")
            },
            "methods": methods_list,
        }
        all_chunk_comparisons.append(chunk_comparison)

        # Write per-chunk comparison.json
        if results_dir.exists():
            comp_path = results_dir / "comparison.json"
            comp_path.write_text(json.dumps({
                "slug": slug,
                "methods": methods_list,
                "aggregate": aggregate,
            }, indent=2), encoding="utf-8")

        # Accumulate cross-chunk aggregate
        total_methods += aggregate["methodCount"]
        total_with_net8 += aggregate["methodsWithNet8"]
        total_better_than_net8 += aggregate.get("methodsBetterThanNet8", 0)
        if aggregate["methodsWithNet8"] > 0:
            chaos_pct = aggregate.get("chaosAotVsNet8Pct")
            if isinstance(chaos_pct, dict) and chaos_pct.get("mean") is not None:
                all_chaos_aot_pcts.append(chaos_pct["mean"])
            chaos_jit_pct = aggregate.get("chaosJitVsNet8Pct")
            if isinstance(chaos_jit_pct, dict) and chaos_jit_pct.get("mean") is not None:
                all_chaos_jit_pcts.append(chaos_jit_pct["mean"])
            net10_pct = aggregate.get("net10VsNet8Pct")
            if isinstance(net10_pct, dict) and net10_pct.get("mean") is not None:
                all_net10_pcts.append(net10_pct["mean"])

        print(f"  [benchmark-report] {slug}: {aggregate['methodCount']} methods, "
              f"{aggregate['methodsWithNet8']} with net8 baseline")

    if not all_chunk_comparisons:
        return StageResult(
            stage="benchmark_report", status="skipped",
            summary="No benchmark data found for any chunk",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    # ── Write aggregate summary ──
    cross_chunk_aggregate: dict[str, Any] = {}
    if all_chaos_aot_pcts:
        cross_chunk_aggregate["chaosAotVsNet8Pct"] = _compute_aggregate_stats(all_chaos_aot_pcts)
    if all_chaos_jit_pcts:
        cross_chunk_aggregate["chaosJitVsNet8Pct"] = _compute_aggregate_stats(all_chaos_jit_pcts)
    if all_net10_pcts:
        cross_chunk_aggregate["net10VsNet8Pct"] = _compute_aggregate_stats(all_net10_pcts)
    cross_chunk_aggregate["highValueMethods_betterThanNet8"] = round(
        total_better_than_net8 / max(total_with_net8, 1) * 100, 1
    ) if total_with_net8 else 0

    comparison_summary = {
        "assemblyName": assembly,
        "timestamp": time.strftime("%Y-%m-%dT%H:%M:%S"),
        "totalMethods": total_methods,
        "methodsWithNet8": total_with_net8,
        "aggregate": cross_chunk_aggregate,
        "perChunk": all_chunk_comparisons,
    }

    latest_dir.mkdir(parents=True, exist_ok=True)
    history_dir.mkdir(parents=True, exist_ok=True)

    (latest_dir / "comparison-summary.json").write_text(
        json.dumps(comparison_summary, indent=2), encoding="utf-8")

    # History snapshot
    today = date.today().isoformat()
    history_entry = {
        "date": today,
        "assemblyName": assembly,
        "aggregate": cross_chunk_aggregate,
        "perChunk": [
            {k: v for k, v in c.items() if k != "methods"}
            for c in all_chunk_comparisons
        ],
    }
    (history_dir / f"comparison-{today}.json").write_text(
        json.dumps(history_entry, indent=2), encoding="utf-8")

    duration_ms = int((time.perf_counter() - start) * 1000)
    chaos_mean = cross_chunk_aggregate.get("chaosAotVsNet8Pct", {}).get("mean", "N/A")
    net10_mean = cross_chunk_aggregate.get("net10VsNet8Pct", {}).get("mean", "N/A")
    print(f"  [benchmark-report] Comparison: {total_methods} methods, "
          f"{total_with_net8} with net8 baseline")
    print(f"  [benchmark-report] Chaos AOT vs .NET 8: mean {chaos_mean}% faster")
    print(f"  [benchmark-report] .NET 10 vs .NET 8:   mean {net10_mean}% faster")
    print(f"  [benchmark-report] Reports written to {latest_dir}")
    print(f"  [benchmark-report] Done ({duration_ms}ms)")

    return StageResult(
        stage="benchmark_report", status="passed",
        summary=f"compared {total_with_net8}/{total_methods} methods vs net8, "
                f"chaosAot mean {chaos_mean}% faster",
        details=comparison_summary,
        duration_ms=duration_ms,
    )
