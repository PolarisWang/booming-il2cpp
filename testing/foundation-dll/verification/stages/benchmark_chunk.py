"""Simplified benchmark stage for chunk-based pipeline.

Runs entry.exe --benchmark-all N from the chunk's native directory.
Writes both chunk-local benchmark.json and unified-format records.jsonl.

Format: line-by-line JSON (each method one complete JSON line, summary at end).
Adaptive iteration: probe with 10 iterations, scale to ~50ms total per method.
Adaptive sampling: 3-10 rounds, early stop when median CV < 5%.
"""
from __future__ import annotations

import json
import math
import os
import re
import subprocess
import time
from collections.abc import Sequence
from datetime import datetime, timezone
from pathlib import Path
from typing import Any

from verification.orchestration.context import ChunkContext, StageResult


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
    }


def _parse_benchmark_lines(stdout: str) -> tuple[list[dict], dict]:
    """Parse line-by-line JSON benchmark output from entry.exe stdout.

    Each method produces one complete JSON line ending with \\n.
    The last line is {"summary":{...}}.
    Stray lines that aren't valid JSON are silently skipped.
    """
    lines = stdout.strip().split('\n')
    results = []
    summary = {}
    for line in lines:
        line = line.strip()
        if not line:
            continue
        try:
            obj = json.loads(line)
        except json.JSONDecodeError:
            continue
        if 'summary' in obj:
            summary = obj['summary']
        else:
            results.append(obj)
    return results, summary


