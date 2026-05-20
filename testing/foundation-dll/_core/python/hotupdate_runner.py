"""HotUpdate stage runners — patch lifecycle and post-patch verification.

Uses entry.exe --hotupdate and entry.exe --hotupdate-and-benchmark (and JIT variants).
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


def _run_hotupdate_fact(exe_path: Path) -> dict[str, Any]:
    """Run entry.exe --hotupdate and parse JSON result."""
    try:
        r = subprocess.run(
            [str(exe_path), "--hotupdate"],
            capture_output=True, text=True, timeout=120,
        )
    except subprocess.TimeoutExpired:
        return {"status": "error", "summary": "timed out (120s)"}
    except OSError as e:
        return {"status": "error", "summary": str(e)}

    output = (r.stdout or "").strip()
    for line in output.splitlines():
        line = line.strip()
        if line.startswith("{"):
            try:
                data = json.loads(line)
                passed = data.get("passedMethods", 0)
                failed = data.get("failedMethods", 0)
                total = data.get("totalMethods", 0)
                status = "passed" if failed == 0 else "failed"
                return {
                    "status": status,
                    "passedMethods": passed,
                    "failedMethods": failed,
                    "totalMethods": total,
                    "exitCode": r.returncode,
                }
            except json.JSONDecodeError:
                pass

    return {
        "status": "failed" if r.returncode != 0 else "passed",
        "exitCode": r.returncode,
        "rawOutput": (r.stdout or "")[:500],
    }


def _run_hotupdate_benchmark(
    exe_path: Path, method_index: int, iterations: int = 100000,
) -> dict[str, Any]:
    """Run entry.exe --hotupdate-and-benchmark N and parse JSON timing."""
    try:
        r = subprocess.run(
            [str(exe_path), "--hotupdate-and-benchmark", str(method_index), str(iterations)],
            capture_output=True, text=True, timeout=120,
        )
    except subprocess.TimeoutExpired:
        return {"methodIndex": method_index, "error": "timed out"}

    output = (r.stdout or "").strip()
    for line in output.splitlines():
        line = line.strip()
        if line.startswith("{"):
            try:
                return json.loads(line)
            except json.JSONDecodeError:
                pass
    return {"methodIndex": method_index, "error": f"no JSON: {(r.stdout or '')[:200]}", "exitCode": r.returncode}


def run_hotupdate(ctx: FamilyContext, stages: dict[str, StageResult]) -> StageResult:
    """Stage 9: HotUpdate AOT Fact — run entry.exe --hotupdate."""
    start = time.perf_counter()

    exe_path = ctx.entry_exe_path
    if not exe_path.exists():
        return StageResult(
            stage="hotupdate", status="skipped",
            summary="entry.exe not found",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    print(f"  [hotupdate] Running {exe_path} --hotupdate...")
    result = _run_hotupdate_fact(exe_path)
    status = result.get("status", "failed")
    passed = result.get("passedMethods", 0)
    total = result.get("totalMethods", 0)

    print(f"  [hotupdate] Result: {status} ({passed}/{total})")

    return StageResult(
        stage="hotupdate", status=status,
        summary=f"{status} ({passed}/{total})",
        details=result,
        duration_ms=int((time.perf_counter() - start) * 1000),
    )


def run_hotupdate_aot_bench(ctx: FamilyContext, stages: dict[str, StageResult]) -> StageResult:
    """Stage 10: HotUpdate AOT Bench — run entry.exe --hotupdate-and-benchmark for all methods."""
    start = time.perf_counter()

    exe_path = ctx.entry_exe_path
    if not exe_path.exists():
        return StageResult(
            stage="hotupdate_aot_benchmark", status="skipped",
            summary="entry.exe not found",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    method_count = _load_method_count(ctx)
    if method_count == 0:
        return StageResult(
            stage="hotupdate_aot_benchmark", status="skipped",
            summary="no methods in contract",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    print(f"  [hotupdate_aot_bench] Running {method_count} methods...")
    results: list[dict[str, Any]] = []
    ok_count = 0
    for i in range(method_count):
        r = _run_hotupdate_benchmark(exe_path, i)
        results.append(r)
        if "error" not in r:
            ok_count += 1

    status = "passed" if ok_count > 0 else "failed"
    print(f"  [hotupdate_aot_bench] Result: {status} ({ok_count}/{method_count})")

    return StageResult(
        stage="hotupdate_aot_benchmark", status=status,
        summary=f"{status} ({ok_count}/{method_count})",
        details={"results": results, "okCount": ok_count, "totalMethods": method_count},
        duration_ms=int((time.perf_counter() - start) * 1000),
    )


def run_hotupdate_jit_fact(ctx: FamilyContext, stages: dict[str, StageResult]) -> StageResult:
    """Stage 11: HotUpdate JIT Fact — run entry-jit.exe --hotupdate."""
    start = time.perf_counter()

    exe_path = ctx.entry_jit_exe_path
    if not exe_path.exists():
        return StageResult(
            stage="hotupdate_jit_fact", status="skipped",
            summary="entry-jit.exe not found",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    print(f"  [hotupdate_jit_fact] Running {exe_path} --hotupdate...")
    result = _run_hotupdate_fact(exe_path)
    status = result.get("status", "failed")
    passed = result.get("passedMethods", 0)
    total = result.get("totalMethods", 0)

    print(f"  [hotupdate_jit_fact] Result: {status} ({passed}/{total})")

    return StageResult(
        stage="hotupdate_jit_fact", status=status,
        summary=f"{status} ({passed}/{total})",
        details=result,
        duration_ms=int((time.perf_counter() - start) * 1000),
    )


def run_hotupdate_jit_bench(ctx: FamilyContext, stages: dict[str, StageResult]) -> StageResult:
    """Stage 12: HotUpdate JIT Bench — run entry-jit.exe --hotupdate-and-benchmark."""
    start = time.perf_counter()

    exe_path = ctx.entry_jit_exe_path
    if not exe_path.exists():
        return StageResult(
            stage="hotupdate_jit_benchmark", status="skipped",
            summary="entry-jit.exe not found",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    method_count = _load_method_count(ctx)
    if method_count == 0:
        return StageResult(
            stage="hotupdate_jit_benchmark", status="skipped",
            summary="no methods in contract",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    print(f"  [hotupdate_jit_bench] Running {method_count} methods...")
    results: list[dict[str, Any]] = []
    ok_count = 0
    for i in range(method_count):
        r = _run_hotupdate_benchmark(exe_path, i)
        results.append(r)
        if "error" not in r:
            ok_count += 1

    status = "passed" if ok_count > 0 else "failed"
    print(f"  [hotupdate_jit_bench] Result: {status} ({ok_count}/{method_count})")

    return StageResult(
        stage="hotupdate_jit_benchmark", status=status,
        summary=f"{status} ({ok_count}/{method_count})",
        details={"results": results, "okCount": ok_count, "totalMethods": method_count},
        duration_ms=int((time.perf_counter() - start) * 1000),
    )
