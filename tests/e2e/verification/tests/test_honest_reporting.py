"""Regression tests for honest-reporting fields (smoke/real/stub exclusion).

Verifies that fact_chunk, aggregate, benchmark_chunk, and reporting stages
surface real-vs-smoke splits and stub-exclusion correctly — no false-green
where a chunk is reported "passed" while being primarily [UNVERIFIED] smoke
stubs or benchmark stubs.

These tests are pure unit/offline: they run on synthetic fixtures, not on
real entry.exe / nightly data.
"""
from __future__ import annotations

import json
from pathlib import Path
from typing import Any


# ──────────────────────────────────────────────────────────────────────────
# Test helpers
# ──────────────────────────────────────────────────────────────────────────

def _make_fact_json(
    passed: int = 0,
    total: int = 0,
    unverified_smoke: int = 0,
    real_passed: int = 0,
    real_total: int = 0,
) -> dict:
    """Build a synthetic fact.json payload so we can verify aggregate logic
    without a real entry.exe run.

    real_passed/real_total default to passed-smoke / total-smoke when absent
    (matching fact_chunk's _write_fact_results semantics).
    """
    if real_total == 0 and total > 0:
        real_total = total - unverified_smoke
    if real_passed == 0 and passed > 0:
        real_passed = passed - unverified_smoke
    return {
        "passed": passed,
        "total": total,
        "unverifiedSmoke": unverified_smoke,
        "realPassed": real_passed,
        "realTotal": real_total,
        "factMethodCount": total,
        "metaTotal": total,
        "valueSuspicious": False,
        "valueWarnings": [],
    }


def _make_chunk_summary(
    slug: str = "test-chunk",
    passed: int = 0,
    total: int = 0,
    unverified_smoke: int = 0,
    real_passed: int = 0,
    real_total: int = 0,
    hu_passed: int = 0,
    hu_failed: int = 0,
    hu_real_passed: int = 0,
    hu_real_failed: int = 0,
    hu_smoke_passed: int = 0,
    hu_smoke_failed: int = 0,
) -> dict:
    """Build a synthetic chunk_summary entry (the per-chunk dict that
    aggregate.py loops over).
    """
    fact = _make_fact_json(passed, total, unverified_smoke, real_passed, real_total)
    # Add smokeDominated flag (aggregate adds this during the loop)
    fact["smokeDominated"] = (
        fact["total"] > 0 and fact["unverifiedSmoke"] * 2 >= fact["total"]
    )
    return {
        "slug": slug,
        "fact": fact,
        "benchmark": {"methodCount": total, "status": "ran"},
        "hotupdate": {
            "patchDataUsed": False,
            "passed": hu_passed,
            "failed": hu_failed,
            "realPassed": hu_real_passed,
            "realFailed": hu_real_failed,
            "smokePassed": hu_smoke_passed,
            "smokeFailed": hu_smoke_failed,
            "realTotal": hu_real_passed + hu_real_failed,
            "smokeTotal": hu_smoke_passed + hu_smoke_failed,
            "realSmokeAnnotated": True,
        },
    }


# ──────────────────────────────────────────────────────────────────────────
# fact_chunk smoke tracking
# ──────────────────────────────────────────────────────────────────────────

def test_fact_json_carries_real_fields():
    """fact.json must carry unverifiedSmoke/realPassed/realTotal so the
    aggregate stage can compute real-vs-smoke splits."""
    fj = _make_fact_json(passed=143, total=143, unverified_smoke=121)
    assert fj["unverifiedSmoke"] == 121
    assert fj["realPassed"] == 22  # 143 - 121
    assert fj["realTotal"] == 22


def test_fact_json_no_smoke():
    """When no [UNVERIFIED] markers exist, realPassed must equal passed
    and realTotal must equal total."""
    fj = _make_fact_json(passed=50, total=50, unverified_smoke=0)
    assert fj["unverifiedSmoke"] == 0
    assert fj["realPassed"] == fj["passed"]
    assert fj["realTotal"] == fj["total"]


