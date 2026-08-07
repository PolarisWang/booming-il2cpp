"""Tests for verification.benchmark_diff — regression detection logic."""

from pathlib import Path
import sys

_HERE = Path(__file__).resolve().parent
_VERIFICATION = _HERE.parent
from verification._path import foundation_root
_FOUNDATION_DLL = foundation_root()
if str(_FOUNDATION_DLL) not in sys.path:
    sys.path.insert(0, str(_FOUNDATION_DLL))

from verification.benchmark_diff import _diff_per_method, _diff_aggregate, _build_method_map


class TestDiffPerMethod:
    """Core regression classification logic — threshold-based delta detection."""

    def test_no_changes(self):
        baseline = {
            "m1": {"chaosAotVsNet8Pct": 50.0},
            "m2": {"chaosAotVsNet8Pct": 30.0},
        }
        target = {
            "m1": {"chaosAotVsNet8Pct": 50.0},
            "m2": {"chaosAotVsNet8Pct": 30.0},
        }
        r = _diff_per_method(baseline, target, threshold=10)
        assert r["summary"]["regressed"] == 0
        assert r["summary"]["improved"] == 0
        assert r["summary"]["unchanged"] == 2

    def test_regression_detected(self):
        baseline = {"m1": {"chaosAotVsNet8Pct": 50.0}}
        target = {"m1": {"chaosAotVsNet8Pct": 30.0}}  # dropped 20pp
        r = _diff_per_method(baseline, target, threshold=10)
        assert r["summary"]["regressed"] == 1
        assert r["summary"]["improved"] == 0
        assert r["details"]["regressed"][0]["delta"] == -20.0

    def test_improvement_detected(self):
        baseline = {"m1": {"chaosAotVsNet8Pct": 20.0}}
        target = {"m1": {"chaosAotVsNet8Pct": 45.0}}  # improved 25pp
        r = _diff_per_method(baseline, target, threshold=10)
        assert r["summary"]["improved"] == 1
        assert r["summary"]["regressed"] == 0
        assert r["details"]["improved"][0]["delta"] == 25.0

    def test_below_threshold_not_counted(self):
        baseline = {"m1": {"chaosAotVsNet8Pct": 50.0}}
        target = {"m1": {"chaosAotVsNet8Pct": 44.0}}  # -6pp, threshold=10
        r = _diff_per_method(baseline, target, threshold=10)
        assert r["summary"]["regressed"] == 0
        assert r["summary"]["unchanged"] == 1

    def test_new_methods(self):
        baseline = {}
        target = {"m1": {"chaosAotVsNet8Pct": 50.0}}
        r = _diff_per_method(baseline, target, threshold=10)
        assert r["summary"]["new"] == 1
        assert r["summary"]["regressed"] == 0

    def test_missing_methods(self):
        baseline = {"m1": {"chaosAotVsNet8Pct": 50.0}}
        target = {}
        r = _diff_per_method(baseline, target, threshold=10)
        assert r["summary"]["missing"] == 1
        assert r["summary"]["regressed"] == 0

    def test_none_values_skipped(self):
        baseline = {"m1": {"chaosAotVsNet8Pct": None}}
        target = {"m1": {"chaosAotVsNet8Pct": 50.0}}
        r = _diff_per_method(baseline, target, threshold=10)
        assert r["summary"]["unchanged"] == 1  # counted as unchanged (can't diff None)

    def test_different_thresholds(self):
        baseline = {"m1": {"chaosAotVsNet8Pct": 50.0}}
        target = {"m1": {"chaosAotVsNet8Pct": 45.0}}  # -5pp
        r5 = _diff_per_method(baseline, target, threshold=5)
        r10 = _diff_per_method(baseline, target, threshold=10)
        assert r5["summary"]["regressed"] == 1  # 5 >= 5
        assert r10["summary"]["regressed"] == 0  # 5 < 10


class TestBuildMethodMap:
    """Method map construction from comparison JSON structure."""

    def test_build_map_from_perchunk(self):
        comparison = {
            "perChunk": [
                {
                    "slug": "system",
                    "aggregate": {
                        "chaosAotVsNet8Pct": {"mean": 45.0},
                        "chaosJitVsNet8Pct": {"mean": 55.0},
                        "net10VsNet8Pct": {"mean": 50.0},
                    },
                },
            ]
        }
        m = _build_method_map(comparison)
        assert "system" in m
        assert m["system"]["chaosAotVsNet8Pct"] == 45.0

    def test_build_map_empty(self):
        assert _build_method_map({}) == {}

    def test_build_map_missing_aggregate(self):
        comparison = {"perChunk": [{"slug": "system"}]}
        m = _build_method_map(comparison)
        assert m["system"]["chaosAotVsNet8Pct"] is None


class TestDiffAggregate:
    """Chunk-level aggregate diff (used as fallback when per-method data unavailable)."""

    def test_aggregate_diff_basic(self):
        baseline = {
            "perChunk": [
                {"slug": "system", "aggregate": {"chaosAotVsNet8Pct": {"mean": 50.0}}},
            ]
        }
        target = {
            "perChunk": [
                {"slug": "system", "aggregate": {"chaosAotVsNet8Pct": {"mean": 30.0}}},
            ]
        }
        r = _diff_aggregate(baseline, target)
        assert len(r["perChunk"]) == 1
        assert r["perChunk"][0]["deltaChaosAotPct"] == -20.0

    def test_aggregate_diff_new_chunk(self):
        baseline = {"perChunk": []}
        target = {
            "perChunk": [
                {"slug": "new-chunk", "aggregate": {"chaosAotVsNet8Pct": {"mean": 60.0}}},
            ]
        }
        r = _diff_aggregate(baseline, target)
        # New chunks: baseline_chaosAotMeanPct is None
        new = [d for d in r["perChunk"] if d["baselineChaosAotMeanPct"] is None]
        assert len(new) == 1
        assert new[0]["slug"] == "new-chunk"
