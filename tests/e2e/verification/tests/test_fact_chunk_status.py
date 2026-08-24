"""Regression tests for fact-chunk gate semantics (Class C).

Verifies that genuine method failures / silent dispatch gaps turn the
fact stage red instead of being reported as non-fatal "partial". This closes
the false-green path where a chunk with failing subjects still exited 0.
"""
from __future__ import annotations

import json

from stages.fact_chunk import _tech_status


def _result(passed: int, total: int, rc: int) -> dict:
    return {"passed": passed, "total": total, "returncode": rc,
            "results": [], "error": None}


def test_full_pass_is_passed():
    assert _tech_status(_result(10, 10, 0), meta_total=None) == "passed"


def test_empty_is_error():
    assert _tech_status(_result(0, 0, 0), meta_total=None) == "error"


def test_clean_exit_with_failures_is_failed():
    # rc==0 but passed<total => real assertion failures: must be fatal.
    assert _tech_status(_result(8, 10, 0), meta_total=None) == "failed"


def test_crash_with_failures_is_failed():
    assert _tech_status(_result(8, 10, 139), meta_total=None) == "failed"


def test_crash_after_all_pass_is_partial():
    # non-zero exit with passed==total => truncated JSON, not a real failure.
    assert _tech_status(_result(10, 10, 139), meta_total=None) == "partial"
