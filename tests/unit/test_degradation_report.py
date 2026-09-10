"""Tests for reporting/degradation_report.py

TARGET: degradation_report.classify_report

The load-bearing behaviour: a metric moving in the bad direction past its hard
threshold must be classified hard; the same movement below it must be soft; and
movement in the good direction must not be classified at all.  Direction is
per-metric — fact_pass_rate is higher-is-better, bench_duration is not — so
getting that backwards would silently invert the gate.
"""
from __future__ import annotations

import importlib.util
import unittest
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parents[2]
SCRIPT = REPO_ROOT / "tests" / "e2e" / "verification" / "reporting" / "degradation_report.py"

_loader = importlib.util.spec_from_file_location("degradation_report", str(SCRIPT))
_mod = importlib.util.module_from_spec(_loader)
_loader.loader.exec_module(_mod)


class ClassifyReportTests(unittest.TestCase):

    def _rep(self, regressions=None, improvements=None, new_failures=None):
        return _mod.classify_report({
            "regressions": regressions or [],
            "improvements": improvements or [],
            "newFailures": new_failures or [],
        })

    # ── fact_pass_rate: higher is better ──

    def test_fact_drop_past_hard(self):
        r = self._rep([{"assembly": "A", "slug": "s", "metric": "fact_pass_rate",
                        "delta": -8.0}])
        self.assertEqual(r["overall"], "hard")
        self.assertEqual(len(r["hard"]), 1)

    def test_fact_drop_below_hard_is_soft(self):
        r = self._rep([{"assembly": "A", "slug": "s", "metric": "fact_pass_rate",
                        "delta": -2.0}])
        self.assertEqual(r["overall"], "soft")
        self.assertEqual(len(r["soft"]), 1)
        self.assertEqual(len(r["hard"]), 0)

    def test_fact_rise_is_not_regression(self):
        """A positive fact delta is an improvement — must not be classified."""
        r = self._rep([{"assembly": "A", "slug": "s", "metric": "fact_pass_rate",
                        "delta": 10.0}])
        self.assertEqual(r["overall"], "ok")
        self.assertEqual(r["counts"]["hard"] + r["counts"]["soft"], 0)

    # ── bench_duration: higher is worse ──

    def test_duration_increase_past_hard(self):
        r = self._rep([{"assembly": "A", "slug": "s", "metric": "bench_duration",
                        "delta": 25.0}])
        self.assertEqual(r["overall"], "hard")

    def test_duration_increase_below_hard_is_soft(self):
        r = self._rep([{"assembly": "A", "slug": "s", "metric": "bench_duration",
                        "delta": 8.0}])
        self.assertEqual(r["overall"], "soft")

    def test_duration_decrease_is_not_regression(self):
        r = self._rep([{"assembly": "A", "slug": "s", "metric": "bench_duration",
                        "delta": -30.0}])
        self.assertEqual(r["overall"], "ok")

    # ── coverage_gap ──

    def test_coverage_gap_widening_is_hard(self):
        r = self._rep([{"assembly": "A", "slug": "s", "metric": "coverage_gap",
                        "delta": 2.0}])
        self.assertEqual(r["overall"], "hard")

    # ── new failures ──

    def test_new_failures_always_hard(self):
        r = self._rep(new_failures=[{"assembly": "A", "slug": "s"}])
        self.assertEqual(r["overall"], "hard")
        self.assertEqual(r["counts"]["newFailures"], 1)

    # ── overall ──

    def test_clean_report_is_ok(self):
        r = self._rep()
        self.assertEqual(r["overall"], "ok")
        self.assertEqual(r["counts"]["hard"], 0)
        self.assertEqual(r["counts"]["soft"], 0)

    def test_hard_dominates_soft(self):
        r = self._rep([
            {"assembly": "A", "slug": "s1", "metric": "fact_pass_rate", "delta": -6.0},
            {"assembly": "A", "slug": "s2", "metric": "fact_pass_rate", "delta": -1.0},
        ])
        self.assertEqual(r["overall"], "hard")
        self.assertEqual(r["counts"]["hard"], 1)
        self.assertEqual(r["counts"]["soft"], 1)

    def test_unknown_metric_ignored(self):
        r = self._rep([{"assembly": "A", "slug": "s", "metric": "made_up",
                        "delta": 999.0}])
        self.assertEqual(r["overall"], "ok")

    def test_none_delta_ignored(self):
        r = self._rep([{"assembly": "A", "slug": "s", "metric": "fact_pass_rate",
                        "delta": None}])
        self.assertEqual(r["overall"], "ok")

    def test_markdown_renders(self):
        r = self._rep([
            {"assembly": "System.X", "slug": "ns", "metric": "fact_pass_rate",
             "before": 90.0, "after": 80.0, "delta": -10.0},
        ])
        md = _mod.render_markdown(r)
        self.assertIn("Nightly 退化报告", md)
        self.assertIn("HARD", md)
        self.assertIn("System.X", md)


if __name__ == "__main__":
    unittest.main()
