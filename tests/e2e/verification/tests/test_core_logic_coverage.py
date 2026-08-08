"""Track 8 tests — core-logic unit coverage (previously zero-tested).

Adds unit tests for pure data-transform functions in the analysis modules that
had no direct coverage: gc_check.check_profile threshold/severity, and
perf_compare.compare_profiles regression detection. These are pure functions, so
they test cleanly with dict fixtures and no build artifacts.
"""
from __future__ import annotations

import sys
from pathlib import Path

import pytest

_HERE = Path(__file__).resolve().parent
for _d in (_HERE, _HERE.parent, _HERE.parent.parent):
    if str(_d) not in sys.path:
        sys.path.insert(0, str(_d))


def test_gc_check_flags_max_pause() -> None:
    from verification.analysis.gc_check import check_profile

    # Default max_gc_pause_ns is 5ms = 5_000_000ns. A 6ms pause must flag GC-01.
    result = check_profile([
        {"methodIndex": 0, "maxGcPauseNs": 6_000_000, "gcPauseCount": 0,
         "fastPathCount": 100, "slowPathCount": 0},
    ])
    checks = [i["check"] for i in result["issues"]]
    assert "GC-01" in checks
    assert result["status"] is not None


def test_gc_check_fast_path_below_threshold() -> None:
    from verification.analysis.gc_check import check_profile

    # fastPathRate 0.5 < default min_fast_path_rate 0.8 -> GC-03.
    result = check_profile([
        {"methodIndex": 1, "maxGcPauseNs": 0, "gcPauseCount": 0,
         "fastPathCount": 5, "slowPathCount": 5},
    ])
    checks = [i["check"] for i in result["issues"]]
    assert "GC-03" in checks


def test_gc_check_healthy_no_issues() -> None:
    from verification.analysis.gc_check import check_profile

    result = check_profile([
        {"methodIndex": 2, "maxGcPauseNs": 0, "gcPauseCount": 0,
         "fastPathCount": 100, "slowPathCount": 0},
    ])
    assert result["issues"] == []
    assert result["status"] == "healthy"


def test_gc_check_custom_threshold() -> None:
    from verification.analysis.gc_check import check_profile

    # Raise max_gc_pause_ns above 6ms so the same data no longer flags.
    result = check_profile(
        [{"methodIndex": 0, "maxGcPauseNs": 6_000_000, "gcPauseCount": 0,
          "fastPathCount": 100, "slowPathCount": 0}],
        thresholds={"max_gc_pause_ns": 10_000_000},
    )
    assert result["issues"] == []


def test_perf_compare_detects_regression() -> None:
    from verification.analysis.perf_compare import compare_profiles

    # Baseline fast_path 0.9 -> target 0.5 (lower is worse; higher_is_better=True
    # for fastPathRate) => regression.
    result = compare_profiles(
        {"metrics": {"fastPathRate": 0.9, "totalGcPauseNs": 1_000_000}},
        {"metrics": {"fastPathRate": 0.5, "totalGcPauseNs": 1_000_000}},
        threshold_pct=10.0,
    )
    assert any(r["metric"] == "fastPathRate" for r in result["regressions"])


def test_perf_compare_detects_improvement() -> None:
    from verification.analysis.perf_compare import compare_profiles

    # GC total pause lower is better: baseline 3s -> target 1s => improvement.
    result = compare_profiles(
        {"metrics": {"totalGcPauseNs": 3_000_000}},
        {"metrics": {"totalGcPauseNs": 1_000_000}},
        threshold_pct=10.0,
    )
    assert any(r["metric"] == "totalGcPauseNs" for r in result["improvements"])


def test_perf_baseline_establishes_metrics() -> None:
    from verification.analysis.perf_baseline import PerfBaseline
    import tempfile

    with tempfile.TemporaryDirectory() as td:
        base = Path(td)
        bl = PerfBaseline(results_base=base)
        path = bl.establish("CoreLib", "numerics", [
            {"gcPauseNs": 1_000_000, "maxGcPauseNs": 2_000_000,
             "nurseryAllocBytes": 4096, "allocCount": 42,
             "fastPathCount": 90, "slowPathCount": 10},
        ])
        assert path.exists()
        data = bl.get("CoreLib", "numerics")
        assert data is not None
        assert data["metrics"]["fastPathRate"] == pytest.approx(0.9)
        assert data["methodCount"] == 1
