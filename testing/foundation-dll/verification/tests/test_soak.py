"""Tests for soak/ — config loading, metric parsing, degradation detection."""

import json
from pathlib import Path

from verification.soak.report import (
    SoakSnapshot,
    compute_degradation,
)


class TestDegradationDetection:
    def test_insufficient_samples(self):
        snapshots = []
        for i in range(3):
            snapshots.append(SoakSnapshot(rss_mb=10.0 + i))
        check = compute_degradation(snapshots, "rss_mb")
        assert check.passed is True
        assert "insufficient samples" in check.warning

    def test_rss_leak_detected(self):
        snapshots = []
        for i in range(6):
            snapshots.append(SoakSnapshot(rss_mb=100.0 + i * 10.0))
        check = compute_degradation(snapshots, "rss_mb")
        assert check.passed is False
        assert "memory leak" in check.warning

    def test_rss_stable(self):
        snapshots = []
        for _ in range(6):
            snapshots.append(SoakSnapshot(rss_mb=100.0))
        check = compute_degradation(snapshots, "rss_mb")
        assert check.passed is True
        assert check.slope == 0.0

    def test_failure_rate_rising(self):
        snapshots = []
        for i in range(6):
            snapshots.append(SoakSnapshot(tests_failed=i))
        check = compute_degradation(snapshots, "tests_failed")
        assert check.passed is False
        assert "failure rate" in check.warning

    def test_empty_snapshots(self):
        check = compute_degradation([], "rss_mb")
        assert check.passed is True

    def test_vms_degradation(self):
        snapshots = []
        for i in range(6):
            snapshots.append(SoakSnapshot(vms_mb=200.0 + i * 20.0))
        check = compute_degradation(snapshots, "vms_mb")
        assert check.passed is False

    def test_cpu_metric_no_threshold(self):
        snapshots = []
        for i in range(6):
            snapshots.append(SoakSnapshot(cpu_percent=50.0 + i * 5.0))
        check = compute_degradation(snapshots, "cpu_percent")
        assert check.passed is True


class TestSoakReportWriter:
    def test_report_roundtrip(self, tmp_path):
        from verification.soak.report import SoakReport

        report_path = tmp_path / "soak.jsonl"
        with SoakReport(report_path) as report:
            snap = SoakSnapshot(
                timestamp="2026-01-01T00:00:00",
                elapsed_sec=30.0,
                phase="gc",
                rss_mb=100.0,
                vms_mb=200.0,
                cpu_percent=10.0,
                tests_passed=50,
                tests_failed=1,
            )
            report.write_snapshot(snap)

        lines = report_path.read_text(encoding="utf-8").strip().splitlines()
        assert len(lines) == 3
        header = json.loads(lines[0])
        assert header["event"] == "soak_start"
        snapshot = json.loads(lines[1])
        assert snapshot["phase"] == "gc"
        assert snapshot["rss_mb"] == 100.0
        assert snapshot["tests_passed"] == 50
        footer = json.loads(lines[2])
        assert footer["event"] == "soak_end"


class TestConfigLoader:
    def test_load_config(self, tmp_path):
        config_content = """
total_duration_min: 10
sampling_interval_sec: 5
test_timeout_sec: 30

phases:
  - name: gc
    duration_min: 5
    test_label: stress;gc
    description: "GC stress"
"""
        config_path = tmp_path / "soak-config.yaml"
        config_path.write_text(config_content, encoding="utf-8")

        from soak.orchestrator import _load_config
        config = _load_config(config_path)
        assert config["total_duration_min"] == 10
        assert config["sampling_interval_sec"] == 5
        assert len(config["phases"]) == 1
        assert config["phases"][0]["name"] == "gc"
        assert config["phases"][0]["duration_min"] == 5