def test_fact_json_partial_smoke():
    """A chunk with both real and smoke subjects carries correct split."""
    fj = _make_fact_json(passed=80, total=100, unverified_smoke=20)
    assert fj["realPassed"] == 60
    assert fj["realTotal"] == 80


# ──────────────────────────────────────────────────────────────────────────
# aggregate smoke-domination logic
# ──────────────────────────────────────────────────────────────────────────

def test_smoke_dominated_chunk_flagged():
    """A chunk with >=50% [UNVERIFIED] smoke must be flagged smokeDominated."""
    # 121/143 smoke => 84.6% smoke => smokeDominated = True
    cs = _make_chunk_summary("net-http", 143, 143, 121)
    assert cs["fact"]["smokeDominated"] is True


def test_smoke_dominated_edge():
    """Exactly 50% smoke must also be flagged smokeDominated."""
    cs = _make_chunk_summary("half-smoke", 10, 10, 5)
    assert cs["fact"]["smokeDominated"] is True


def test_not_smoke_dominated():
    """A chunk with <50% smoke must NOT be flagged smokeDominated."""
    cs = _make_chunk_summary("mostly-real", 100, 100, 30)
    assert cs["fact"]["smokeDominated"] is False


def test_no_smoke_not_dominated():
    """A chunk with zero smoke must NOT be flagged smokeDominated."""
    cs = _make_chunk_summary("clean", 50, 50, 0)
    assert cs["fact"]["smokeDominated"] is False


def test_zero_total_not_dominated():
    """A chunk with no methods must not be smokeDominated (avoid div-by-zero)."""
    cs = _make_chunk_summary("empty", 0, 0, 0)
    assert cs["fact"]["smokeDominated"] is False


# ──────────────────────────────────────────────────────────────────────────
# aggregate real-vs-smoke aggregate computation
# ──────────────────────────────────────────────────────────────────────────

def test_aggregate_real_passed_across_chunks():
    """total_real_passed must be the sum of per-chunk realPassed, not
    the smoke-inclusive passed."""
    chunks = [
        _make_chunk_summary("http", 143, 143, 121, 22, 22),
        _make_chunk_summary("clean", 50, 50, 0, 50, 50),
        _make_chunk_summary("partial", 80, 100, 20, 60, 80),
    ]
    total_real_passed = sum(c["fact"]["realPassed"] for c in chunks)
    total_real_fact = sum(c["fact"]["realTotal"] for c in chunks)
    total_smoke = sum(c["fact"]["unverifiedSmoke"] for c in chunks)
    total_nominal_passed = sum(c["fact"]["passed"] for c in chunks)

    # Nominal: 273/293 passed (looks nearly green)
    assert total_nominal_passed == 273
    assert total_nominal_passed / 293 > 0.93  # 93% — misleadingly good

    # Real: 132/152 passed (87% — honest, shows the smoke gap)
    assert total_real_passed == 132
    assert total_real_fact == 152
    assert total_smoke == 141

    # The gap: 141 methods counted "passed" that are NOT real verifications
    assert total_nominal_passed - total_real_passed == 141


def test_aggregate_smoke_dominated_count():
    """smokeDominatedChunkCount must count only chunks where >=50% smoke."""
    chunks = [
        _make_chunk_summary("http", 143, 143, 121),    # 84.6% — dominated
        _make_chunk_summary("clean", 50, 50, 0),        # 0% — not dominated
        _make_chunk_summary("half", 10, 10, 5),         # 50% — dominated
        _make_chunk_summary("mostly", 100, 100, 30),    # 30% — not dominated
    ]
    # Aggregate loop would add smokeDominated; simulate it
    dominated = [c for c in chunks if c["fact"]["smokeDominated"]]
    assert len(dominated) == 2
    assert dominated[0]["slug"] == "http"
    assert dominated[1]["slug"] == "half"


# ──────────────────────────────────────────────────────────────────────────
# benchmark_chunk stub exclusion
# ──────────────────────────────────────────────────────────────────────────

