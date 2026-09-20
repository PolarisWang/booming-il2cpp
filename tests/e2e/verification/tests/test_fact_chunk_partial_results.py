"""Regression tests for partial-result recovery on fact-run timeout.

Background
----------
``entry.exe --fact-json`` streams its results: it prints ``{"factResults":[``
then one JSON object per subject, each followed by an fflush.  A single
deadlocking subject (e.g. ``new Task(() => {}).Wait()``) parks the process
forever, so the runner's 600 s ``subprocess.run(timeout=...)`` fires.

Before this fix the TimeoutExpired handler returned an empty dict — every
subject that HAD completed before the deadlock was thrown away, and the chunk
reported ``passed: 0 / total: 0`` (measured on threading-tasks: 8 subjects ran,
448 results lost).  The process-level recovery in ``pal_eh_posix.cpp`` already
documents this failure mode for the abort path ("truncating the streaming
factResults JSON ... reports as 0/0 passed for the whole chunk"); the timeout
path is the same defect through a different door.

These tests pin the recovery: a truncated stream must yield the subjects that
were fully emitted, and must be reported as truncated rather than silently
"0/0 passed".
"""
from __future__ import annotations

from stages.fact_chunk import _parse_fact_stdout, _tech_status


def _entry(si: int, passed: bool) -> str:
    return '{"subjectIndex":%d,"passed":%s}' % (si, "true" if passed else "false")


def test_complete_document_parses():
    # Well-formed output (process exited normally) — unchanged behaviour.
    stdout = '{"factResults":[' + _entry(0, True) + ',' + _entry(1, False) + ']}'
    results, truncated = _parse_fact_stdout(stdout)
    assert truncated is False
    assert len(results) == 2
    assert results[0]["passed"] is True
    assert results[1]["passed"] is False


def test_truncated_stream_keeps_completed_subjects():
    # The deadlock case: the array is still open when the process is killed.
    # The two fully-emitted entries must survive; the half-written third must not.
    stdout = ('{"factResults":[' + _entry(0, True) + ',' + _entry(1, True) + ','
              + '{"subjectIndex":2,"pas')
    results, truncated = _parse_fact_stdout(stdout)
    assert truncated is True
    assert [r["subjectIndex"] for r in results] == [0, 1]
    assert all(r["passed"] for r in results)


def test_truncated_immediately_after_header_keeps_zero_but_flags():
    # Killed before any subject completed: no results, but still "truncated"
    # so the caller can distinguish this from a genuinely empty run.
    results, truncated = _parse_fact_stdout('{"factResults":[')
    assert results == []
    assert truncated is True


def test_empty_output_is_not_truncated():
    # No output at all means the binary never started emitting — a different
    # failure than a mid-stream kill, and must not be silently "recovered".
    results, truncated = _parse_fact_stdout("")
    assert results == []
    assert truncated is False


def test_garbage_without_header_is_not_recovered():
    # Nothing resembling the header: refuse to invent results.
    results, truncated = _parse_fact_stdout("not json at all\nboom\n")
    assert results == []
    assert truncated is False


# ── Gate: a truncated run must not be promotable to "passed" ────────────────
#
# The pipeline promotes "partial" to "passed" whenever JIT succeeds.  Without
# this guard, recovering 8 of 258 subjects from a deadlock-killed run would go
# green — the timeout would look like a soft degradation instead of a hard stop.

def _result(passed: int, total: int, rc: int, truncated: bool) -> dict:
    return {"passed": passed, "total": total, "returncode": rc,
            "results": [], "error": None, "truncated": truncated}


def test_truncated_run_is_error_not_partial():
    # 8 of 258 subjects recovered from a mid-stream kill: every parsed subject
    # passed and rc is -1, which previously yielded "partial" -> promotable.
    assert _tech_status(_result(8, 8, -1, truncated=True), meta_total=258) == "error"


def test_truncated_run_without_meta_total_still_partial():
    # No metadata baseline to compare against: keep the old behaviour rather
    # than inventing a shortfall we cannot substantiate.
    assert _tech_status(_result(8, 8, -1, truncated=True), meta_total=None) == "partial"


def test_complete_run_is_not_affected_by_guard():
    # A full, complete run keeps its normal statuses — the guard is keyed on
    # the truncation flag, not on counts.
    assert _tech_status(_result(258, 258, 0, truncated=False), meta_total=258) == "passed"


def test_truncated_run_that_lost_a_failure_still_error():
    # Truncated AND a parsed subject failed: "error" (the run is invalid) —
    # never "failed"-as-final, because the remaining subjects were never run.
    assert _tech_status(_result(7, 8, -1, truncated=True), meta_total=258) == "error"
