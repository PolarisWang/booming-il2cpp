"""Unit tests for benchmark_comparison.py"""
from __future__ import annotations

import importlib.util
from pathlib import Path

import pytest

REPO_ROOT = Path(__file__).resolve().parents[3]
_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "benchmark_comparison.py"


def _load():
    spec = importlib.util.spec_from_file_location("benchmark_comparison", _MODULE_PATH)
    assert spec is not None and spec.loader is not None
    mod = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(mod)  # type: ignore[union-attr]
    return mod


_mod = _load()
compute_comparison = _mod.compute_comparison
evaluate_targets = _mod.evaluate_targets


class TestComputeComparison:
    def test_normal_three_way(self):
        managed = {"meanDurationMs": 12.0, "opsPerSecond": 83333}
        native = {"meanDurationMs": 2.0, "opsPerSecond": 500000}
        interpreter = {"meanDurationMs": 120.0, "opsPerSecond": 8333}
        result = compute_comparison(managed, native, interpreter)

        # native vs managed meanDuration: 2.0 / 12.0 = 0.17
        assert result["nativeVsManaged"]["meanDurationMs"] == pytest.approx(0.17, abs=0.01)
        # interpreter vs managed: 120 / 12 = 10.0
        assert result["interpreterVsManaged"]["meanDurationMs"] == pytest.approx(10.0, abs=0.01)
        # nativeSpeedup (managed / native latency): 12 / 2 = 6.0
        assert result["nativeSpeedup"] == pytest.approx(6.0, abs=0.01)
        # interpreterOverhead (interp / managed latency): 120 / 12 = 10.0
        assert result["interpreterOverhead"] == pytest.approx(10.0, abs=0.01)

    def test_missing_interpreter(self):
        managed = {"meanDurationMs": 12.0}
        native = {"meanDurationMs": 2.0}
        result = compute_comparison(managed, native, None)

        assert result["interpreterVsManaged"]["meanDurationMs"] is None
        assert result["interpreterOverhead"] is None
        assert result["nativeSpeedup"] == pytest.approx(6.0, abs=0.01)

    def test_missing_all(self):
        result = compute_comparison(None, None, None)
        assert result["nativeSpeedup"] is None
        assert result["interpreterOverhead"] is None

    def test_division_by_zero(self):
        managed = {"meanDurationMs": 0.0}
        native = {"meanDurationMs": 2.0}
        result = compute_comparison(managed, native, None)
        assert result["nativeVsManaged"]["meanDurationMs"] == "N/A"
        assert result["interpreterOverhead"] is None  # interp is None

    def test_ratio_precision(self):
        managed = {"meanDurationMs": 10.0}
        native = {"meanDurationMs": 3.0}
        result = compute_comparison(managed, native, None)
        # 3/10 = 0.30 (rounded to 2 decimal places)
        assert result["nativeVsManaged"]["meanDurationMs"] == 0.30


class TestEvaluateTargets:
    def test_pass_both(self):
        comparison = {
            "nativeSpeedup": 5.9,
            "interpreterOverhead": 12.7,
        }
        verdict = evaluate_targets(comparison)
        assert verdict["overallPass"] is True
        assert verdict["verdicts"]["nativeVsManaged"]["pass"] is True
        assert verdict["verdicts"]["interpreterVsManaged"]["pass"] is True

    def test_fail_native_speedup(self):
        comparison = {"nativeSpeedup": 1.5, "interpreterOverhead": 10.0}
        verdict = evaluate_targets(comparison)
        assert verdict["verdicts"]["nativeVsManaged"]["pass"] is False
        assert verdict["overallPass"] is False

    def test_fail_interpreter_overhead(self):
        comparison = {"nativeSpeedup": 5.0, "interpreterOverhead": 75.0}
        verdict = evaluate_targets(comparison)
        assert verdict["verdicts"]["interpreterVsManaged"]["pass"] is False
        assert verdict["overallPass"] is False

    def test_no_data(self):
        comparison = {"nativeSpeedup": None, "interpreterOverhead": None}
        verdict = evaluate_targets(comparison)
        # Both are "no data" (pass=None), overallPass should still be True
        # (no failures when there's no data)
        assert verdict["overallPass"] is True
        assert verdict["verdicts"]["nativeVsManaged"]["pass"] is None
