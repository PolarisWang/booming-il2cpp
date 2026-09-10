"""Tests for .github/scripts/stage_pages_reports.py

TARGET: stage_pages_reports.stage

The contract that matters: a freshly generated report must beat the committed
snapshot, and a report the run did not regenerate must survive from the
committed snapshot.  Getting this backwards silently publishes stale data.
"""
from __future__ import annotations

import importlib.util
import tempfile
import unittest
from pathlib import Path
from unittest import mock

REPO_ROOT = Path(__file__).resolve().parents[2]
SCRIPT = REPO_ROOT / ".github" / "scripts" / "stage_pages_reports.py"

_loader = importlib.util.spec_from_file_location("stage_pages_reports", str(SCRIPT))
_mod = importlib.util.module_from_spec(_loader)
_loader.loader.exec_module(_mod)


class StagePagesReportsTests(unittest.TestCase):

    def setUp(self):
        self.td = tempfile.TemporaryDirectory()
        root = Path(self.td.name)

        self.committed = root / "committed"
        (self.committed / "foundation-dll-audit").mkdir(parents=True)
        (self.committed / "foundation-dll-audit" / "dashboard.html").write_text(
            "COMMITTED", encoding="utf-8")
        (self.committed / "foundation-dll-audit" / "only-committed.json").write_text(
            "{}", encoding="utf-8")
        (self.committed / "benchmark").mkdir(parents=True)
        (self.committed / "benchmark" / "dashboard.html").write_text(
            "BENCH-COMMITTED", encoding="utf-8")

        self.fresh = root / "fresh" / "verification"
        (self.fresh / "foundation-dll-audit").mkdir(parents=True)
        (self.fresh / "foundation-dll-audit" / "dashboard.html").write_text(
            "FRESH", encoding="utf-8")

        self.site = root / "site"

    def tearDown(self):
        self.td.cleanup()

    def _stage(self, report_dirs=None):
        with mock.patch.object(_mod, "COMMITTED_REPORTS", self.committed):
            return _mod.stage(self.site, report_dirs or [], dry_run=False)

    def test_fresh_overrides_committed(self):
        self.assertEqual(self._stage([self.fresh.parent]), 0)
        got = (self.site / "verification" / "foundation-dll-audit" / "dashboard.html")
        self.assertEqual(got.read_text(encoding="utf-8"), "FRESH")

    def test_committed_only_files_survive(self):
        self._stage([self.fresh.parent])
        kept = self.site / "verification" / "foundation-dll-audit" / "only-committed.json"
        self.assertTrue(kept.is_file())

    def test_untouched_report_served_from_committed(self):
        self._stage([self.fresh.parent])
        bench = self.site / "verification" / "benchmark" / "dashboard.html"
        self.assertEqual(bench.read_text(encoding="utf-8"), "BENCH-COMMITTED")

    def test_missing_report_dir_is_skipped(self):
        rc = self._stage([Path(self.td.name) / "does-not-exist"])
        self.assertEqual(rc, 0)
        # committed snapshot still published
        self.assertTrue((self.site / "verification" / "benchmark").is_dir())

    def test_dry_run_writes_nothing(self):
        with mock.patch.object(_mod, "COMMITTED_REPORTS", self.committed):
            _mod.stage(self.site, [], dry_run=True)
        self.assertFalse(self.site.exists())


if __name__ == "__main__":
    unittest.main()
