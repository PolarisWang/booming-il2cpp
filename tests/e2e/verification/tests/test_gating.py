"""Tests for verification/stages/gating.py — classify_gate unified fact gate."""
from __future__ import annotations

import importlib.util, sys, pathlib

# Load gating module directly (avoids __init__ import cascade from regular import)
_h = pathlib.Path(__file__).resolve().parent.parent / "stages" / "gating.py"
_spec = importlib.util.spec_from_file_location("_gate_test", str(_h))
_g = importlib.util.module_from_spec(_spec); _spec.loader.exec_module(_g)
classify_gate = _g.classify_gate
GATE_REAL_RATIO = _g.GATE_REAL_RATIO


class TestClassifyGate:
    def test_pass_nominal(self):
        assert classify_gate({"realTotal": 100, "realPassed": 85, "total": 200, "passed": 200}) == "pass"

    def test_fail_under_ratio(self):
        assert classify_gate({"realTotal": 100, "realPassed": 5, "total": 200, "passed": 200}) == "fail"

    def test_skip_none(self):
        assert classify_gate(None) == "skip"

    def test_skip_empty(self):
        assert classify_gate({}) == "skip"

    def test_skip_zero_total(self):
        assert classify_gate({"total": 0}) == "skip"

    def test_fail_all_smoke(self):
        """realTotal=0 (all smoke) → fail even if nominal passed==total."""
        assert classify_gate({"total": 50, "passed": 50, "realTotal": 0, "realPassed": 0}) == "fail"

    def test_pass_exactly_at_threshold(self):
        """ratio == threshold should pass."""
        assert classify_gate({"realTotal": 100, "realPassed": 10, "total": 100, "passed": 100}) == "pass"

    def test_custom_threshold(self):
        assert classify_gate({"realTotal": 100, "realPassed": 90, "total": 100, "passed": 100}, min_ratio=0.95) == "fail"
        assert classify_gate({"realTotal": 100, "realPassed": 95, "total": 100, "passed": 100}, min_ratio=0.95) == "pass"

    def test_fallback_nominal(self):
        """No realTotal/realPassed fields → fallback to total/passed."""
        assert classify_gate({"total": 100, "passed": 5}) == "fail"  # all nominal-fail still fail
        assert classify_gate({"total": 100, "passed": 95}) == "pass"

    def test_ratio_helper(self):
        assert _g.fact_real_ratio({"realTotal": 100, "realPassed": 25}) == 0.25
        assert _g.fact_real_ratio({"total": 50, "passed": 50, "realTotal": 0, "realPassed": 0}) == 0.0
        assert _g.fact_real_ratio(None) == 0.0