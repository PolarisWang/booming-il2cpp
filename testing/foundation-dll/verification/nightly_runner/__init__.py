"""Nightly Runner — full pipeline execution with unified reporting.

A standalone module for running the Chaos IL2CPP foundation-dll verification
pipeline in a Jenkins-friendly way. Reuses all existing stage functions
while adding:

  - Per-worker log capture (TeeStream → file + console)
  - Unified report directory with per-assembly, per-chunk, and summary views
  - Jenkins metadata injection (git commit, job name, build ID)
  - Automatic delta computation and Markdown summary generation

Usage:
    python -m verification.nightly_runner.main --report-dir /path/to/reports
"""
from __future__ import annotations

from verification.nightly_runner.config import NightlyConfig
from verification.nightly_runner.orchestrator import NightlyOrchestrator, NightlyResult
from verification.nightly_runner.report_collector import ReportCollector

__all__ = [
    "NightlyConfig",
    "NightlyOrchestrator",
    "NightlyResult",
    "ReportCollector",
]
