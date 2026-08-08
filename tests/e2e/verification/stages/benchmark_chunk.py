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
from verification._path import results_base

_RESULTS_BASE = results_base()

# Benchmark calibration defaults
_CALIB_TARGET_MS = 10.0          # target duration per method for iteration scaling
_CALIB_MIN_ITERATIONS = 50      # minimum iterations regardless of method speed
_CALIB_FALLBACK_PROBE_FAIL = 1000    # fallback when probe yields no data
_CALIB_FALLBACK_ALL_FAST = 10000     # fallback when all methods are very fast
_CALIB_CAP_LARGE_CHUNK = 10000       # iteration cap for chunks with >5000 entries
_CALIB_CAP_DEFAULT = 50000           # iteration cap for normal chunks
_CALIB_LARGE_THRESHOLD = 3000        # entry count threshold for large chunk cap
_MIN_ELAPSED_FLOOR = 0.001           # minimum elapsed ms for perf store (avoid zero)


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


def _parse_benchmark_lines(stdout: str | bytes) -> tuple[list[dict], dict]:
    """Parse line-by-line JSON benchmark output from entry.exe stdout.

    Each method produces one complete JSON line ending with \n.
    The last line is {"summary":{...}}.
    Stray lines that aren't valid JSON are silently skipped.
    """
    if isinstance(stdout, bytes):
        stdout = stdout.decode("utf-8", errors="replace")
    lines = stdout.strip().split('\n')
    results = []
    summary = {}
    _dropped_count_local = 0
    for line in lines:
        line = line.strip()
        if not line:
            continue
        try:
            obj = json.loads(line)
        except json.JSONDecodeError:
            _dropped_count_local += 1
            continue
        if 'summary' in obj:
            summary = obj['summary']
        else:
            results.append(obj)
    return results, summary


