"""Benchmark stage runner — native AOT and JIT performance measurement.

Runs entry.exe --benchmark and entry-jit.exe --benchmark for all methods.
Managed (.NET) benchmark requires additional harness setup (Phase 3 TODO).
"""

from __future__ import annotations

import json
import subprocess
import time
from pathlib import Path
from typing import Any

from _core.python.models import FamilyContext, StageResult


def _load_method_count(ctx: FamilyContext) -> int:
    """Load method count from contract.json."""
    contract_path = ctx.contract_path
    if not contract_path.exists():
        return 0
    try:
        import json as _json
        c = _json.loads(contract_path.read_text(encoding="utf-8"))
        mids = c.get("methodSubjectIds", [])
        if mids:
            return len(mids)
        return c.get("methodSubjectCount", 0)
    except Exception:
        return 0


def _run_single_benchmark(
    exe_path: Path, method_index: int, iterations: int = 100000,
) -> dict[str, Any] | None:
    """Run entry.exe --benchmark N and parse JSON timing output."""
    try:
        r = subprocess.run(
            [str(exe_path), "--benchmark", str(method_index), str(iterations)],
            capture_output=True, text=True, timeout=120,
        )
    except subprocess.TimeoutExpired:
        return {"methodIndex": method_index, "error": "timed out"}
    except OSError as e:
        return {"methodIndex": method_index, "error": str(e)}

    output = (r.stdout or "").strip()
    # Parse JSON from output
    for line in output.splitlines():
        line = line.strip()
        if line.startswith("{"):
            try:
                return json.loads(line)
            except json.JSONDecodeError:
                pass
    return {"methodIndex": method_index, "error": f"no JSON output: {output[:200]}", "exitCode": r.returncode}


def _run_all_benchmarks(
    ctx: FamilyContext, exe_path: Path, label: str,
) -> dict[str, Any]:
    """Run --benchmark for all methods under a given EXE."""
    method_count = _load_method_count(ctx)
    if method_count == 0:
        return {"status": "skipped", "summary": "no methods in contract"}

    results: list[dict[str, Any]] = []
    total_ops = 0.0
    ok_count = 0
    fail_count = 0

    for i in range(method_count):
        result = _run_single_benchmark(exe_path, i)
        results.append(result)
        if result and "error" not in result:
            total_ops += result.get("opsPerSecond", 0)
            ok_count += 1
        else:
            fail_count += 1

    avg_ops = total_ops / ok_count if ok_count > 0 else 0.0

    print(f"  [benchmark/{label}] {ok_count}/{method_count} OK, avg {avg_ops:.0f} ops/s")

    return {
        "status": "passed" if ok_count > 0 else "failed",
        "label": label,
        "results": results,
        "okCount": ok_count,
        "failCount": fail_count,
        "totalMethods": method_count,
        "averageOpsPerSecond": avg_ops,
    }


def run_benchmark(ctx: FamilyContext, stages: dict[str, StageResult]) -> StageResult:
    """Stage 8: 3-way benchmark (native-aot + native-jit).

    TODO(Phase 3): Add managed (.NET) benchmark via multi_benchmark_runner.
    """
    start = time.perf_counter()

    method_count = _load_method_count(ctx)
    if method_count == 0:
        return StageResult(
            stage="benchmark", status="skipped",
            summary="no methods in contract",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    aot_exe = ctx.entry_exe_path
    jit_exe = ctx.entry_jit_exe_path

    benchmarks: dict[str, Any] = {}

    # Native AOT benchmark
    if aot_exe.exists():
        aot_result = _run_all_benchmarks(ctx, aot_exe, "native-aot")
        benchmarks["native-aot"] = aot_result
    else:
        benchmarks["native-aot"] = {"status": "skipped", "summary": "entry.exe not found"}

    # Native JIT benchmark
    if jit_exe.exists():
        jit_result = _run_all_benchmarks(ctx, jit_exe, "native-jit")
        benchmarks["native-jit"] = jit_result
    else:
        benchmarks["native-jit"] = {"status": "skipped", "summary": "entry-jit.exe not found"}

    # Count total ok
    total_ok = sum(
        1 for b in benchmarks.values()
        if isinstance(b, dict) and b.get("status") == "passed"
    )

    status = "passed" if total_ok > 0 else "failed"

    return StageResult(
        stage="benchmark", status=status,
        summary=f"{status}: native-aot="
                f"{benchmarks.get('native-aot', {}).get('okCount', 0)}/"
                f"{benchmarks.get('native-aot', {}).get('totalMethods', 0)}, "
                f"native-jit="
                f"{benchmarks.get('native-jit', {}).get('okCount', 0)}/"
                f"{benchmarks.get('native-jit', {}).get('totalMethods', 0)}",
        details=benchmarks,
        duration_ms=int((time.perf_counter() - start) * 1000),
    )
