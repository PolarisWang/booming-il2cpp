"""Regression tests for the fact gate's numerator/denominator consistency.

The gate ratio must describe ONE population.  The denominator excludes
infrastructure-gap kinds (factoryGap / stubGap / nullArg / envSensitive) because
those say nothing about whether the method under test is correct.  The numerator
must therefore count only passes drawn from that same remainder.

It previously did not: it used the run-wide pass count, so a chunk dominated by
stubGap could report a ratio above 100% — measured at 880% on the reflection
chunk (gateTotal 50, gatePassed 440), where 390 of those "passes" were methods
that never executed.
"""
from __future__ import annotations

import sys
from pathlib import Path

_HERE = Path(__file__).resolve().parent  # verification/tests/
for _d in _HERE.parents:
    if (_d / "_path.py").exists():
        if str(_d) not in sys.path:
            sys.path.insert(0, str(_d))
        break
from _path import foundation_root  # noqa: E402

_FOUNDATION_DLL = foundation_root()
if str(_FOUNDATION_DLL) not in sys.path:
    sys.path.insert(0, str(_FOUNDATION_DLL))

from stages.fact_chunk import compute_gate_counts  # noqa: E402


def _rec(kind: str, passed: bool) -> dict:
    return {"resultKind": kind, "passed": passed}


def test_gate_passed_never_exceeds_gate_total():
    """The invariant that was violated: a ratio is a fraction, not a percentage
    of something else."""
    # The exact shape measured on reflection: 390 stubGap passes + 47 real + 3
    # unassertable.
    annotated = (
        [_rec("stubGap", True) for _ in range(390)]
        + [_rec("real", True) for _ in range(47)]
        + [_rec("unassertable", True) for _ in range(3)]
    )

    gate_total, gate_passed = compute_gate_counts(annotated)

    assert gate_total == 50, "stubGap must not inflate the denominator"
    assert gate_passed == 50, "stubGap must not inflate the numerator either"
    assert gate_passed <= gate_total


def test_stub_gap_passes_are_not_counted_as_verified():
    """A method with no AOT body never ran; it must not count as passing."""
    annotated = [_rec("stubGap", True) for _ in range(10)]

    gate_total, gate_passed = compute_gate_counts(annotated)

    assert gate_total == 0
    assert gate_passed == 0


def test_factory_gap_excluded_from_both():
    annotated = [_rec("factoryGap", True) for _ in range(5)] + [_rec("real", True)]

    gate_total, gate_passed = compute_gate_counts(annotated)

    assert (gate_total, gate_passed) == (1, 1)


def test_null_arg_and_env_sensitive_excluded_from_both():
    annotated = [
        _rec("nullArg", False),
        _rec("envSensitive", False),
        _rec("real", True),
    ]

    gate_total, gate_passed = compute_gate_counts(annotated)

    assert (gate_total, gate_passed) == (1, 1)


def test_genuine_failure_stays_in_denominator_and_out_of_numerator():
    """A real failing assertion must still drag the ratio down — the whole point
    of the gate."""
    annotated = [_rec("real", True), _rec("real", False)]

    gate_total, gate_passed = compute_gate_counts(annotated)

    assert gate_total == 2
    assert gate_passed == 1


def test_unassertable_counts_as_passed_but_stays_in_denominator():
    """unassertable means the method RAN without asserting; it stays in the
    denominator so an all-void chunk cannot claim a free 100%."""
    annotated = [_rec("unassertable", True), _rec("real", False)]

    gate_total, gate_passed = compute_gate_counts(annotated)

    assert gate_total == 2
    assert gate_passed == 1


def test_empty_input():
    assert compute_gate_counts([]) == (0, 0)


def test_missing_resultkind_is_treated_as_measured():
    """Records without a kind (pre-annotation) are not infrastructure gaps, so
    they stay in both — silently dropping them would understate the denominator."""
    annotated = [{"passed": True}, {"passed": False}]

    gate_total, gate_passed = compute_gate_counts(annotated)

    assert gate_total == 2
    assert gate_passed == 1
