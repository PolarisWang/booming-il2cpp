"""Simplified benchmark stage for chunk-based pipeline.

Runs entry.exe --benchmark-all N from the chunk's native directory.
"""

from __future__ import annotations

import json
import subprocess
import time

from verification.orchestration.context import ChunkContext, StageResult

_ITERATIONS = 1000  # default iterations per method


def run_benchmark_chunk(ctx: ChunkContext, stages: dict[str, StageResult]) -> StageResult:
    """Benchmark stage: run chunk's entry.exe --benchmark-all N."""
    start = time.perf_counter()

    exe_path = ctx.entry_exe_path
    if not exe_path.exists():
        return StageResult(
            stage="benchmark", status="skipped",
            summary=f"entry.exe not found, skipping benchmark",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    iterations = _ITERATIONS
    print(f"  [benchmark] Running {exe_path} --benchmark-all {iterations}...")
    try:
        r = subprocess.run(
            [str(exe_path), "--benchmark-all", str(iterations)],
            capture_output=True, text=True, timeout=300,
        )
    except subprocess.TimeoutExpired:
        return StageResult(
            stage="benchmark", status="error",
            summary="benchmark timed out after 300s",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    stdout = r.stdout or ""
    stderr = r.stderr or ""

    # Parse JSON output
    benchmark_data = {}
    try:
        json_start = stdout.find("{")
        json_end = stdout.rfind("}") + 1
        if json_start >= 0 and json_end > json_start:
            payload = stdout[json_start:json_end]
            parsed = json.loads(payload)
            benchmark_data = parsed.get("benchmarkAll", [])
    except (json.JSONDecodeError, KeyError):
        pass

    ctx.results_dir.mkdir(parents=True, exist_ok=True)
    result_path = ctx.results_dir / "benchmark.json"
    result_data = {
        "exitCode": r.returncode,
        "iterations": iterations,
        "methodCount": len(benchmark_data),
        "results": benchmark_data,
        "stderr": stderr[:500] if stderr else "",
    }
    result_path.write_text(json.dumps(result_data, indent=2), encoding="utf-8")

    status = "passed" if len(benchmark_data) > 0 else "error"
    duration_ms = int((time.perf_counter() - start) * 1000)
    print(f"  [benchmark] {status}: {len(benchmark_data)} methods benchmarked ({duration_ms}ms)")

    return StageResult(
        stage="benchmark", status=status,
        summary=f"{status}: {len(benchmark_data)} methods at {iterations} iterations each",
        details=result_data,
        duration_ms=duration_ms,
    )
