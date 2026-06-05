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

_RESULTS_BASE = Path(__file__).resolve().parent.parent / "results" / "foundation-dll"


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

    Each method produces one complete JSON line ending with \n.
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


def _calibrate_iterations(exe_path: Path, timeout: int, entry_count: int = 0,
                          start_idx: int = 0, end_idx: int = 0) -> int:
    """Probe-run with 10 iterations, scale to target ~50ms total per method.

    Pass start_idx/end_idx to use --benchmark-range for calibration;
    otherwise falls back to --benchmark-all.

    For large chunks (>5000 entries), caps iterations at 10000 instead of 50000
    to keep total benchmark time reasonable.
    """
    result = _run_entry_once(exe_path, 10, timeout, start_idx=start_idx, end_idx=end_idx)
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


def _run_entry_once(exe_path: Path, iterations: int, timeout: int,
                     start_idx: int = 0, end_idx: int = 0) -> subprocess.CompletedProcess | None:
    """Run entry.exe benchmark once, returning the CompletedProcess or None on failure.

    Uses --benchmark-range start end iterations when end_idx > start_idx,
    otherwise falls back to --benchmark-all (subject-only mode).

    On TimeoutExpired, attempts to recover partial stdout (available since Python 3.8).
    Returns a CompletedProcess with the partial stdout on timeout so the caller
    can still salvage benchmark data from hanging chunks.
    """
    env = os.environ.copy()
    env["CHAOS_IL2CPP_LOG_LEVEL"] = "0"  # suppress debug logs in benchmark output
    if end_idx > start_idx:
        cmd = [str(exe_path), "--benchmark-range", str(start_idx), str(end_idx), str(iterations)]
    else:
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
    technology: str = "chaos-aot",
):
    """Append unified-format records.jsonl alongside the chunk-local benchmark.json."""
    records_path = ctx.results_dir / "records.jsonl"
    now = datetime.now(timezone.utc).strftime("%Y-%m-%dT%H:%M:%SZ")

    record = {
        "runId": f"{ctx.assembly}-{ctx.slug}-benchmark-{int(time.time())}",
        "subject": f"{ctx.assembly}/{ctx.slug}",
        "mode": "native",
        "technology": technology,
        "platform": "windows-x64",
        "device": {"id": "chunk-pipeline", "name": "chunk-pipeline"},
        "recordedAt": now,
        "gitCommit": "unknown",
        "nativeConfig": ctx.native_config,
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


def _load_method_index_map(ctx: ChunkContext) -> dict[int, str] | None:
    """Read native-aot.methods.json and build methodIndex → subjectId mapping.

    Returns None if the manifest is not available or lacks subjectId fields.
    """
    manifest_path = ctx.chunk_dir / "native" / "codegen" / "generated" / "native-aot.methods.json"
    if not manifest_path.exists():
        return None

    try:
        manifest = json.loads(manifest_path.read_text(encoding="utf-8-sig"))
        methods = manifest.get("methods", [])
        if not methods or "subjectId" not in methods[0]:
            return None

        mapping: dict[int, str] = {}
        for m in methods:
            idx = m.get("index")
            sid = m.get("subjectId", "")
            if idx is not None and sid:
                mapping[idx] = sid
        return mapping
    except (json.JSONDecodeError, KeyError, IndexError, IOError):
        return None


def _get_manifest_method_count(ctx: ChunkContext) -> int:
    """Read total method count from native-aot.methods.json, or 0 if unavailable."""
    manifest_path = ctx.chunk_dir / "native" / "codegen" / "generated" / "native-aot.methods.json"
    if not manifest_path.exists():
        return 0
    try:
        manifest = json.loads(manifest_path.read_text(encoding="utf-8-sig"))
        return manifest.get("methodCount", 0)
    except (json.JSONDecodeError, IOError):
        return 0


def _read_benchmark_metadata(ctx: ChunkContext) -> list[dict]:
    """Read subjects metadata for methodSubjectId resolution."""
    metadata_path = ctx.subjects_metadata_path
    if not metadata_path.exists():
        return []
    try:
        metadata = json.loads(metadata_path.read_text(encoding="utf-8"))
        return metadata.get("methods", [])
    except (json.JSONDecodeError, IOError):
        return []


def _write_perf_store(
    per_method_stats: list[dict],
    ctx: ChunkContext,
    technology: str,
    metadata_methods: list[dict],
    iterations: int,
    method_index_to_subject_id: dict[int, str] | None = None,
    benchmark_start_idx: int = 0,
):
    """Write benchmark-history.jsonl in dashboard-compatible format.

    When method_index_to_subject_id is provided, resolves methodSubjectId
    from the method table index (benchmark_start_idx + position) using the
    codegen manifest, rather than by position into metadata_methods.
    """
    now = datetime.now(timezone.utc).strftime("%Y-%m-%dT%H:%M:%SZ")
    perf_path = _RESULTS_BASE / ctx.assembly / ctx.slug / "perf" / "benchmark-history.jsonl"
    perf_path.parent.mkdir(parents=True, exist_ok=True)

    # Determine append mode: check if AOT already wrote (JIT appends)
    append = perf_path.exists()

    with open(perf_path, "a" if append else "w", encoding="utf-8") as f:
        for i, s in enumerate(per_method_stats):
            elapsed_ms = s.get("meanDurationMs", 0)
            ops = s.get("meanOpsPerSecond", 0)

            method_subject_id = ""
            if method_index_to_subject_id is not None:
                # Use manifest-based mapping: method table index → subjectId
                method_table_index = benchmark_start_idx + i
                method_subject_id = method_index_to_subject_id.get(method_table_index, "")
            elif i < len(metadata_methods):
                # Fallback: position-based mapping into metadata
                method_subject_id = metadata_methods[i].get("methodSubjectId", "")

            record = {
                "timestamp": now,
                "slug": ctx.slug,
                "technology": technology,
                "methodSubjectId": method_subject_id,
                "methodIndex": i,
                "metrics": {
                    "elapsedMilliseconds": elapsed_ms if elapsed_ms > 0 else 0.001,
                    "opsPerSecond": ops,
                },
                "iterations": iterations,
                "status": "completed",
                "nativeConfig": ctx.native_config,
            }
            f.write(json.dumps(record, ensure_ascii=False, separators=(",", ":")) + "\n")


def _run_single_benchmark(
    exe_path: Path,
    technology: str,
    ctx: ChunkContext,
    timeout: int,
    metadata_methods: list[dict],
) -> dict | None:
    """Run benchmark for a single technology (AOT or JIT).

    Uses the codegen manifest (native-aot.methods.json) when available to
    benchmark ALL compiled methods with correct methodSubjectId mapping.
    Falls back to subject-only --benchmark-all when manifest is unavailable.

    Returns result dict with keys (per_method_stats, summary, iterations,
    sample_rounds, method_count, tech_duration_ms) or None on failure.
    """
    start = time.perf_counter()

    # Load method index → subjectId mapping from codegen manifest
    method_index_to_subject_id = _load_method_index_map(ctx)
    total_method_count = _get_manifest_method_count(ctx) if method_index_to_subject_id else 0
    _ = total_method_count  # suppress unused (available for future range-mode support)

    # Determine benchmark range
    benchmark_start_idx = 0
    benchmark_end_idx = 0
    # NOTE: --benchmark-range is disabled because the current entry.exe
    # (built from an older TPG template) does not support it.  The subject
    # slot map based --benchmark-all mode is used instead.
    use_range = False

    entry_count = _get_entry_count(exe_path) if not use_range else benchmark_end_idx

    mode_label = "--benchmark-range" if use_range else "--benchmark-all"
    print(f"  [benchmark] [{technology}] {exe_path} {mode_label} 0..{benchmark_end_idx} (adaptive)")
    iterations = _calibrate_iterations(exe_path, timeout, entry_count,
                                       start_idx=benchmark_start_idx, end_idx=benchmark_end_idx)
    print(f"  [benchmark] [{technology}] calibrated iterations={iterations}, entries={entry_count}")

    # Phase 1: Adaptive sampling rounds (3-10, early stop on CV < 5%)
    max_rounds = 10
    min_rounds = 3
    all_rounds: list[list[dict]] = []

    for s in range(max_rounds):
        print(f"  [benchmark] [{technology}] sampling round {s + 1}/{max_rounds}...")
        result = _run_entry_once(exe_path, iterations, timeout,
                                  start_idx=benchmark_start_idx, end_idx=benchmark_end_idx)
        if result is None:
            if all_rounds:
                break
            return None

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
                    print(f"  [benchmark] [{technology}] early stop at round {s + 1} (median CV={median_cv:.4f} < 5%)")
                    break

    if not all_rounds or not all_rounds[0]:
        return None

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

    tech_duration_ms = int((time.perf_counter() - start) * 1000)
    print(f"  [benchmark] [{technology}] {method_count} methods "
          f"({len(all_rounds)} samples, {tech_duration_ms}ms, {iterations} iterations)")

    # Phase 4: Write perf store
    _write_perf_store(per_method_stats, ctx, technology, metadata_methods, iterations,
                      method_index_to_subject_id=method_index_to_subject_id,
                      benchmark_start_idx=benchmark_start_idx)

    return {
        "per_method_stats": per_method_stats,
        "summary": summary,
        "iterations": iterations,
        "sample_rounds": len(all_rounds),
        "method_count": method_count,
        "tech_duration_ms": tech_duration_ms,
    }


def run_benchmark_chunk(ctx: ChunkContext, stages: dict[str, StageResult]) -> StageResult:
    """Benchmark stage: run AOT and optionally JIT entry.exes with adaptive settings."""
    start = time.perf_counter()
    timeout = max(ctx.stage_timeout_seconds or 300, 30)
    metadata_methods = _read_benchmark_metadata(ctx)

    # Read technologies to benchmark
    technologies: list[tuple[Path, str]] = []

    # AOT: required
    aot_exe = ctx.entry_exe_path
    if aot_exe.exists():
        technologies.append((aot_exe, "chaos-aot"))
    else:
        return StageResult(
            stage="benchmark", status="skipped",
            summary=f"entry.exe not found, skipping benchmark",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    # JIT: optional (only if entry-jit.exe was built)
    jit_exe = ctx.entry_jit_exe_path
    if jit_exe.exists():
        technologies.append((jit_exe, "chaos-jit"))
    else:
        print(f"  [benchmark] entry-jit.exe not found, skipping chaos-jit benchmark")

    # Run benchmarks for each technology
    results: list[dict] = []
    errors: list[str] = []
    for exe_path, tech in technologies:
        result = _run_single_benchmark(exe_path, tech, ctx, timeout, metadata_methods)
        if result is None:
            errors.append(f"{tech}: no data returned")
            continue
        results.append(result)

    if not results:
        return StageResult(
            stage="benchmark", status="error",
            summary="all benchmark technologies failed",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    # Write chunk-local output files from the primary (AOT) result
    primary = results[0]
    per_method_stats = primary["per_method_stats"]
    summary = primary["summary"]
    iterations = primary["iterations"]
    method_count = primary["method_count"]
    sample_rounds = primary["sample_rounds"]

    ctx.results_dir.mkdir(parents=True, exist_ok=True)
    result_path = ctx.results_dir / "benchmark.json"

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
        "sampleRounds": sample_rounds,
        "stderr": "",
        "technologiesRun": [t for _, t in technologies],
    }
    result_path.write_text(json.dumps(result_data, indent=2), encoding="utf-8")

    # Write unified-format records.jsonl from primary result
    _write_records_jsonl(per_method_stats, summary, ctx, iterations)

    # FP-12: Validate benchmark data minimum quality
    if status == "passed" and method_count > 0:
        zero_duration = sum(1 for m in per_method_stats if m.get("meanDurationMs", 0) <= 0)
        if zero_duration == method_count:
            status = "warning_no_valid_data"
            print(f"  [benchmark] WARNING: all {method_count} methods returned zero/negative duration")
        elif zero_duration > method_count * 0.5:
            status = "warning_mostly_zero"
            print(f"  [benchmark] WARNING: {zero_duration}/{method_count} methods returned zero/negative duration")

    status = "passed" if method_count > 0 else "error"
    if errors:
        status = "passed_with_errors" if status == "passed" else "error"

    duration_ms = int((time.perf_counter() - start) * 1000)
    print(f"  [benchmark] {status}: {method_count} methods "
          f"({len(results)} technologies, {sample_rounds} samples, {duration_ms}ms, {iterations} iterations)")

    return StageResult(
        stage="benchmark", status=status,
        summary=f"{status}: {method_count} methods, {len(results)} techs, "
                f"{sample_rounds} samples, {summary.get('totalOutliers', 0)} outliers removed"
                + (f"; errors: {'; '.join(errors[:2])}" if errors else ""),
        details=result_data,
        duration_ms=duration_ms,
    )
