"""Regression tests for the fact-chunk cross-technology count split (T0.5).

fact.json aggregates real / unassertable / smoke / factoryGap / failed purely
from the AOT annotated records.  The headline ``passed``/``total`` therefore
have to come from the SAME run.

They used to be replaced by the JIT counts whenever the JIT pass-rate exceeded
the AOT pass-rate.  That mixed populations and produced the impossible
``gatePassed > gateTotal`` -- observed on the threading chunk as 520 > 519,
because ``gateTotal = total - factoryGap`` subtracted an AOT-derived
factory-gap count from a JIT-derived total.

The regression is worth guarding because the bug was *directional*: it flattered
exactly the case with the most signal (AOT failing where JIT passes), which is a
genuine AOT lowering defect.  On the threading chunk it hid a real one --
``CancellationTokenRegistrationTests::DisposeAsync_1__0`` AOT=FAIL / JIT=PASS.
"""
from __future__ import annotations

from stages.fact_chunk import select_reference_counts


def _tech(passed: int, total: int) -> dict:
    return {"passed": passed, "total": total}


def test_aot_is_the_reference_population():
    assert select_reference_counts(_tech(519, 523), _tech(520, 523))[:2] == (519, 523)


def test_higher_jit_pass_rate_does_not_replace_aot_counts():
    # The exact threading-chunk shape: JIT passes one more than AOT.
    passed, total, _, _ = select_reference_counts(_tech(519, 523), _tech(520, 523))
    assert (passed, total) == (519, 523), "JIT counts must not become the headline"


def test_jit_counts_are_still_reported():
    assert select_reference_counts(_tech(519, 523), _tech(520, 523)) == (519, 523, 520, 523)


def test_gate_numerator_never_exceeds_denominator():
    """The invariant the old behaviour violated (gatePassed=520 > gateTotal=519)."""
    passed, total, _, _ = select_reference_counts(_tech(519, 523), _tech(520, 523))
    factory_gap = 4
    gate_total = total - factory_gap
    assert passed <= gate_total, f"gatePassed={passed} > gateTotal={gate_total}"


def test_missing_jit_run_reports_zeros():
    assert select_reference_counts(_tech(10, 10), None) == (10, 10, 0, 0)


def test_missing_keys_do_not_raise():
    # Older/partial fact payloads must not crash the aggregate writer.
    assert select_reference_counts({}, {}) == (0, 0, 0, 0)


def test_none_values_fall_back_to_zero():
    assert select_reference_counts({"passed": None, "total": None},
                                    {"passed": None, "total": None}) == (0, 0, 0, 0)