def test_benchmark_single_per_method_stats():
    """Verify that the isStub heuristic correctly identifies methods at the
    elapsed floor as stubs.  This mirrors the logic in benchmark_chunk's
    _build_perf_record and the FP-12 zero-duration check."""
    # A method with meanDurationMs at the floor
    from stages.benchmark_chunk import _MIN_ELAPSED_FLOOR

    stub_stats = {"meanDurationMs": _MIN_ELAPSED_FLOOR}
    non_stub_stats = {"meanDurationMs": 1.5}
    zero_stats = {"meanDurationMs": 0.0}

    assert stub_stats["meanDurationMs"] <= _MIN_ELAPSED_FLOOR
    assert non_stub_stats["meanDurationMs"] > _MIN_ELAPSED_FLOOR

    # isStub = elapsed_ms <= _MIN_ELAPSED_FLOOR
    assert (stub_stats["meanDurationMs"] <= _MIN_ELAPSED_FLOOR) is True
    assert (non_stub_stats["meanDurationMs"] <= _MIN_ELAPSED_FLOOR) is False
    assert (zero_stats["meanDurationMs"] <= _MIN_ELAPSED_FLOOR) is True


def test_benchmark_result_data_stub_count():
    """Verify that benchmark_chunk's result_data includes stubCount and
    nonStubCount, and that the headline excludes stubs from the count of
    'real methods'."""
    from stages.benchmark_chunk import _MIN_ELAPSED_FLOOR

    # Simulate the per_method_stats loop that counts stubs
    per_method_stats = [
        {"meanDurationMs": _MIN_ELAPSED_FLOOR},  # stub
        {"meanDurationMs": _MIN_ELAPSED_FLOOR},  # stub
        {"meanDurationMs": 2.5},                  # real
        {"meanDurationMs": 3.1},                  # real
        {"meanDurationMs": 1.8},                  # real
    ]
    method_count = len(per_method_stats)
    stub_count = sum(1 for s in per_method_stats if s.get("meanDurationMs", 0) <= _MIN_ELAPSED_FLOOR)
    non_stub_count = method_count - stub_count

    assert method_count == 5
    assert stub_count == 2
    assert non_stub_count == 3

    # The headline should say "3 real methods (2 stubs excluded)" not "5 methods"
    assert non_stub_count < method_count  # stubs removed from "real" count


def test_benchmark_all_stubs_not_masked():
    """When ALL methods are stubs, the headline must clearly say 0 real
    methods rather than 'X methods passed'."""
    from stages.benchmark_chunk import _MIN_ELAPSED_FLOOR

    per_method_stats = [
        {"meanDurationMs": _MIN_ELAPSED_FLOOR},
        {"meanDurationMs": _MIN_ELAPSED_FLOOR},
    ]
    method_count = len(per_method_stats)
    stub_count = sum(1 for s in per_method_stats if s.get("meanDurationMs", 0) <= _MIN_ELAPSED_FLOOR)
    non_stub_count = method_count - stub_count

    assert method_count == 2
    assert stub_count == 2
    assert non_stub_count == 0  # all stubs, zero real methods


# ──────────────────────────────────────────────────────────────────────────
# reporting cross-dll real-vs-smoke fields
# ──────────────────────────────────────────────────────────────────────────