def _calibrate_iterations(exe_path: Path, timeout: int, entry_count: int = 0) -> int:
    """Probe-run with 10 iterations, scale to target ~50ms total per method.

    For large chunks (>5000 entries), caps iterations at 10000 instead of 50000
    to keep total benchmark time reasonable.
    """
    result = _run_entry_once(exe_path, 10, timeout)
    if result is None or not result.stdout:
        return 1000  # fallback

    data, _ = _parse_benchmark_lines(result.stdout or "")
    if not data:
        return 1000  # fallback

    # Collect positive elapsed times to estimate per-call cost
    elapsed = [
        float(r['elapsedMilliseconds']) for r in data
        if isinstance(r.get('elapsedMilliseconds'), (int, float))
        and r['elapsedMilliseconds'] > 0
    ]
    if not elapsed:
        return 10000  # all very fast, use high default

    # Per-call ms = median elapsed / 10 (probe iterations)
    median_elapsed = sorted(elapsed)[len(elapsed) // 2]
    per_call_ms = median_elapsed / 10.0
    target_ms = 50.0
    iterations = max(100, int(target_ms / max(per_call_ms, 0.001)))

    # Cap: 50000 normally, 10000 for large chunks
    cap = 10000 if entry_count > 5000 else 50000
    return min(iterations, cap)


def _run_entry_once(exe_path: Path, iterations: int, timeout: int) -> subprocess.CompletedProcess | None:
    """Run entry.exe --benchmark-all once, returning the CompletedProcess or None on failure.

    On TimeoutExpired, attempts to recover partial stdout (available since Python 3.8).
    Returns a CompletedProcess with the partial stdout on timeout so the caller
    can still salvage benchmark data from hanging chunks.
    """
    env = os.environ.copy()
    env["CHAOS_IL2CPP_LOG_LEVEL"] = "0"  # suppress debug logs in benchmark output
    cmd = [str(exe_path), "--benchmark-all", str(iterations)]
    try:
        return subprocess.run(
            cmd,
            stdout=subprocess.PIPE, stderr=subprocess.DEVNULL, text=True, timeout=timeout,
            env=env, errors="replace",
        )
    except subprocess.TimeoutExpired as e:
        partial = e.stdout  # partial stdout captured before timeout
        if partial and partial.strip():
            # Return a fake CompletedProcess with partial stdout so the caller
            # can attempt to salvage whatever benchmark data exists.
            return subprocess.CompletedProcess(
                args=e.cmd, returncode=-1,
                stdout=partial, stderr="",
            )
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


def _get_entry_count(exe_path: Path) -> int:
    """Read kSubjectEntryCount from the generated native-aot.generated.cpp."""
    subject_file = exe_path.parent / "subjects" / "native-aot.generated.cpp"
    if not subject_file.exists():
        return 0
    content = subject_file.read_text(encoding="utf-8", errors="replace")
    m = re.search(r"kSubjectEntryCount\s*=\s*(\d+)", content)
    return int(m.group(1)) if m else 0


def run_benchmark_chunk(ctx: ChunkContext, stages: dict[str, StageResult]) -> StageResult:
    """Benchmark stage: run chunk's entry.exe --benchmark-all N with adaptive settings."""
    start = time.perf_counter()

    exe_path = ctx.entry_exe_path
    if not exe_path.exists():
        return StageResult(
            stage="benchmark", status="skipped",
            summary=f"entry.exe not found, skipping benchmark",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    timeout = max(ctx.stage_timeout_seconds or 300, 30)
    entry_count = _get_entry_count(exe_path)

    # Phase 0: Calibrate iteration count via probe run
    print(f"  [benchmark] {exe_path} --benchmark-all (adaptive)")
    iterations = _calibrate_iterations(exe_path, timeout, entry_count)
    print(f"  [benchmark] calibrated iterations={iterations}, entries={entry_count}, timeout={timeout}s")

    # Phase 1: Adaptive sampling rounds (3-10, early stop on CV < 5%)
    max_rounds = 10
    min_rounds = 3
    all_rounds: list[list[dict]] = []

    for s in range(max_rounds):
        print(f"  [benchmark] sampling round {s + 1}/{max_rounds}...")
        result = _run_entry_once(exe_path, iterations, timeout)
        if result is None:
            # If we have partial data from previous rounds, use it
            if all_rounds:
                break
            return StageResult(
                stage="benchmark", status="error",
                summary=f"benchmark timed out during sampling round {s + 1}",
                duration_ms=int((time.perf_counter() - start) * 1000),
            )

        parsed, _ = _parse_benchmark_lines(result.stdout or "")
        round_results: list[dict] = [dict(r) if isinstance(r, dict) else {} for r in parsed]
        all_rounds.append(round_results)

        # Early stop: after reaching min_rounds, check median CV across last 3 rounds
        if s >= min_rounds - 1 and len(all_rounds) >= 3:
            num_methods = len(all_rounds[0])
            cvs = []
            for method_idx in range(num_methods):
                elapsed = []
                for rd in all_rounds[-3:]:
                    if method_idx < len(rd):
                        e = rd[method_idx].get('elapsedMilliseconds', 0)
                        if isinstance(e, (int, float)) and e >= 0:
                            elapsed.append(float(e))
                if len(elapsed) >= 3:
                    mean_e = _mean(elapsed)
                    if mean_e > 0:
                        cvs.append(_stddev(elapsed, mean_e) / mean_e)
            if cvs:
                median_cv = sorted(cvs)[len(cvs) // 2]
                if median_cv < 0.05:
                    print(f"  [benchmark] early stop at round {s + 1} (median CV={median_cv:.4f} < 5%)")
                    break

    if not all_rounds or not all_rounds[0]:
        ctx.results_dir.mkdir(parents=True, exist_ok=True)
        result_path = ctx.results_dir / "benchmark.json"
        result_path.write_text(
            json.dumps({"exitCode": -1, "iterations": iterations, "methodCount": 0,
                        "results": [], "summary": {}, "stderr": "no data",
                        "sampleRounds": len(all_rounds)},
                       indent=2),
            encoding="utf-8",
        )
        duration_ms = int((time.perf_counter() - start) * 1000)
        return StageResult(
            stage="benchmark", status="error",
            summary="no benchmark data returned",
            duration_ms=duration_ms,
        )

    # Phase 2: Per-method statistical computation
    method_count = len(all_rounds[0])
    per_method_stats: list[dict] = []

    for method_idx in range(method_count):
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

    # Phase 3: Aggregate summary
    summary = _compute_summary(per_method_stats)

    # Phase 4: Write output files
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
        "sampleRounds": len(all_rounds),
        "stderr": "",
    }
    result_path.write_text(json.dumps(result_data, indent=2), encoding="utf-8")

    # Write unified-format records.jsonl
    _write_records_jsonl(per_method_stats, summary, ctx, iterations)

    status = "passed" if method_count > 0 else "error"
    duration_ms = int((time.perf_counter() - start) * 1000)
    print(f"  [benchmark] {status}: {method_count} methods "
          f"({len(all_rounds)} samples, {duration_ms}ms, {iterations} iterations)")

    return StageResult(
        stage="benchmark", status=status,
        summary=f"{status}: {method_count} methods, {len(all_rounds)} samples, "
                f"{summary.get('totalOutliers', 0)} outliers removed",
        details=result_data,
        duration_ms=duration_ms,
    )
