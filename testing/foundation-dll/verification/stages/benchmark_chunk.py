"""Simplified benchmark stage for chunk-based pipeline.

Runs entry.exe --benchmark-all N from the chunk's native directory.
Writes both chunk-local benchmark.json and unified-format records.jsonl.

Statistical QC (M1): includes warmup iterations, multi-round sampling,
per-method mean/stddev/CV, and IQR-based outlier removal.
"""
from __future__ import annotations

import json
import math
import os
import subprocess
import time
from collections.abc import Sequence
from datetime import datetime, timezone
from pathlib import Path
from typing import Any

from verification.orchestration.context import ChunkContext, StageResult

_ITERATIONS = 1000  # default iterations per method
_WARMUP_ROUNDS = 2  # warmup rounds before sampling (discarded)
_SAMPLE_ROUNDS = 5  # sampling rounds for statistical QC


def _mean(values: Sequence[float]) -> float:
    return sum(values) / len(values)


def _stddev(values: Sequence[float], mean_val: float) -> float:
    variance = sum((v - mean_val) ** 2 for v in values) / len(values)
    return math.sqrt(variance)


def _quartile(values: list[float], q: float) -> float:
    """Return the q-th quartile of a sorted list."""
    n = len(values)
    idx = q * (n - 1)
    lo = int(idx)
    hi = lo + 1
    frac = idx - lo
    if hi >= n:
        return values[lo]
    return values[lo] * (1 - frac) + values[hi] * frac


def _remove_outliers(values: list[float]) -> tuple[list[float], int]:
    """Remove outliers using IQR method (robust for small sample sizes).

    Returns (cleaned_values, outlier_count).
    Falls back to ±3σ when sample count >= 15.
    """
    if len(values) < 4:
        return values, 0

    sorted_vals = sorted(values)

    if len(values) >= 15:
        mean_val = _mean(values)
        sd = _stddev(values, mean_val)
        if sd == 0:
            return values, 0
        cleaned = [v for v in values if abs(v - mean_val) <= 3.0 * sd]
    else:
        q1 = _quartile(sorted_vals, 0.25)
        q3 = _quartile(sorted_vals, 0.75)
        iqr = q3 - q1
        lo = q1 - 1.5 * iqr
        hi = q3 + 1.5 * iqr
        cleaned = [v for v in values if lo <= v <= hi]

    outlier_count = len(values) - len(cleaned)
    return cleaned if cleaned else values, outlier_count


def _compute_per_method_stats(
    elapsed_samples: list[float],
    ops_samples: list[float],
    alloc_samples: list[float],
) -> dict:
    """Compute statistical summary across sample rounds for a single method."""
    if not elapsed_samples:
        return {}

    mean_elapsed = _mean(elapsed_samples)
    sd_elapsed = _stddev(elapsed_samples, mean_elapsed)
    cv_elapsed = sd_elapsed / mean_elapsed if mean_elapsed > 0 else 0.0

    # Remove outliers and recompute
    cleaned, outlier_count = _remove_outliers(elapsed_samples)
    cleaned_mean = _mean(cleaned) if cleaned else mean_elapsed
    cleaned_sd = _stddev(cleaned, cleaned_mean) if len(cleaned) > 1 else 0.0

    return {
        "meanDurationMs": round(cleaned_mean, 4),
        "stddevMs": round(cleaned_sd, 4),
        "cv": round(cv_elapsed, 6),
        "meanOpsPerSecond": round(_mean(ops_samples), 2) if ops_samples else 0.0,
        "meanAllocatedBytes": round(_mean(alloc_samples), 1) if alloc_samples else 0,
        "sampleCount": len(elapsed_samples),
        "outlierCount": outlier_count,
        "cleanedSampleCount": len(cleaned),
        "minDurationMs": round(min(elapsed_samples), 4),
        "maxDurationMs": round(max(elapsed_samples), 4),
    }


def _compute_summary(stats: list[dict]) -> dict:
    """Compute aggregate summary across all methods.

    Each entry in `stats` is the per-method statistical summary dict
    produced by _compute_per_method_stats.
    """
    if not stats:
        return {}

    elapsed_means = [s.get("meanDurationMs", 0) for s in stats]
    ops_means = [s.get("meanOpsPerSecond", 0) for s in stats if s.get("meanOpsPerSecond", 0) > 0]
    alloc_means = [s.get("meanAllocatedBytes", 0) for s in stats]
    sample_sizes = [s.get("cleanedSampleCount", 0) for s in stats]
    total_outliers = sum(s.get("outlierCount", 0) for s in stats)

    return {
        "methodCount": len(elapsed_means),
        "meanDurationMs": sum(elapsed_means) / len(elapsed_means),
        "minDurationMs": min(elapsed_means),
        "maxDurationMs": max(elapsed_means),
        "totalDurationMs": sum(elapsed_means),
        "meanOpsPerSecond": sum(ops_means) / len(ops_means) if ops_means else 0.0,
        "totalAllocatedBytes": sum(alloc_means),
        "elapsedMilliseconds": sum(elapsed_means),
        "meanSampleCount": round(_mean(sample_sizes), 1) if sample_sizes else 0,
        "totalOutliers": total_outliers,
        "warmupRounds": _WARMUP_ROUNDS,
        "sampleRounds": _SAMPLE_ROUNDS,
    }


