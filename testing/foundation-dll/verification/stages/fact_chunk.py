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
            capture_output=True, timeout=600,
        )
    except subprocess.TimeoutExpired:
        return StageResult(
            stage="fact", status="error",
            summary="fact timed out after 600s",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    stdout = r.stdout.decode("utf-8", errors="replace") if r.stdout else ""
    stderr = r.stderr.decode("utf-8", errors="replace") if r.stderr else ""

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

    # Read expected total from subjects.metadata.json for accurate comparison
    expected_total = None
    try:
        meta_path = ctx.chunk_dir / "managed" / "subjects" / "subjects.metadata.json"
        if meta_path.exists():
            meta = json.loads(meta_path.read_text(encoding="utf-8"))
            expected_total = meta.get("totalMethods")
    except Exception:
        pass

    # Save results to chunk results dir
    ctx.results_dir.mkdir(parents=True, exist_ok=True)
    result_path = ctx.results_dir / "fact.json"

    # Determine effective total for status: use expected_total if available and larger
    effective_total = expected_total if expected_total is not None else total

    # Determine which subject index caused the crash (the unflushed one)
    # If JSON is truncated (total < effective_total) and process crashed,
    # the subject at index `total` (0-based) is the crash culprit.
    crashed_index = (total if total < effective_total and r.returncode != 0
                     and r.returncode != 0 else None)

    result_data = {
        "exitCode": r.returncode,
        "passed": passed,
        "total": total,
        "expectedTotal": effective_total,
        "crashedAtIndex": crashed_index,
        "isPartial": total < effective_total,
        "results": fact_results,
        "stderr": stderr[:500] if stderr else "",
    }
    result_path.write_text(json.dumps(result_data, indent=2), encoding="utf-8")

    status = "failed"
    if total == 0:
        status = "error"
    elif r.returncode != 0 and total < effective_total:
        # Process crashed before dispatching all subjects — partial results
        # (e.g. threading chunk: /GS crash at si=2 truncates to 2/673)
        status = "partial" if total > 0 else "error"
    elif r.returncode != 0 and passed == total:
        # All dispatched subjects completed successfully but process crashed
        # during teardown (e.g. /GS stack cookie in CRT after last subject).
        # Subject results are complete — treat as passed.
        status = "passed"
    elif r.returncode != 0:
        # Process crashed AND some subjects failed — ambiguous
        status = "failed"
    elif passed == total and total > 0:
        status = "passed"
    elif total < effective_total:
        # Clean exit but fewer than expected — metadata mismatch
        status = "partial"

    duration_ms = int((time.perf_counter() - start) * 1000)
    expected_str = f" (expected {effective_total})" if effective_total != total else ""
    print(f"  [fact] {status}: {passed}/{total} passed{expected_str} ({duration_ms}ms)")

    summary_detail = f"exit={r.returncode}"
    if crashed_index is not None:
        summary_detail += f", crash at subject index {crashed_index}"

    return StageResult(
        stage="fact", status=status,
        summary=f"{status}: {passed}/{total} passed ({summary_detail})",
        details=result_data,
        duration_ms=duration_ms,
    )
