"""Simplified fact stage for chunk-based pipeline.

Runs entry.exe --fact-json from the chunk's native directory and parses
per-method JSON results.
"""

from __future__ import annotations

import json
import subprocess
import time
from pathlib import Path

from verification.orchestration.context import ChunkContext, StageResult


def run_fact_chunk(ctx: ChunkContext, stages: dict[str, StageResult]) -> StageResult:
    """Fact stage: run chunk's entry.exe --fact-json."""
    start = time.perf_counter()

    exe_path = ctx.entry_exe_path
    if not exe_path.exists():
        return StageResult(
            stage="fact", status="failed",
            summary=f"entry.exe not found: {exe_path}",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    print(f"  [fact] Running {exe_path} --fact-json...")
    try:
        r = subprocess.run(
            [str(exe_path), "--fact-json"],
            capture_output=True, text=True, timeout=120,
        )
    except subprocess.TimeoutExpired:
        return StageResult(
            stage="fact", status="error",
            summary="fact timed out after 120s",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    stdout = r.stdout or ""
    stderr = r.stderr or ""

    # Parse JSON output
    fact_results = []
    try:
        # Find JSON object in stdout (entry.exe may print other text)
        json_start = stdout.find("{")
        json_end = stdout.rfind("}") + 1
        if json_start >= 0 and json_end > json_start:
            payload = stdout[json_start:json_end]
            parsed = json.loads(payload)
            fact_results = parsed.get("factResults", [])
    except (json.JSONDecodeError, KeyError):
        # If parsing fails, try appending missing closing brackets.
        # entry.exe sometimes crashes (SEGFAULT) during shutdown before
        # flushing the final ]}, but all fact entries are already written.
        try:
            if json_start >= 0:
                payload = stdout[json_start:]
                for suffix in ("]}", "}", ""):
                    try:
                        parsed = json.loads(payload + suffix)
                        fact_results = parsed.get("factResults", [])
                        if fact_results:
                            break
                    except json.JSONDecodeError:
                        continue
        except Exception:
            pass

    passed = sum(1 for r in fact_results if r.get("passed"))
    total = len(fact_results)

    # Save results to chunk results dir
    ctx.results_dir.mkdir(parents=True, exist_ok=True)
    result_path = ctx.results_dir / "fact.json"
    result_data = {
        "exitCode": r.returncode,
        "passed": passed,
        "total": total,
        "results": fact_results,
        "stderr": stderr[:500] if stderr else "",
    }
    result_path.write_text(json.dumps(result_data, indent=2), encoding="utf-8")

    status = "passed" if passed == total and total > 0 else "failed"
    if total == 0:
        if r.returncode == 0:
            status = "skipped"
        else:
            status = "error"

    duration_ms = int((time.perf_counter() - start) * 1000)
    print(f"  [fact] {status}: {passed}/{total} passed ({duration_ms}ms)")

    return StageResult(
        stage="fact", status=status,
        summary=f"{status}: {passed}/{total} passed (exit={r.returncode})",
        details=result_data,
        duration_ms=duration_ms,
    )
