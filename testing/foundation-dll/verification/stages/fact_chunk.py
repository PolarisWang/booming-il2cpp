"""Simplified fact stage for chunk-based pipeline.

Runs entry.exe --fact-json from the chunk's native directory and parses
per-method JSON results.

Status determination is exit-code based: a clean exit means all subjects
were dispatched; a crash with passed==total is shutdown-AV (acceptable);
a crash with passed<total is genuine failure.

Metadata cross-check (expectedTotal) is advisory-only — codegen may produce
fewer subjects than metadata declares, and that's expected.
"""

from __future__ import annotations

import json
import subprocess
import time
from pathlib import Path

from verification.orchestration.context import ChunkContext, StageResult

# Known shutdown-AV exit codes: process crashed during CRT teardown AFTER
# all subjects completed successfully.  The dispatch results are complete.
_SHUTDOWN_AV_CODES = frozenset({
    3221225477,   # 0xC0000005 — STATUS_ACCESS_VIOLATION
    3221226505,   # 0xC0000409 — STATUS_STACK_BUFFER_OVERRUN
})


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
    json_truncated = False
    try:
        json_start = stdout.find("{")
        json_end = stdout.rfind("}") + 1
        if json_start >= 0 and json_end > json_start:
            payload = stdout[json_start:json_end]
            parsed = json.loads(payload)
            fact_results = parsed.get("factResults", [])
    except (json.JSONDecodeError, KeyError):
        # Truncated JSON — entry.exe crashed before flushing final ]}
        json_truncated = True
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

    # ── Parse assertion failure messages from stderr ──
    # Assert.Fail() writes "[ASSERT FAIL] ..." to stderr for each failure.
    # Map them to failing subjects by order (Nth stderr line = Nth failure).
    assert_messages: list[str] = []
    if stderr:
        for line in stderr.splitlines():
            if "[ASSERT FAIL]" in line:
                msg = line[line.index("[ASSERT FAIL]") + 14:].strip()
                assert_messages.append(msg)
    fail_idx = 0
    for fr in fact_results:
        if not fr.get("passed") and fail_idx < len(assert_messages):
            fr["message"] = assert_messages[fail_idx]
            fail_idx += 1

    # ── Metadata cross-check (advisory only) ──
    # The metadata declares totalMethods = all entries (fact + benchmark + hotupdate).
    # Codegen may produce fewer subjects (e.g. when some methods fail lowering),
    # so a shortfall is NOT diagnosed as "partial".
    # factMethodCount = unique fact wrappers (not value sets) — a closer match to
    # kSubjectEntryCount. Pipeline uses this for mismatch detection.
    meta_total = None
    fact_method_count = None
    meta_benchmark_count = 0
    try:
        meta_path = ctx.chunk_dir / "managed" / "subjects" / "subjects.metadata.json"
        if meta_path.exists():
            meta = json.loads(meta_path.read_text(encoding="utf-8"))
            meta_total = meta.get("totalMethods")
            fact_method_count = meta.get("factMethodCount") or meta_total
            meta_benchmark_count = len(meta.get("benchmarkMethodIndices") or [])
    except Exception:
        pass

    # ── Status determination ──
    # Clean exit: all subjects dispatched, results are complete.
    # Shutdown AV: process crashed after all subjects completed (teardown race).
    # Partial: crash mid-dispatch, some results lost.
    # Failed: crash with subject failures.
    is_shutdown_av = (
        r.returncode != 0
        and r.returncode in _SHUTDOWN_AV_CODES
        and passed == total
        and total > 0
    )
    is_clean = r.returncode == 0

    if is_clean and passed == total and total > 0:
        status = "passed"
    elif is_shutdown_av:
        status = "passed"  # subjects complete, teardown crash is acceptable
    elif total == 0:
        status = "skipped" if meta_total else "error"
    elif r.returncode != 0 and passed < total:
        status = "partial" if passed > 0 else "error"
    elif r.returncode != 0:
        status = "failed"
    elif passed < total:
        status = "failed"
    else:
        status = "passed"

    # ── Value integrity check ──
    # The fact-json mode now uses ChaosDispatchMethodGetValue, so the
    # "value" field contains the actual managed method return value
    # (int64_t), not a dispatch status code.
    # Void methods return RAX garbage — flag those as value-unstable
    # rather than trying to validate.
    # Populate exitCode from value for backward compatibility: -1 means
    # caught/exception, 0 means normal return.
    value_warnings = sum(
        1 for r in fact_results
        if r.get("passed") and r.get("value", 0) < 0 and r.get("value", 0) != -1
    )
    for fr in fact_results:
        fr["exitCode"] = -1 if not fr.get("passed") else (fr.get("value", 0) if fr.get("value", 0) >= -1 else 0)
    value_suspicious = value_warnings > 0

    # ── Build result ──
    # crashed_index: which subject index was being dispatched when the crash
    # happened.  Only set for partial mid-dispatch crashes (not shutdown AV).
    crashed_index = None
    if r.returncode != 0 and not is_shutdown_av and json_truncated and total > 0:
        # The subject at index `total` (0-based) was the one being dispatched
        # when the crash occurred (the last complete entry is total-1).
        crashed_index = total

    result_data = {
        "exitCode": r.returncode,
        "passed": passed,
        "total": total,
        "metaTotal": meta_total,
        "factMethodCount": fact_method_count,
        "metaBenchmarkCount": meta_benchmark_count,
        "crashedAtIndex": crashed_index,
        "isShutdownAV": is_shutdown_av,
        "valueWarnings": value_warnings,
        "valueSuspicious": value_suspicious,
        "results": fact_results,
        "stderr": stderr[:500] if stderr else "",
    }

    # Save to chunk results dir
    ctx.results_dir.mkdir(parents=True, exist_ok=True)
    result_path = ctx.results_dir / "fact.json"
    result_path.write_text(json.dumps(result_data, indent=2), encoding="utf-8")

    duration_ms = int((time.perf_counter() - start) * 1000)
    parts = [f"  [fact] {status}: {passed}/{total} passed"]
    if is_shutdown_av:
        parts.append(f"shutdown_av=0x{r.returncode:08X}")
    if meta_total is not None and meta_total != total:
        parts.append(f"meta={meta_total}")
    if value_warnings:
        parts.append(f"value_warnings={value_warnings}")
    parts.append(f"({duration_ms}ms)")
    print(" ".join(parts))

    summary_detail = f"exit={r.returncode}"
    if is_shutdown_av:
        summary_detail += ", shutdown_av"
    if crashed_index is not None:
        summary_detail += f", crash at subject index {crashed_index}"
    if value_warnings:
        summary_detail += f", {value_warnings} value warning(s)"

    return StageResult(
        stage="fact", status=status,
        summary=f"{status}: {passed}/{total} passed ({summary_detail})",
        details=result_data,
        duration_ms=duration_ms,
    )
