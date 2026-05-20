"""Fact verification stage runners — run native entry EXE and verify assertions."""

from __future__ import annotations

import re
import subprocess
import time
from typing import Any

from _core.python.models import FamilyContext, StageResult


def run_fact(ctx: FamilyContext, stages: dict[str, StageResult]) -> StageResult:
    """Stage 3: Fact AOT — run il2cpp-translated native entry EXE."""
    start = time.perf_counter()

    # Try new framework path first, fall back to old structure
    exe_path = ctx.entry_exe_path
    if not exe_path.exists():
        return StageResult(
            stage="fact", status="failed",
            summary="entry.exe not found (codegen stage may have failed)",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    print(f"  [fact] Running {exe_path}...")
    r = subprocess.run([str(exe_path)], capture_output=True, text=True, timeout=120)
    output = r.stdout + r.stderr

    passed = total = 0
    for line in output.splitlines():
        m = re.search(r'Passed:\s*(\d+)/(\d+)', line)
        if m:
            passed, total = int(m.group(1)), int(m.group(2))
        if "FAIL" in line or "fail" in line.lower():
            print(f"    {line}")

    status = "passed" if r.returncode == 0 else "failed"
    print(f"  [fact] Result: {status} ({passed}/{total})")

    return StageResult(
        stage="fact", status=status,
        summary=f"{status} ({passed}/{total})",
        details={"passed": passed, "total": total, "exitCode": r.returncode},
        duration_ms=int((time.perf_counter() - start) * 1000),
    )


def run_fact_jit(ctx: FamilyContext, stages: dict[str, StageResult]) -> StageResult:
    """Stage 4: Fact JIT — run entry-jit.exe through interpreter dispatch."""
    start = time.perf_counter()

    jit_exe = ctx.entry_jit_exe_path

    if not jit_exe.exists():
        return StageResult(
            stage="fact_jit", status="skipped",
            summary="entry-jit.exe not found (JIT codegen skipped or failed)",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    print(f"  [fact_jit] Running {jit_exe}...")
    r = subprocess.run([str(jit_exe)], capture_output=True, text=True, timeout=120)
    output = r.stdout + r.stderr

    passed = total = 0
    for line in output.splitlines():
        m = re.search(r'Passed:\s*(\d+)/(\d+)', line)
        if m:
            passed, total = int(m.group(1)), int(m.group(2))

    status = "passed" if r.returncode == 0 else "failed"
    print(f"  [fact_jit] Result: {status} ({passed}/{total})")

    return StageResult(
        stage="fact_jit", status=status,
        summary=f"{status} ({passed}/{total})",
        details={"passed": passed, "total": total, "exitCode": r.returncode},
        duration_ms=int((time.perf_counter() - start) * 1000),
    )
