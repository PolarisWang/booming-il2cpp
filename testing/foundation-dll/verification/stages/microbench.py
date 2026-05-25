"""Microbench stage runner — interpreter internal metrics via entry.exe --microbench."""

from __future__ import annotations

import re
import subprocess
import time
from typing import Any

from orchestration.context import FamilyContext, StageResult


def run_microbench(ctx: FamilyContext, stages: dict[str, StageResult]) -> StageResult:
    """Stage 7: Run native entry.exe --microbench and parse metrics.

    Requires entry.exe (built by codegen + dispatch + cmake stages).
    """
    start = time.perf_counter()

    exe_path = ctx.entry_exe_path
    if not exe_path.exists():
        return StageResult(
            stage="microbench", status="skipped",
            summary="entry.exe not found (codegen stage may have failed)",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    print(f"  [microbench] Running {exe_path} --microbench...")
    try:
        r = subprocess.run(
            [str(exe_path), "--microbench"],
            capture_output=True, text=True, timeout=120,
        )
    except subprocess.TimeoutExpired:
        return StageResult(
            stage="microbench", status="error",
            summary="microbench timed out (120s)",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    output = (r.stdout or "") + (r.stderr or "")
    exit_code = r.returncode

    ACCESS_VIOLATION_EXIT = 0xC0000005
    if exit_code < 0:
        pass
    elif exit_code == ACCESS_VIOLATION_EXIT:
        print(f"  [microbench] CRASHED with access violation (0xC0000005)")
        print(f"    This is a known GC concurrency issue: BGC thread races with")
        print(f"    mutator during repeated allocations. See FD-I2 in")
        print(f"    docs/dev/in-progress/foundation-dll-industrialization/industrialization-assessment.md")
    elif exit_code != 0:
        print(f"  [microbench] non-zero exit: {exit_code}")

    metrics: dict[str, Any] = {"exitCode": exit_code}
    for line in output.splitlines():
        line = line.strip()
        if not line or line.startswith("[") or line.startswith("Benchmark "):
            continue
        m = re.match(r'^(.+?):\s+([\d.]+)\s+(ns/op|ns/call)\b', line)
        if m:
            label = m.group(1).strip()
            metrics[label] = {
                "value": float(m.group(2)),
                "unit": m.group(3),
            }

    status = "passed" if exit_code == 0 else "failed"
    print(f"  [microbench] Result: {status} ({len(metrics)} metrics)")

    return StageResult(
        stage="microbench", status=status,
        summary=f"{status} ({len(metrics)} metrics, exit={exit_code})",
        details={"metrics": metrics, "exitCode": exit_code},
        duration_ms=int((time.perf_counter() - start) * 1000),
    )
