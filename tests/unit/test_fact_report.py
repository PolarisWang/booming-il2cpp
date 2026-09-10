"""Tests for tests/e2e/verification/reporting/fact_report.py

TARGET: fact_report.collect and fact_report.render_markdown

Verifies:
  - normal DLL with a fact-summary.json
  - uncovered chunk (dir present, not in summary)
  - vanished chunk (in summary, dir absent)
  - stale detection by age
  - DLL with no report at all
  - markdown rendering structure
"""
from __future__ import annotations

import importlib.util
import json
import tempfile
import unittest
from pathlib import Path
from datetime import datetime, timezone, timedelta

REPO_ROOT = Path(__file__).resolve().parents[2]
SCRIPT = REPO_ROOT / "tests" / "e2e" / "verification" / "reporting" / "fact_report.py"

_loader = importlib.util.spec_from_file_location("fact_report", str(SCRIPT))
_mod = importlib.util.module_from_spec(_loader)
_loader.loader.exec_module(_mod)


class FactReportTests(unittest.TestCase):

    def setUp(self):
        self.td = tempfile.TemporaryDirectory()
        self.root = Path(self.td.name)

    def tearDown(self):
        self.td.cleanup()

    def _make_dll(self, name: str, chunks: list[str],
                  reported_chunks: list[str] | None = None,
                  passed: int = 10, total: int = 10,
                  timestamp: str | None = None) -> Path:
        """Create a minimal DLL tree with fact-summary.json under _dll/reports/latest."""
        dll = self.root / name
        (dll / "chunks").mkdir(parents=True)
        for c in chunks:
            (dll / "chunks" / c).mkdir()
        report_dir = dll / "_dll" / "reports" / "latest"
        report_dir.mkdir(parents=True)

        reported = reported_chunks or chunks
        summary = {
            "assemblyName": name,
            "timestamp": timestamp or "2026-09-01T00:00:00",
            "totalChunks": len(reported),
            "totalPassed": passed,
            "totalFactMethods": total,
            "chunksWithValueWarnings": 0,
            "chunkSummaries": [
                {"slug": c, "fact": {"passed": passed // len(reported) if reported else 0,
                                       "total": total // len(reported) if reported else 0}}
                for c in reported
            ],
        }
        (report_dir / "fact-summary.json").write_text(
            json.dumps(summary), encoding="utf-8")
        return dll

    # ── tests ──

    def test_normal_dll(self):
        self._make_dll("System.A", ["ns1", "ns2"], passed=20, total=20,
                       timestamp="2026-09-01T00:00:00")
        r = _mod.collect(self.root, stale_days=14)
        d = r["dlls"][0]
        self.assertEqual(d["status"], "ok")
        self.assertEqual(d["passed"], 20)
        self.assertEqual(d["passRate"], 100.0)
        self.assertEqual(d["uncoveredChunks"], [])
        self.assertEqual(d["vanishedChunks"], [])
        self.assertFalse(d["stale"])

    def test_uncovered_chunk(self):
        dll = self._make_dll("System.A", ["ns1", "ns2"],
                             reported_chunks=["ns1"], passed=10, total=10)
        r = _mod.collect(self.root, stale_days=14)
        d = r["dlls"][0]
        self.assertEqual(d["uncoveredChunks"], ["ns2"])
        self.assertEqual(d["vanishedChunks"], [])

    def test_vanished_chunk(self):
        dll = self._make_dll("System.A", ["ns1"],
                             reported_chunks=["ns1", "ns2"], passed=10, total=10)
        r = _mod.collect(self.root, stale_days=14)
        d = r["dlls"][0]
        self.assertEqual(d["uncoveredChunks"], [])
        self.assertEqual(d["vanishedChunks"], ["ns2"])

    def test_stale_detection(self):
        old_ts = (datetime.now(timezone.utc) - timedelta(days=20)).isoformat()
        self._make_dll("System.A", ["ns1"], passed=10, total=10, timestamp=old_ts)
        r = _mod.collect(self.root, stale_days=14)
        self.assertTrue(r["dlls"][0]["stale"])
        self.assertGreater(r["dlls"][0]["ageDays"], 14)

    def test_dll_with_no_report(self):
        dll = self.root / "System.NoReport"
        (dll / "chunks" / "ns1").mkdir(parents=True)
        r = _mod.collect(self.root, stale_days=14)
        d = r["dlls"][0]
        self.assertEqual(d["status"], "no-report")
        self.assertEqual(d["diskChunks"], ["ns1"])

    def test_hephaestus_cache_filtered(self):
        """The pipeline records .hephaestus-cache as a chunk — it must not
        turn up as 'vanished' since it is not a testable chunk."""
        dll = self._make_dll("System.A", ["ns1"],
                             reported_chunks=[".hephaestus-cache", "ns1"],
                             passed=5, total=5)
        (dll / "chunks" / ".hephaestus-cache").mkdir(parents=True)
        r = _mod.collect(self.root, stale_days=14)
        d = r["dlls"][0]
        self.assertEqual(d["vanishedChunks"], [])
        self.assertEqual(d["uncoveredChunks"], [])

    def test_markdown_rendering(self):
        self._make_dll("System.X", ["ns1"], passed=10, total=10)
        r = _mod.collect(self.root, stale_days=14)
        md = _mod.render_markdown(r)
        self.assertIn("总量", md)
        self.assertIn("System.X", md)
        self.assertIn("通过率", md)
        self.assertIn("100.0%", md)

    def test_totals_aggregation(self):
        self._make_dll("System.A", ["ns1"], passed=10, total=20)
        self._make_dll("System.B", ["ns1", "ns2"], passed=30, total=30)
        r = _mod.collect(self.root, stale_days=14)
        t = r["totals"]
        self.assertEqual(t["dllCount"], 2)
        self.assertEqual(t["reportedDlls"], 2)
        self.assertEqual(t["factMethods"], 50)
        self.assertEqual(t["passed"], 40)
        self.assertEqual(t["failed"], 10)
        self.assertAlmostEqual(t["passRate"], 80.0)


if __name__ == "__main__":
    unittest.main()