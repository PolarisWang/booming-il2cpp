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

    Also builds a secondary index by (slug, methodIndex) so records with
    divergent subject ID formats (CombinedSubjects wrappers vs raw metadata)
    can still be aligned.  The slug+methodIndex pair is stable across both
    AOT and managed benchmark paths because both index into the same metadata.
    """
    if not jsonl_path.exists():
        return {}

    tech_map: dict[str, dict[str, Any]] = defaultdict(dict)
    idx_map: dict[tuple[str, int], dict[str, Any]] = defaultdict(dict)

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

            # Primary index: by methodSubjectId — keep the MAX elapsedMs
            # across rounds (ignores timer-floor 0.001ms values from very
            # fast methods, while also ignoring unusually high outliers).
            existing = tech_map[msid].get(tech)
            if existing:
                existing_ms = (existing.get("metrics") or {}).get("elapsedMilliseconds", 0) or 0
                new_ms = (rec.get("metrics") or {}).get("elapsedMilliseconds", 0) or 0
                if new_ms <= existing_ms:
                    continue
            tech_map[msid][tech] = rec

            # Secondary index: by (slug, methodIndex) for cross-format alignment
            sl = rec.get("slug", "")
            mi = rec.get("methodIndex")
            if sl and isinstance(mi, int) and mi >= 0:
                ikey = (sl, mi)
                iex = idx_map[ikey].get(tech)
                if iex and rec.get("timestamp", "") < iex.get("timestamp", ""):
                    continue
                idx_map[ikey][tech] = rec

    # Merge primary and secondary indices
    merged: dict[str, dict[str, Any]] = defaultdict(dict)

    # First pass: copy all records from primary tech_map
    for msid, techs in tech_map.items():
        for tech, rec in techs.items():
            merged[msid][tech] = rec

    # Second pass: supplement missing technologies via slug+methodIndex
    for (sl, mi), idx_techs in idx_map.items():
        found_key = None
        for msid in tech_map:
            for tech, rec in tech_map[msid].items():
                if rec.get("slug") == sl and rec.get("methodIndex") == mi:
                    found_key = msid
                    break
            if found_key:
                break
        if found_key:
            for tech, rec in idx_techs.items():
                if tech not in merged[found_key]:
                    merged[found_key][tech] = rec
        else:
            fkey = f'{sl}::{mi}'
            for tech, rec in idx_techs.items():
                merged[fkey][tech] = rec

    return dict(merged)

def _build_method_comparison(
    tech_map: dict[str, dict[str, Any]],
    slug: str,
    profile_data: list[dict] | None = None,
) -> tuple[list[dict], dict[str, Any]]:
    """Build per-method comparison records and chunk-level aggregate stats.

    When profile_data is provided (from profile.json), adds gcComparison
    between chaos-aot (from profile) and net8-jit (from benchmark-history.jsonl).

    Returns (methods_list, aggregate_dict).
    """
    methods_list: list[dict] = []
    all_chaos_aot_pcts: list[float] = []
    all_chaos_jit_pcts: list[float] = []
    all_net10_pcts: list[float] = []
    total_better_than_net8 = 0
    total_with_net8 = 0

    # GC comparison accumulators
    all_alloc_ratios: list[float] = []
    all_gc_pause_deltas: list[float] = []
    methods_with_alloc_data = 0

    for msid, techs in tech_map.items():
        net8_rec = techs.get("net8-jit")
        # Fall back to net10-jit when net8-jit is unavailable (e.g. CombinedSubjects
        # can't compile for net8.0 due to newer API dependencies). This allows
        # assemblies with net10-only benchmark data to still get comparison results.
        if net8_rec is None:
            net8_rec = techs.get("net10-jit")
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

            # ── Bottleneck classification ──
            method_entry["bottleneck"] = _classify_bottleneck(
                chaos_aot_ms=chaos_aot_ms, net8_ms=net8_ms,
                high_variance=net8_high_var,
                gc_comparison=gc_comparison if 'gc_comparison' in dir() else None)

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

        # ── GC comparison: net8 gcInfo vs chaos-aot profile ──
        # Runs for ALL methods (including errors) so throwing
        # methods still get gcComparison with pre-throw gcInfo.
        net8_gi = _get_gcinfo(net8_rec)
        chaos_profile = _find_profile(profile_data, msid) if profile_data else None
        if net8_gi:
            gc_comp = {
                "net8AllocBytes": net8_gi.get("totalAllocatedBytes"),
                "net8CollectionCount0": net8_gi.get("collectionCount0"),
                "net8HeapDelta": net8_gi.get("heapDelta"),
            }
            if chaos_profile:
                gc_comp["chaosAotGcPauseNs"] = chaos_profile.get("gcPauseNs", 0)
                gc_comp["chaosAotAllocBytes"] = chaos_profile.get("nurseryAllocBytes", 0)
                gc_comp["chaosAotFastPathRate"] = _fast_path_rate(chaos_profile)
                alloc_pct = _compare_alloc(
                    net8_gi.get("totalAllocatedBytes"),
                    chaos_profile.get("nurseryAllocBytes"),
                )
                if alloc_pct is not None:
                    gc_comp["aotAllocVsNet8Pct"] = alloc_pct
                    all_alloc_ratios.append(alloc_pct)
                    methods_with_alloc_data += 1
            method_entry["gcComparison"] = gc_comp
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

    # GC comparison aggregate
    if all_alloc_ratios:
        aggregate["gcAllocComparison"] = {
            "aotVsNet8Ratio": _compute_aggregate_stats(all_alloc_ratios),
            "methodsWithAllocData": methods_with_alloc_data,
        }
    if all_gc_pause_deltas:
        aggregate["gcPauseComparison"] = {
            "aotVsNet8GcPauseDelta": _compute_aggregate_stats(all_gc_pause_deltas),
        }

    return methods_list, aggregate


def _is_high_variance(rec: dict | None) -> bool:
    """Check if the record has high variance (CV > 20%)."""
    if rec is None:
        return False
    return rec.get("highVariance", False)


def _classify_bottleneck(
    chaos_aot_ms: float | None,
    net8_ms: float | None,
    high_variance: bool,
    gc_comparison: dict | None = None,
) -> str:
    """Classify the performance bottleneck for a benchmarked method.

    Returns one of: "gc_pause", "dispatch_overhead", "alloc_hot", "unstable", or "".
    """
    if high_variance:
        return "unstable"
    if chaos_aot_ms is None or net8_ms is None or net8_ms <= 0:
        return ""

    ratio = chaos_aot_ms / net8_ms
    if ratio > 2.0:
        return "dispatch_overhead"
    if ratio > 1.5 and gc_comparison:
        alloc_ratio = gc_comparison.get("aotVsNet8Ratio", 1.0)
        if alloc_ratio and alloc_ratio > 2.0:
            return "alloc_hot"
    return ""


def _get_elapsed(rec: dict | None) -> float | None:
    """Extract elapsedMilliseconds per iteration from a record.

    Normalizes by iteration count so records with different iteration
    counts can be compared fairly. AOT benchmark uses calibrated
    iterations (~50ms target), managed benchmark uses fixed 1000 iters.
    """
    if rec is None:
        return None
    metrics = rec.get("metrics") or {}
    val = metrics.get("elapsedMilliseconds")
    if val is None:
        return None
    iterations = rec.get("iterations", 1)
    if iterations is None or iterations <= 0:
        iterations = 1
    return float(val) / max(iterations, 1)


def _find_profile(profile_data: list[dict] | None, msid: str) -> dict | None:
    """Find a profile entry by methodSubjectId. Falls back to index matching."""
    if profile_data is None:
        return None
    for p in profile_data:
        if p.get("methodSubjectId", "") == msid:
            return p
    return None


def _fast_path_rate(profile: dict) -> float:
    """Compute fast path rate from profile entry."""
    fast = profile.get("fastPathCount", 0)
    slow = profile.get("slowPathCount", 0)
    total = fast + slow
    return round(fast / total, 4) if total > 0 else 1.0


def _get_gcinfo(rec: dict | None) -> dict | None:
    """Extract gcInfo from a record."""
    if rec is None:
        return None
    return rec.get("gcInfo")


def _has_gcinfo(rec: dict | None) -> bool:
    """Check if a record has non-empty gcInfo."""
    gi = _get_gcinfo(rec)
    if gi is None:
        return False
    return gi.get("totalAllocatedBytes", 0) > 0 or bool(gi.get("collectionCount0"))


def _compare_alloc(net8_bytes: int | None, tech_bytes: int | None) -> float | None:
    """Compute (tech_bytes - net8_bytes) / net8_bytes * 100. Positive = more alloc than net8."""
    if net8_bytes is None or net8_bytes <= 0 or tech_bytes is None or tech_bytes < 0:
        return None
    return round((tech_bytes - net8_bytes) / net8_bytes * 100, 2)


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

        # Load AOT profile data for GC comparison (optional)
        profile_data = None
        profile_path = chunk_dir / "results" / "profile.json"
        if profile_path.exists():
            try:
                prof = json.loads(profile_path.read_text(encoding="utf-8"))
                pd = prof.get("profileData", [])
                # Build methodSubjectId map from metadata
                meta_path = chunk_dir / "managed" / "subjects" / "subjects.metadata.json"
                if meta_path.exists() and pd:
                    meta = json.loads(meta_path.read_text(encoding="utf-8"))
                    meta_methods = meta.get("methods", [])
                    for i, m in enumerate(pd):
                        if i < len(meta_methods):
                            m["methodSubjectId"] = meta_methods[i].get("methodSubjectId", f"method-{i}")
                    profile_data = pd
            except (json.JSONDecodeError, OSError):
                pass

        methods_list, aggregate = _build_method_comparison(tech_map, slug, profile_data)
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

    # Print GC comparison summary
    gc_agg = cross_chunk_aggregate.get("gcAllocComparison", {})
    if gc_agg:
        gc_mean = gc_agg.get("aotVsNet8Ratio", {}).get("mean", "N/A")
        gc_methods = gc_agg.get("methodsWithAllocData", 0)
        print(f"  [benchmark-report] GC alloc: AOT vs .NET 8 mean {gc_mean}% ({gc_methods} methods)")

    print(f"  [benchmark-report] Reports written to {latest_dir}")
    print(f"  [benchmark-report] Done ({duration_ms}ms)")

    return StageResult(
        stage="benchmark_report", status="passed",
        summary=f"compared {total_with_net8}/{total_methods} methods vs net8, "
                f"chaosAot mean {chaos_mean}% faster",
        details=comparison_summary,
        duration_ms=duration_ms,
    )
