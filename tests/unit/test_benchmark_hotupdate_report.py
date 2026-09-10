"""Tests for benchmark_report.py and hotupdate_report.py

TARGET: benchmark_report.collect / hotupdate_report.collect

Verified via synthetic DLL trees:
  - benchmark: method/iteration aggregation, stale detection, missing file skip
  - hotupdate: nested hotupdate field extraction, .hephaestus-cache filter,
    chunks with no hotupdate data omitted, patch-failure/crash counting
"""
from __future__ import annotations

import importlib.util
import json
import tempfile
import unittest
from datetime import datetime, timezone, timedelta
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parents[2]
RPT = REPO_ROOT / "tests" / "e2e" / "verification" / "reporting"


def _load(name: str):
    spec = importlib.util.spec_from_file_location(name, str(RPT / f"{name}.py"))
    m = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(m)
    return m


benchmark_report = _load("benchmark_report")
hotupdate_report = _load("hotupdate_report")


def _write_summary(root: Path, dll: str, payload: dict) -> None:
    d = root / dll / "_dll" / "reports" / "latest"
    d.mkdir(parents=True, exist_ok=True)
    (d / "fact-summary.json").write_text(json.dumps(payload), encoding="utf-8")


class BenchmarkReportTests(unittest.TestCase):

    def setUp(self):
        self.td = tempfile.TemporaryDirectory()
        self.root = Path(self.td.name)

    def tearDown(self):
        self.td.cleanup()

    def _bench(self, root, dll, methods, iters, mean_ms=None, ts="2026-09-01T00:00:00"):
        d = root / dll / "_dll" / "reports" / "latest"
        d.mkdir(parents=True, exist_ok=True)
        (d / "benchmark-summary.json").write_text(json.dumps({
            "assemblyName": dll,
            "timestamp": ts,
            "totalChunks": 1,
            "totalBenchmarkedMethods": methods,
            "aggregatePerformance": {"meanDurationMs": mean_ms, "meanOpsPerSecond": 100.0,
                                       "aggregateMeanCv": 0.1, "totalOutliers": 2},
            "chunkSummaries": [{"slug": "ns1",
                                 "benchmark": {"methodCount": methods, "iterations": iters}}],
        }), encoding="utf-8")

    def test_collects_benchmark(self):
        self._bench(self.root, "System.A", methods=10, iters=100, mean_ms=1.5)
        r = benchmark_report.collect(self.root, stale_days=14)
        self.assertEqual(r["totals"]["dllCount"], 1)
        self.assertEqual(r["totals"]["totalBenchmarkedMethods"], 10)
        self.assertEqual(r["totals"]["totalIterations"], 100)

    def test_aggregates_multiple_dlls(self):
        self._bench(self.root, "System.A", methods=10, iters=100, mean_ms=1.0)
        self._bench(self.root, "System.B", methods=20, iters=200, mean_ms=3.0)
        r = benchmark_report.collect(self.root, stale_days=14)
        self.assertEqual(r["totals"]["totalBenchmarkedMethods"], 30)
        self.assertEqual(r["totals"]["totalIterations"], 300)
        self.assertAlmostEqual(r["totals"]["meanDurationMs"], 2.0)

    def test_missing_file_skipped(self):
        (self.root / "System.NoBench").mkdir()
        r = benchmark_report.collect(self.root, stale_days=14)
        self.assertEqual(r["dlls"], [])

    def test_stale_detection(self):
        old = (datetime.now(timezone.utc) - timedelta(days=30)).isoformat()
        self._bench(self.root, "System.A", methods=1, iters=1, mean_ms=1.0, ts=old)
        r = benchmark_report.collect(self.root, stale_days=14)
        self.assertTrue(r["dlls"][0]["stale"])
        self.assertEqual(r["totals"]["staleDlls"], 1)

    def test_markdown_renders(self):
        self._bench(self.root, "System.A", methods=5, iters=50, mean_ms=2.0)
        r = benchmark_report.collect(self.root, stale_days=14)
        md = benchmark_report.render_markdown(r)
        self.assertIn("Benchmark 报告", md)
        self.assertIn("System.A", md)


class HotupdateReportTests(unittest.TestCase):

    def setUp(self):
        self.td = tempfile.TemporaryDirectory()
        self.root = Path(self.td.name)

    def tearDown(self):
        self.td.cleanup()

    def _hu(self, dll, chunks, ts="2026-09-01T00:00:00", chunks_list=None):
        _write_summary(self.root, dll, {
            "assemblyName": dll,
            "timestamp": ts,
            "chunkSummaries": chunks_list if chunks_list is not None else [
                {"slug": slug, "hotupdate": hu} for slug, hu in chunks
            ],
        })

    def test_extracts_hotupdate(self):
        self._hu("System.A", [("ns1", {"passed": 10, "failed": 0,
                                        "semanticChangedCount": 2,
                                        "patchFailed": False, "crash": False})])
        r = hotupdate_report.collect(self.root, stale_days=14)
        self.assertEqual(r["totals"]["dllCount"], 1)
        self.assertEqual(r["totals"]["totalPassed"], 10)
        self.assertEqual(r["totals"]["semanticChangedTotal"], 2)

    def test_hephaestus_cache_filtered(self):
        self._hu("System.A", [
            (".hephaestus-cache", {"passed": None, "failed": None}),
            ("ns1", {"passed": 5, "failed": 0, "patchFailed": False, "crash": False}),
        ])
        r = hotupdate_report.collect(self.root, stale_days=14)
        self.assertEqual(r["totals"]["totalPassed"], 5)
        self.assertEqual(r["dlls"][0]["chunksWithHotupdate"], 1)

    def test_chunk_without_hotupdate_omitted(self):
        self._hu("System.A", [], chunks_list=[
            {"slug": "ns1"},  # no hotupdate key at all
        ])
        r = hotupdate_report.collect(self.root, stale_days=14)
        # No chunk had usable hotupdate data -> DLL not reported
        self.assertEqual(r["dlls"], [])

    def test_patch_failure_and_crash_counted(self):
        self._hu("System.A", [
            ("ns1", {"passed": 5, "failed": 0, "patchFailed": True, "crash": False}),
            ("ns2", {"passed": 3, "failed": 0, "patchFailed": False, "crash": True}),
        ])
        r = hotupdate_report.collect(self.root, stale_days=14)
        d = r["dlls"][0]
        self.assertEqual(d["patchFailedChunks"], 1)
        self.assertEqual(d["crashChunks"], 1)
        self.assertEqual(r["totals"]["dllsWithPatchFailures"], 1)
        self.assertEqual(r["totals"]["dllsWithCrashes"], 1)

    def test_stale_detection(self):
        old = (datetime.now(timezone.utc) - timedelta(days=50)).isoformat()
        self._hu("System.A", [("ns1", {"passed": 1, "failed": 0})], ts=old)
        r = hotupdate_report.collect(self.root, stale_days=14)
        self.assertTrue(r["dlls"][0]["stale"])

    def test_markdown_renders(self):
        self._hu("System.A", [("ns1", {"passed": 1, "failed": 0})])
        r = hotupdate_report.collect(self.root, stale_days=14)
        md = hotupdate_report.render_markdown(r)
        self.assertIn("Hotupdate 报告", md)
        self.assertIn("System.A", md)


if __name__ == "__main__":
    unittest.main()
