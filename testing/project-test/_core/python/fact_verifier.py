"""Fact verification stage runners — run entry EXE and validate stdout."""

from __future__ import annotations

import re
import subprocess
import time

from _pipeline.stage_result import StageResult


def _compare_output(actual: str, expected_path, ctx) -> dict:
    """Compare actual stdout against expected baseline.

    Supports two modes:
      1. Exact match — every line must match exactly
      2. Prefix match — lines starting with EVIDENCE_PREFIX are checked,
         other lines are ignored (for flexible output)

    Falls back to prefix matching if expected file doesn't exist.
    """
    from pathlib import Path

    if expected_path and Path(expected_path).exists():
        expected = Path(expected_path).read_text(encoding="utf-8").strip()
        actual_stripped = actual.strip()
        if actual_stripped == expected:
            return {"match": True, "mode": "exact"}
        # Show diff
        exp_lines = expected.splitlines()
        act_lines = actual_stripped.splitlines()
        diffs = []
        for i, (e, a) in enumerate(zip(exp_lines, act_lines)):
            if e != a:
                diffs.append({"line": i, "expected": e, "actual": a})
        if len(exp_lines) > len(act_lines):
            for i in range(len(act_lines), len(exp_lines)):
                diffs.append({"line": i, "expected": exp_lines[i], "actual": "(missing)"})
        return {"match": False, "mode": "exact", "differences": diffs[:20]}

    # Prefix matching (fallback): check all RTS_ lines
    prefix = "RTS_"
    actual_lines = [l for l in actual.splitlines() if l.startswith(prefix)]
    if actual_lines:
        return {"match": True, "mode": "prefix", "evidenceLines": actual_lines}
    return {"match": False, "mode": "prefix", "evidenceLines": actual_lines,
            "note": "No RTS_ evidence lines found in output"}


def run_project_fact(ctx, stages) -> StageResult:
    """Stage 3: Fact AOT — run il2cpp-translated entry EXE and verify output."""
    start = time.perf_counter()

    exe_path = ctx.entry_exe_path
    if not exe_path.exists():
        return StageResult(
            stage="fact", status="failed",
            summary="entry-aot.exe not found (codegen stage may have failed)",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    expected_path = ctx.expected_dir / "fact-output.txt"

    print(f"  [fact] Running {exe_path}...")
    try:
        r = subprocess.run([str(exe_path)], capture_output=True, text=True, timeout=120)
    except subprocess.TimeoutExpired:
        return StageResult(
            stage="fact", status="failed",
            summary="entry-aot.exe timed out (120s)",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    actual_output = r.stdout + r.stderr

    # Filter output to only RTS_ evidence lines (log output from runtime libs
    # writes to stdout with log framework which we cannot suppress here).
    actual_lines = [l for l in (r.stdout or "").splitlines() if l.startswith("RTS_")]
    actual_output = "\n".join(actual_lines)

    # Compare output against expected baseline
    comparison = _compare_output(actual_output, expected_path, ctx)

    # If expected file doesn't exist yet, create baseline from first run
    if not expected_path or not expected_path.exists():
        evidence_lines = [l for l in actual_output.splitlines() if l.startswith("RTS_")]
        if evidence_lines:
            baseline = "\n".join(evidence_lines)
            expected_path.parent.mkdir(parents=True, exist_ok=True)
            expected_path.write_text(baseline, encoding="utf-8")
            comparison = {"match": True, "mode": "baseline_created",
                          "evidenceLines": evidence_lines}

    status = "passed" if (comparison["match"] and r.returncode == 0) else "failed"
    print(f"  [fact] Result: {status} (mode={comparison['mode']})")

    return StageResult(
        stage="fact", status=status,
        summary=f"{status}: stdout {comparison['mode']} match" if comparison["match"]
                else f"FAILED: stdout mismatch",
        details={
            "match": comparison["match"],
            "matchMode": comparison["mode"],
            "exitCode": r.returncode,
            **comparison,
        },
        duration_ms=int((time.perf_counter() - start) * 1000),
    )


def run_project_fact_jit(ctx, stages) -> StageResult:
    """Stage 4: Fact JIT — run entry-jit.exe through interpreter dispatch."""
    start = time.perf_counter()

    jit_exe = ctx.entry_jit_exe_path
    if not jit_exe.exists():
        return StageResult(
            stage="fact_jit", status="skipped",
            summary="entry-jit.exe not found (JIT codegen skipped or failed)",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    expected_path = ctx.expected_dir / "fact-output.txt"
    print(f"  [fact_jit] Running {jit_exe}...")
    try:
        r = subprocess.run([str(jit_exe)], capture_output=True, text=True, timeout=120)
    except subprocess.TimeoutExpired:
        return StageResult(
            stage="fact_jit", status="failed",
            summary="entry-jit.exe timed out",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    # Filter to RTS_ evidence lines (same as AOT fact)
    actual_lines = [l for l in (r.stdout or "").splitlines() if l.startswith("RTS_")]
    actual_output = "\n".join(actual_lines)

    comparison = _compare_output(actual_output, expected_path, ctx)
    status = "passed" if (comparison["match"] and r.returncode == 0) else "failed"

    return StageResult(
        stage="fact_jit", status=status,
        summary=f"{status}: stdout match" if comparison["match"] else "FAILED",
        details={"match": comparison["match"], "exitCode": r.returncode},
        duration_ms=int((time.perf_counter() - start) * 1000),
    )