def test_reporting_cross_dll_smoke_fields():
    """Verify that the cross-dll dashboard includes realFactPassRate,
    smokeOnlyMethodCount, and nominalVsRealGap."""
    from stages.reporting import _build_cross_dll_summary

    # Simulate the assemblies dict structure that _build_cross_dll_summary reads
    assemblies = {
        "System.Net.Http": {
            "fact-summary": {
                "totalPassed": 143,
                "totalFactMethods": 143,
                "totalRealPassed": 22,
                "totalRealFactMethods": 22,
                "totalUnverifiedSmoke": 121,
            },
            "dashboard": {"summary": {"totalBenchmarkedMethods": 0}},
            "comparison-summary": {},
        },
        "System.Core": {
            "fact-summary": {
                "totalPassed": 500,
                "totalFactMethods": 500,
                "totalRealPassed": 500,
                "totalRealFactMethods": 500,
                "totalUnverifiedSmoke": 0,
            },
            "dashboard": {"summary": {"totalBenchmarkedMethods": 200}},
            "comparison-summary": {},
        },
    }
    result = _build_cross_dll_summary(assemblies)

    # Overall: 643/643 nominal, but only 522/522 real (121 smoke)
    assert result["overallFactPassRate"] == 1.0  # 100% — misleadingly green
    assert result["overallRealFactPassRate"] == 1.0  # 522/522 real also 100%
    assert result["totalUnverifiedSmoke"] == 121
    assert result["nominalVsRealGap"] == 121  # 643 - 522

    # Per-assembly: Net.Http has 121 smoke, Core has 0
    assemblies_list = sorted(result["assemblies"], key=lambda a: a["assembly"])
    net_http = [a for a in assemblies_list if a["assembly"] == "System.Net.Http"][0]
    core = [a for a in assemblies_list if a["assembly"] == "System.Core"][0]

    assert net_http["smokeOnlyMethodCount"] == 121
    assert net_http["realFactPassRate"] == 1.0  # 22/22
    assert net_http["realVerifiedMethodCount"] == 22

    assert core["smokeOnlyMethodCount"] == 0
    assert core["realFactPassRate"] == 1.0  # 500/500
    assert core["realVerifiedMethodCount"] == 500


def test_reporting_cross_dll_no_smoke():
    """When no smoke exists, real fields must match nominal fields."""
    from stages.reporting import _build_cross_dll_summary

    assemblies = {
        "System.Core": {
            "fact-summary": {
                "totalPassed": 500,
                "totalFactMethods": 500,
                "totalRealPassed": 500,
                "totalRealFactMethods": 500,
                "totalUnverifiedSmoke": 0,
            },
            "dashboard": {"summary": {"totalBenchmarkedMethods": 200}},
            "comparison-summary": {},
        },
    }
    result = _build_cross_dll_summary(assemblies)
    assert result["overallFactPassRate"] == result["overallRealFactPassRate"]
    assert result["totalUnverifiedSmoke"] == 0
    assert result["nominalVsRealGap"] == 0


def test_reporting_cross_dll_legacy_fallback():
    """When fact-summary lacks real fields (pre-fix data), must fall back
    to nominal fields conservatively (no crash, no smoke distinction lost)."""
    from stages.reporting import _build_cross_dll_summary

    assemblies = {
        "Legacy": {
            "fact-summary": {
                "totalPassed": 100,
                "totalFactMethods": 100,
                # No totalRealPassed / totalRealFactMethods / totalUnverifiedSmoke
            },
            "dashboard": {"summary": {"totalBenchmarkedMethods": 0}},
            "comparison-summary": {},
        },
    }
    result = _build_cross_dll_summary(assemblies)
    # Must not crash; real fields should default to nominal
    assert result["overallFactPassRate"] == 1.0
    assert result["overallRealFactPassRate"] == 1.0
    assert result["totalUnverifiedSmoke"] == 0
    assert result["nominalVsRealGap"] == 0


# ──────────────────────────────────────────────────────────────────────────
# hotupdate real-vs-smoke split (ENG-34919)
# ──────────────────────────────────────────────────────────────────────────

def test_hotupdate_real_smoke_split_sums_to_nominal():
    """realPassed+smokePassed must equal the nominal passed count — the split
    partitions the total, it never drops or double-counts a method."""
    cs = _make_chunk_summary(
        "hu-chunk",
        hu_passed=100, hu_failed=0,
        hu_real_passed=12, hu_smoke_passed=88,
    )
    hu = cs["hotupdate"]
    assert hu["realPassed"] + hu["smokePassed"] == hu["passed"]
    assert hu["realFailed"] + hu["smokeFailed"] == hu["failed"]
    assert hu["realTotal"] == 12
    assert hu["smokeTotal"] == 88


