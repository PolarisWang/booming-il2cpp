"""Regression tests for cross-tech (AOT vs JIT) diff semantics.

The AOT and JIT executables legitimately disagree about some methods because the
two modes dispatch differently: AOT marks subject methods `kHotpatchKeepNative`
(the native body runs) while JIT leaves the entry flags at 0 and routes through
the interpreter — that IS JIT mode's purpose.  For a body that reaches an
unimplemented external-runtime helper, the interpreter raises where the native
path returns the structural 42 (an `unassertable` smoke result).

Neither verdict is a conclusion about the method under test, so reporting that
pair as a cross-tech inconsistency is noise that reads like a defect.  These
tests pin the exclusion — and, more importantly, pin that everything else is
still compared strictly.
"""
from __future__ import annotations

from stages.fact_chunk import split_cross_tech_diffs


def _rec(mid: str, passed: bool, kind: str) -> dict:
    return {"methodSubjectId": mid, "passed": passed, "resultKind": kind}


def test_unassertable_vs_factorygap_is_excluded():
    # The exact shape observed in the threading-tasks chunk:
    # AOT ran the smoke body (42, unassertable); JIT's interpreter path raised
    # because SubjectInstanceFactory.Create<ValueTask>() is unimplemented.
    aot = [_rec("M::AsTask_5__0", True, "unassertable")]
    jit = [_rec("M::AsTask_5__0", False, "factoryGap")]

    diffs, excluded = split_cross_tech_diffs(aot, jit)

    assert diffs == []
    assert len(excluded) == 1
    assert excluded[0]["methodSubjectId"] == "M::AsTask_5__0"
    assert "assertion conclusion" in excluded[0]["reason"]


def test_factorygap_vs_unassertable_is_excluded_symmetrically():
    # Direction must not matter — the pair is unordered.
    aot = [_rec("M::X", False, "factoryGap")]
    jit = [_rec("M::X", True, "unassertable")]

    diffs, excluded = split_cross_tech_diffs(aot, jit)

    assert diffs == []
    assert len(excluded) == 1


def test_real_kind_mismatch_is_still_reported():
    # The case this diff exists for: a genuine assertion that disagrees.
    aot = [_rec("M::Real", True, "real")]
    jit = [_rec("M::Real", False, "realDefect")]

    diffs, excluded = split_cross_tech_diffs(aot, jit)

    assert len(diffs) == 1
    assert excluded == []
    assert diffs[0]["aotKind"] == "real"
    assert diffs[0]["jitKind"] == "realDefect"


def test_factorygap_vs_realdefect_is_still_reported():
    # factoryGap only pairs with unassertable.  Against a genuine defect it must
    # still surface — otherwise the exclusion would swallow real signal.
    aot = [_rec("M::Y", False, "factoryGap")]
    jit = [_rec("M::Y", True, "real")]

    diffs, excluded = split_cross_tech_diffs(aot, jit)

    assert len(diffs) == 1
    assert excluded == []


def test_failed_vs_unassertable_is_still_reported():
    # `failed` (caught, unclassified) against unassertable is NOT the excluded
    # pair — only factoryGap is.
    aot = [_rec("M::Z", True, "unassertable")]
    jit = [_rec("M::Z", False, "failed")]

    diffs, excluded = split_cross_tech_diffs(aot, jit)

    assert len(diffs) == 1
    assert excluded == []


def test_agreeing_records_produce_nothing():
    aot = [_rec("M::A", True, "real"), _rec("M::B", False, "factoryGap")]
    jit = [_rec("M::A", True, "real"), _rec("M::B", False, "factoryGap")]

    diffs, excluded = split_cross_tech_diffs(aot, jit)

    assert diffs == []
    assert excluded == []


def test_both_directions_in_one_run_partition_correctly():
    # Mixed population: one excluded pair, one genuine diff, one agreement.
    aot = [_rec("M::Excl", True, "unassertable"),
           _rec("M::Real", True, "real"),
           _rec("M::Same", True, "real")]
    jit = [_rec("M::Excl", False, "factoryGap"),
           _rec("M::Real", False, "realDefect"),
           _rec("M::Same", True, "real")]

    diffs, excluded = split_cross_tech_diffs(aot, jit)

    assert [d["methodSubjectId"] for d in diffs] == ["M::Real"]
    assert [e["methodSubjectId"] for e in excluded] == ["M::Excl"]


def test_missing_resultkind_defaults_to_compared():
    # Records without a resultKind (e.g. pre-annotation) must not be silently
    # excluded — an absent kind is not the excluded pair.
    aot = [{"methodSubjectId": "M::N", "passed": True}]
    jit = [{"methodSubjectId": "M::N", "passed": False}]

    diffs, excluded = split_cross_tech_diffs(aot, jit)

    assert len(diffs) == 1
    assert excluded == []