def _strip_log_lines(text: str) -> str:
    """Remove log lines (e.g. [2026-06-03T...][LEVEL] ...) from stdout.

    The native runtime may interleave CHAOS_IL2CPP_LOG_DEBUG/INFO lines
    with benchmark JSON output. Stripping them lets the JSON parser work.
    """
    import re
    # Match [datetime][LEVEL] anchor anywhere on a line (not just start-of-line),
    # then consume the rest of the line as the log message.
    log_pattern = re.compile(r"\[\d{4}-\d{2}-\d{2}T\d{2}:\d{2}:\d{2}\]\[[A-Za-z]+\].*$", re.MULTILINE)
    return log_pattern.sub("", text).strip()


def _parse_benchmark_output(stdout: str) -> list[dict]:
    """Parse JSON benchmark output from entry.exe stdout."""
    # Strip interleaved log lines first
    clean = _strip_log_lines(stdout)
    if not clean:
        return []
    # entry.exe may crash after the last method, truncating `]}`.
    # Try normal parse first, then repair if needed.
    for attempt in (clean, clean.rstrip() + "]}", clean.rstrip() + "\n]}"):
        try:
            parsed = json.loads(attempt)
            break
        except json.JSONDecodeError:
            continue
    else:
        return []
    return parsed.get("benchmarkAll", [])


def _run_entry_once(exe_path: Path, iterations: int, timeout: int) -> subprocess.CompletedProcess | None:
    """Run entry.exe --benchmark-all once, returning the CompletedProcess or None on failure."""
    env = os.environ.copy()
    env["CHAOS_IL2CPP_LOG_LEVEL"] = "0"  # suppress debug logs in benchmark output
    try:
        return subprocess.run(
            [str(exe_path), "--benchmark-all", str(iterations)],
            capture_output=True, text=True, timeout=timeout,
            env=env, errors="replace",
        )
    except subprocess.TimeoutExpired:
        return None


def _write_records_jsonl(
    stats: list[dict],
    summary: dict,
    ctx: ChunkContext,
    iterations: int,
):
    """Append unified-format records.jsonl alongside the chunk-local benchmark.json."""

    # M1 fields: attach statistical QC metadata to the per-method records
    records_path = ctx.results_dir / "records.jsonl"
    now = datetime.now(timezone.utc).strftime("%Y-%m-%dT%H:%M:%SZ")

    record = {
        "runId": f"{ctx.assembly}-{ctx.slug}-benchmark-{int(time.time())}",
        "subject": f"{ctx.assembly}/{ctx.slug}",
        "mode": "native",
        "platform": "windows-x64",
        "device": {"id": "chunk-pipeline", "name": "chunk-pipeline"},
        "recordedAt": now,
        "gitCommit": "unknown",
        "metrics": summary,
    }

    with open(records_path, "a", encoding="utf-8") as f:
        f.write(json.dumps(record, ensure_ascii=False, separators=(",", ":")) + "\n")
        for i, s in enumerate(stats):
            method_record = dict(record)
            method_record["runId"] = f"{record['runId']}-method-{i}"
            method_record["metrics"] = {
                "meanDurationMs": s.get("meanDurationMs", 0),
                "stddevMs": s.get("stddevMs", 0),
                "cv": s.get("cv", 0),
                "meanOpsPerSecond": s.get("meanOpsPerSecond", 0),
                "meanAllocatedBytes": s.get("meanAllocatedBytes", 0),
                "sampleCount": s.get("cleanedSampleCount", s.get("sampleCount", 0)),
                "outlierCount": s.get("outlierCount", 0),
            }
            method_record["benchmarkCase"] = {
                "entryIndex": i,
                "stableId": f"method-{i}",
                "alias": f"method-{i}",
            }
            f.write(json.dumps(method_record, ensure_ascii=False, separators=(",", ":")) + "\n")