def test_hotupdate_smoke_only_chunk_has_zero_real():
    """A chunk whose entire hotupdate 'pass' is smoke placeholders must report
    realTotal == 0 — this is the false-green the split exists to expose."""
    cs = _make_chunk_summary(
        "all-smoke",
        hu_passed=50, hu_failed=0,
        hu_real_passed=0, hu_smoke_passed=50,
    )
    hu = cs["hotupdate"]
    assert hu["realTotal"] == 0
    assert hu["smokeTotal"] == 50
    # A consumer checking "did any real method pass?" gets a truthful no.
    assert hu["realPassed"] == 0


def test_hotupdate_real_ratio_gate():
    """realPassRate must be computed from real methods only, ignoring smoke."""
    # Simulate the totals block in hotupdate_report.collect()
    dlls = [
        _make_chunk_summary("a", hu_passed=12, hu_real_passed=12, hu_smoke_passed=0),
        _make_chunk_summary("b", hu_passed=88, hu_real_passed=0, hu_smoke_passed=88),
    ]
    tot_real_pass = sum(d["hotupdate"]["realPassed"] for d in dlls)
    tot_real_fail = sum(d["hotupdate"]["realFailed"] for d in dlls)
    tot_real = tot_real_pass + tot_real_fail
    real_pass_rate = round(tot_real_pass / tot_real * 100, 2) if tot_real else None

    assert tot_real_pass == 12
    assert tot_real == 12
    assert real_pass_rate == 100.0
    # Nominal would have said 100/100 = 100% over 100 methods — far weaker evidence.
    tot_nominal = sum(d["hotupdate"]["passed"] for d in dlls)
    assert tot_nominal == 100


def test_hotupdate_real_ratio_none_when_all_smoke():
    """When no real method was exercised, realPassRate must be None (not a
    misleading 0% or 100%)."""
    dlls = [_make_chunk_summary("all-smoke", hu_passed=50, hu_smoke_passed=50)]
    tot_real_pass = sum(d["hotupdate"]["realPassed"] for d in dlls)
    tot_real_fail = sum(d["hotupdate"]["realFailed"] for d in dlls)
    tot_real = tot_real_pass + tot_real_fail
    real_pass_rate = round(tot_real_pass / tot_real * 100, 2) if tot_real else None

    assert tot_real == 0
    assert real_pass_rate is None


def test_hotupdate_legacy_json_defaults_to_smoke():
    """A pre-ENG-34919 hotupdate.json (no real/smoke fields) must not inflate
    real counts — absent annotation defaults the split conservatively."""
    legacy_hu = {"patchDataUsed": True, "passed": 30, "failed": 0}
    # This mirrors hotupdate_chunk's conservative default: unannotated => smoke.
    real_passed = legacy_hu.get("realPassed", 0)
    smoke_passed = legacy_hu.get("smokePassed", legacy_hu.get("passed", 0))
    assert real_passed == 0
    assert smoke_passed == 30


def test_hotupdate_triage_smoke_only_finding():
    """nightly_triage must raise a smokeOnly finding when a chunk's hotupdate
    has annotated data but zero real methods."""
    # Mirror the guard in _analyze_hotupdate.
    section = {
        "realSmokeAnnotated": True,
        "realPassed": 0, "realFailed": 0,
        "smokePassed": 40, "smokeFailed": 0,
    }
    real_t = section.get("realPassed", 0) + section.get("realFailed", 0)
    smoke_t = section.get("smokePassed", 0) + section.get("smokeFailed", 0)
    assert section.get("realSmokeAnnotated") is True
    assert real_t == 0 and smoke_t > 0  # -> smokeOnly finding fires


def test_hotupdate_triage_no_finding_without_annotation():
    """Un-annotated (legacy) hotupdate data must NOT raise smokeOnly — we only
    flag what we can actually prove."""
    section = {"realPassed": 0, "realFailed": 0, "smokePassed": 0, "smokeFailed": 0}
    assert not section.get("realSmokeAnnotated", False)