def _calibrate_iterations(exe_path: Path, timeout: int, entry_count: int = 0,
                          start_idx: int = 0, end_idx: int = 0) -> int:
    """Probe-run with 10 iterations, scale to target ~50ms total per method."""
    # Probe all methods in the given range to calibrate iteration count
    result = _run_entry_once(exe_path, 10, min(timeout, 30), start_idx=start_idx, end_idx=end_idx or None)
    if result is None or not result.stdout:
        return 0  # calibration failed, skip benchmark

    data, _ = _parse_benchmark_lines(result.stdout or "")
    if not data:
        print("  [benchmark] [calibrate] probe yielded no data, using fallback iteration count")
        return _CALIB_FALLBACK_PROBE_FAIL  # fallback

    # Collect positive elapsed times to estimate per-call cost
    elapsed = [
        float(r['elapsedMilliseconds']) for r in data
        if isinstance(r.get('elapsedMilliseconds'), (int, float))
        and r['elapsedMilliseconds'] > 0
    ]
    if not elapsed:
        print("  [benchmark] [calibrate] all methods very fast (no elapsed > 0), using fallback iteration count")
        return _CALIB_FALLBACK_ALL_FAST  # all very fast, use high default

    # Per-call ms = median elapsed / 10 (probe iterations)
    median_elapsed = sorted(elapsed)[len(elapsed) // 2]
    per_call_ms = median_elapsed / 10.0
    iterations = max(_CALIB_MIN_ITERATIONS, int(_CALIB_TARGET_MS / max(per_call_ms, _MIN_ELAPSED_FLOOR)))

    # Cap: use smaller cap for large chunks
    cap = _CALIB_CAP_LARGE_CHUNK if entry_count > _CALIB_LARGE_THRESHOLD else _CALIB_CAP_DEFAULT
    return min(iterations, cap)


def _run_entry_once(exe_path: Path, iterations: int, timeout: int,
                     start_idx: int = 0, end_idx: int = 0) -> subprocess.CompletedProcess | None:
    """Run entry.exe benchmark once, returning the CompletedProcess or None on failure.

    Uses --benchmark-range start end iterations when end_idx > start_idx,
    otherwise falls back to --benchmark-all (subject-only mode).

    Benchmark protocol JSON is written to an OS temp file via `--benchmark-out`
    (native decoupling: machine data no longer rides on the console stdout
    stream). After the run, the file content is read back into `result.stdout`
    so downstream parsing is unchanged.

    On TimeoutExpired, attempts to recover any partial file content. Returns a
    CompletedProcess with stdout=file content (possibly empty) on timeout so the
    benchmark can continue past hanging methods.
    """
    env = os.environ.copy()
    env["CHAOS_IL2CPP_LOG_LEVEL"] = "0"  # suppress debug logs in benchmark output

    # Temp file for native benchmark JSON output. delete=False so the native
    # subprocess (a separate process) can open the path by name; we unlink it
    # explicitly after reading the data back.
    import tempfile
    fh = tempfile.NamedTemporaryFile(mode="w", suffix=".jsonl", encoding="utf-8",
                                     delete=False)
    bench_out_path = fh.name
    fh.close()  # release the handle so the child can open/truncate the path

    if end_idx is not None and end_idx > start_idx:
        cmd = [str(exe_path), "--benchmark-range", str(start_idx), str(end_idx),
               str(iterations), "--benchmark-out", bench_out_path]
    else:
        cmd = [str(exe_path), "--benchmark-all", str(iterations),
               "--benchmark-out", bench_out_path]

    stderr_str = ""
    try:
        r = subprocess.run(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE,
                           text=True, timeout=timeout, env=env, errors="replace")
        stderr_str = r.stderr
    except subprocess.TimeoutExpired as e:
        stderr_str = e.stderr or ""
        r = subprocess.CompletedProcess(
            args=e.cmd, returncode=-1, stdout="", stderr=stderr_str,
        )

    # The native side wrote benchmark JSON to the temp file. Read it back (this
    # is the authoritative data). If the file is empty, fall back to the
    # subprocess stdout for older natives that don't support --benchmark-out.
    file_data = ""
    if bench_out_path:
        try:
            file_data = Path(bench_out_path).read_text(encoding="utf-8", errors="replace")
        except OSError:
            file_data = ""
    # Clean up the temp file (explicit unlink since delete=False).
    try:
        Path(bench_out_path).unlink(missing_ok=True)
    except OSError:
        pass
    if not file_data:
        file_data = r.stdout or ""

    return subprocess.CompletedProcess(
        args=cmd, returncode=r.returncode, stdout=file_data, stderr=stderr_str,
    )


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
        "runId": ctx.run_id or f"{ctx.assembly}-{ctx.slug}-benchmark-{int(time.time())}",
        "subject": f"{ctx.assembly}/{ctx.slug}",
        "mode": "native",
        "technology": technology,
        "platform": ctx.platform or "windows-x64",
        "device": ctx.device or {"id": "chunk-pipeline", "name": "chunk-pipeline"},
        "recordedAt": now,
        "gitCommit": ctx.git_commit or "unknown",
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
    """Write benchmark-history.jsonl in dashboard-compatible format (single tech).

    When method_index_to_subject_id is provided, resolves methodSubjectId
    from the method table index (benchmark_start_idx + position) using the
    codegen manifest, rather than by position into metadata_methods.

    Kept for backward-compat / single-tech callers. `run_benchmark_chunk`
    prefers `_write_combined_perf_store` which writes ALL techs in one pass so
    later technologies do not overwrite earlier ones (the overwrite bug).
    """
    tech_stats = [(technology, per_method_stats, iterations)]
    _write_combined_perf_store(
        tech_stats, ctx, metadata_methods,
        method_index_to_subject_id=method_index_to_subject_id,
        benchmark_start_idx=benchmark_start_idx,
    )


def _write_combined_perf_store(
    tech_stats: list[tuple[str, list[dict], int]],
    ctx: ChunkContext,
    metadata_methods: list[dict],
    method_index_to_subject_id: dict[int, str] | None = None,
    benchmark_start_idx: int = 0,
):
    """Write benchmark-history.jsonl aggregating ALL technologies in one pass.

    `tech_stats` is a list of (technology, per_method_stats, iterations).
    Opening the file in "w" mode exactly ONCE and writing every technology's
    records means no technology's data is clobbered by a later write — the root
    cause of the missing chaos-aot benchmark data (each per-tech _write_perf_store
    previously opened the same path with "w", so chaos-jit erased chaos-aot).
    """
    now = datetime.now(timezone.utc).strftime("%Y-%m-%dT%H:%M:%SZ")
    perf_path = _RESULTS_BASE / ctx.assembly / ctx.slug / "perf" / "benchmark-history.jsonl"
    perf_path.parent.mkdir(parents=True, exist_ok=True)

    # Always overwrite the whole file ONCE per run: each pipeline run produces a
    # self-contained file. Append mode caused stale data accumulation across
    # partial runs, mixing old net8-jit baselines with fresh chaos-aot results.
    with open(perf_path, "w", encoding="utf-8") as f:
        for technology, per_method_stats, iterations in tech_stats:
            for i, s in enumerate(per_method_stats):
                record = _build_perf_record(
                    s, ctx, technology, metadata_methods, iterations,
                    method_index_to_subject_id, benchmark_start_idx, i, now,
                )
                f.write(json.dumps(record, ensure_ascii=False, separators=(",", ":")) + "\n")


def _build_perf_record(
    s: dict,
    ctx: ChunkContext,
    technology: str,
    metadata_methods: list[dict],
    iterations: int,
    method_index_to_subject_id: dict[int, str] | None,
    benchmark_start_idx: int,
    i: int,
    now: str,
) -> dict:
    """Build one benchmark-history.jsonl record dict for a single method entry."""
    elapsed_ms = s.get("meanDurationMs", 0)
    ops = s.get("meanOpsPerSecond", 0)

    method_subject_id = ""
    # Primary key: use the metadata methodSubjectId so chaos-aot and
    # net8-jit records share the same identifier (raw .NET method name).
    # This enables direct alignment in benchmark_report without needing
    # the slug+methodIndex fallback.
    if i < len(metadata_methods):
        method_subject_id = (metadata_methods[i].get("methodSubjectId", "")
                             or metadata_methods[i].get("subjectId", ""))

    # Secondary key: CombinedSubjects wrapper name from codegen manifest.
    # Stored as a separate field for debugging; NOT used for alignment.
    combined_subjects_id = ""
    if method_index_to_subject_id is not None:
        method_table_index = benchmark_start_idx + i
        combined_subjects_id = method_index_to_subject_id.get(method_table_index, "")
        if not method_subject_id:
            method_subject_id = combined_subjects_id

    return {
        "timestamp": now,
        "slug": ctx.slug,
        "technology": technology,
        "methodSubjectId": method_subject_id,
        "combinedSubjectsId": combined_subjects_id,
        "methodIndex": i,
        "metrics": {
            # CONTRACT: elapsedMilliseconds = TOTAL for this iterations batch
            # (per-iteration normalization happens once in _get_elapsed).
            "elapsedMilliseconds": elapsed_ms if elapsed_ms > 0 else _MIN_ELAPSED_FLOOR,
            "opsPerSecond": ops,
        },
        "iterations": iterations,
        "status": "completed",
        "nativeConfig": ctx.native_config,
    }


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

    # Always use --benchmark-all (--benchmark-range is not supported by current entry.exe)
    entry_count = _get_entry_count(exe_path)

    print(f"  [benchmark] [{technology}] {exe_path} --benchmark-all (adaptive)")
    iterations = _calibrate_iterations(exe_path, timeout, entry_count,
                                       start_idx=0, end_idx=0)
    print(f"  [benchmark] [{technology}] calibrated iterations={iterations}, entries={entry_count}")
    if iterations <= 0:
        print(f"  [benchmark] [{technology}] calibration failed, skipping")
        return None

    # Phase 1: Adaptive sampling rounds (3-10, early stop on CV < 5%)
    max_rounds = 10
    min_rounds = 3
    all_rounds: list[list[dict]] = []

    for s in range(max_rounds):
        print(f"  [benchmark] [{technology}] sampling round {s + 1}/{max_rounds}...")
        result = _run_entry_once(exe_path, iterations, timeout)
        if result is None:
            if all_rounds:
                break
            return None

        # JIT diagnostic: capture stderr when output is empty
        if technology == "chaos-jit" and not (result.stdout or "").strip():
            stderr_out = (result.stderr or "")[:500]
            if stderr_out:
                print(f"  [benchmark] [jit-diag] stderr: {stderr_out}")
        
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

    # Phase 3b: Calibration quality monitoring.
    # Calibration uses a single method (index 0).  If actual durations vary
    # widely across methods, the per-call estimate from method 0 may not
    # generalize.  Emit a WARN when the slowest method is >10x the median.
    if per_method_stats:
        valid_durations = [
            s.get("meanDurationMs", 0) for s in per_method_stats
            if s.get("meanDurationMs", 0) > 0
        ]
        if valid_durations:
            sorted_d = sorted(valid_durations)
            median_d = sorted_d[len(sorted_d) // 2]
            max_d = sorted_d[-1]
            if median_d > 0 and max_d > median_d * 10:
                print(f"  [benchmark] [{technology}] WARNING: calibration from method 0 may under-estimate "
                      f"— max={max_d:.1f}ms, median={median_d:.1f}ms ({max_d/median_d:.0f}x range)")

    tech_duration_ms = int((time.perf_counter() - start) * 1000)
    print(f"  [benchmark] [{technology}] {method_count} methods "
          f"({len(all_rounds)} samples, {tech_duration_ms}ms, {iterations} iterations)")

    # Note: perf-store (benchmark-history.jsonl) write is deferred to
    # `run_benchmark_chunk`, which collects ALL technologies' results and writes
    # a single self-contained file. Writing here per-technology with "w" mode
    # overwrote the previous technology's records (chaos-aot was clobbered by
    # the subsequent chaos-jit write) — the root cause of missing chaos-aot
    # benchmark data. See _write_combined_perf_store().

    return {
        "per_method_stats": per_method_stats,
        "summary": summary,
        "iterations": iterations,
        "sample_rounds": len(all_rounds),
        "method_count": method_count,
        "tech_duration_ms": tech_duration_ms,
        "technology": technology,
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
    # Track whether the AOT technology produced data. AOT (chaos-aot) is the
    # primary signal — its absence must be surfaced (strict warn), not silently
    # masked by a succeeding chaos-jit. net10-jit/net8-jit absence stays an
    # optional baseline warning (managed_benchmark handles those separately).
    aot_ran = False
    missing_techs: list[str] = []  # non-fatal tech absences -> warning, not error
    for exe_path, tech in technologies:
        result = _run_single_benchmark(exe_path, tech, ctx, timeout, metadata_methods)
        if result is None:
            missing_techs.append(f"{tech}: no data returned")
            if tech == "chaos-aot":
                print(f"  [benchmark] ⚠️ chaos-aot produced NO data — skipping")
            continue
        results.append(result)
        if tech == "chaos-aot":
            aot_ran = True

    if not results:
        return StageResult(
            stage="benchmark", status="error",
            summary="all benchmark technologies failed",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    # Write the combined perf-store (benchmark-history.jsonl) ONCE for ALL
    # successful technologies. Each result now carries its own "technology"
    # label (returned by _run_single_benchmark). Writing here aggregates
    # chaos-aot + chaos-jit + ... in a single "w" pass so no technology's
    # records are overwritten by a later one (the overwrite bug that erased
    # chaos-aot data).
    _write_combined_perf_store(
        [(r["technology"], r["per_method_stats"], r["iterations"]) for r in results],
        ctx,
        metadata_methods,
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

    status = "passed" if method_count > 0 else "error"

    # FP-12: Validate benchmark data minimum quality
    if status == "passed" and method_count > 0:
        zero_duration = sum(1 for m in per_method_stats if m.get("meanDurationMs", 0) <= 0)
        if zero_duration == method_count:
            status = "failed"
            errors.append("all methods returned zero/negative duration — benchmark data invalid")
            print(f"  [benchmark] FAILED: all {method_count} methods returned zero/negative duration")
        elif zero_duration > method_count * 0.5:
            status = "failed"
            errors.append(f"{zero_duration}/{method_count} methods returned zero/negative duration")
            print(f"  [benchmark] FAILED: {zero_duration}/{method_count} methods returned zero/negative duration")

    # Strict AOT non-silence (decision 1): if JIT produced data but the primary
    # AOT signal did not, mark the stage a WARNING (not silent-passed, not
    # full-error). JIT data is retained and the pipeline continues; downstream
    # consumers (P2-E warning model) see details.chaosAot == "no_data".
    if not aot_ran:
        if missing_techs:
            print(f"  [benchmark] WARN: chaos-aot produced NO benchmark data (AOT signal absent)")
        result_data["chaosAot"] = "no_data"
        if status == "passed":
            status = "warning"

    # Non-fatal tech absences (e.g. chaos-jit not built) surface as a warning
    # in the summary but must NOT fail the pipeline.
    result_data["missingTechnologies"] = missing_techs

    if errors:
        status = "error"

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