def run_benchmark_chunk(ctx: ChunkContext, stages: dict[str, StageResult]) -> StageResult:
    """Benchmark stage: run chunk's entry.exe --benchmark-all N with statistical QC."""
    start = time.perf_counter()

    exe_path = ctx.entry_exe_path
    if not exe_path.exists():
        return StageResult(
            stage="benchmark", status="skipped",
            summary=f"entry.exe not found, skipping benchmark",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    iterations = _ITERATIONS
    timeout = ctx.stage_timeout_seconds or 300
    warmup = _WARMUP_ROUNDS
    samples = _SAMPLE_ROUNDS

    print(f"  [benchmark] {exe_path} --benchmark-all {iterations}")
    print(f"  [benchmark] warmup={warmup}, samples={samples} (statistical QC, M1)")

    # Phase 1: Warmup — run without collecting metrics
    for w in range(warmup):
        print(f"  [benchmark] warmup round {w + 1}/{warmup}...")
        result = _run_entry_once(exe_path, iterations, timeout)
        if result is None:
            return StageResult(
                stage="benchmark", status="error",
                summary=f"benchmark timed out during warmup round {w + 1}",
                duration_ms=int((time.perf_counter() - start) * 1000),
            )

    # Phase 2: Sampling — collect metrics across multiple rounds
    all_rounds: list[list[dict]] = []
    for s in range(samples):
        print(f"  [benchmark] sample round {s + 1}/{samples}...")
        result = _run_entry_once(exe_path, iterations, timeout)
        if result is None:
            return StageResult(
                stage="benchmark", status="error",
                summary=f"benchmark timed out during sample round {s + 1}",
                duration_ms=int((time.perf_counter() - start) * 1000),
            )

        parsed = _parse_benchmark_output(result.stdout or "")
        round_results: list[dict] = [dict(r) if isinstance(r, dict) else {} for r in parsed]
        all_rounds.append(round_results)

    if not all_rounds or not all_rounds[0]:
        ctx.results_dir.mkdir(parents=True, exist_ok=True)
        result_path = ctx.results_dir / "benchmark.json"
        result_path.write_text(
            json.dumps({"exitCode": -1, "iterations": iterations, "methodCount": 0,
                        "results": [], "summary": {}, "stderr": "no data",
                        "warmupRounds": warmup, "sampleRounds": samples},
                       indent=2),
            encoding="utf-8",
        )
        duration_ms = int((time.perf_counter() - start) * 1000)
        return StageResult(
            stage="benchmark", status="error",
            summary="no benchmark data returned",
            duration_ms=duration_ms,
        )

    # Phase 3: Per-method statistical computation
    method_count = len(all_rounds[0])
    per_method_stats: list[dict] = []

    for method_idx in range(method_count):
        # Collect elapsed, ops, alloc across all sample rounds
        elapsed_samples: list[float] = []
        ops_samples: list[float] = []
        alloc_samples: list[float] = []

        for round_data in all_rounds:
            if method_idx < len(round_data):
                m = round_data[method_idx]
                elapsed = m.get("elapsedMilliseconds")
                if isinstance(elapsed, (int, float)) and elapsed >= 0:
                    elapsed_samples.append(float(elapsed))
                ops = m.get("opsPerSecond")
                if isinstance(ops, (int, float)) and ops > 0:
                    ops_samples.append(float(ops))
                alloc = m.get("allocatedBytes")
                if isinstance(alloc, (int, float)) and alloc >= 0:
                    alloc_samples.append(float(alloc))

        method_stats = _compute_per_method_stats(elapsed_samples, ops_samples, alloc_samples)
        per_method_stats.append(method_stats)

    # Phase 4: Aggregate summary
    summary = _compute_summary(per_method_stats)

    # Phase 5: Write output files
    ctx.results_dir.mkdir(parents=True, exist_ok=True)
    result_path = ctx.results_dir / "benchmark.json"

    # Build flat results array for backward compat (mean values)
    flat_results: list[dict] = []
    for method_idx, s in enumerate(per_method_stats):
        flat_results.append({
            "entryIndex": method_idx,
            "elapsedMilliseconds": s.get("meanDurationMs", 0),
            "opsPerSecond": s.get("meanOpsPerSecond", 0),
            "allocatedBytes": s.get("meanAllocatedBytes", 0),
            "stddevMs": s.get("stddevMs", 0),
            "cv": s.get("cv", 0),
            "sampleCount": s.get("cleanedSampleCount", s.get("sampleCount", 0)),
            "outlierCount": s.get("outlierCount", 0),
        })

    result_data = {
        "exitCode": 0,
        "technology": "chaos-aot",
        "iterations": iterations,
        "methodCount": method_count,
        "results": flat_results,
        "summary": summary,
        "perMethodStats": per_method_stats,
        "warmupRounds": warmup,
        "sampleRounds": samples,
        "stderr": "",
    }
    result_path.write_text(json.dumps(result_data, indent=2), encoding="utf-8")

    # Write unified-format records.jsonl
    _write_records_jsonl(per_method_stats, summary, ctx, iterations)

    status = "passed" if method_count > 0 else "error"
    duration_ms = int((time.perf_counter() - start) * 1000)
    print(f"  [benchmark] {status}: {method_count} methods "
          f"({samples} samples, {warmup} warmup, {duration_ms}ms)")

    return StageResult(
        stage="benchmark", status=status,
        summary=f"{status}: {method_count} methods, {samples} samples, "
                f"{summary.get('totalOutliers', 0)} outliers removed",
        details=result_data,
        duration_ms=duration_ms,
    )
