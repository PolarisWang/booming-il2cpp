"""Soak test report — JSONL snapshot recording and aggregation.

Follows the same ARRHENIUS-compatible JSONL format used by gc_soak_test.cpp.
"""

from __future__ import annotations

import json
import time
from dataclasses import dataclass, field, asdict
from pathlib import Path
from typing import Any


@dataclass
class SoakSnapshot:
    timestamp: str = ""
    elapsed_sec: float = 0.0
    phase: str = ""
    rss_mb: float = 0.0
    vms_mb: float = 0.0
    cpu_percent: float = 0.0
    tests_passed: int = 0
    tests_failed: int = 0

    def to_jsonl(self) -> str:
        return json.dumps(asdict(self), ensure_ascii=False)


@dataclass
class DegradationCheck:
    passed: bool = True
    metric: str = ""
    slope: float = 0.0
    warning: str = ""
    sufficient: bool = True   # False when too few samples to judge


class SoakReport:
    """JSONL-based soak report writer."""

    def __init__(self, path: Path):
        self.path = path
        self._file = None

    def __enter__(self) -> SoakReport:
        self.path.parent.mkdir(parents=True, exist_ok=True)
        self._file = self.path.open("w", encoding="utf-8")
        header = {
            "event": "soak_start",
            "timestamp": time.strftime("%Y-%m-%dT%H:%M:%S", time.localtime()),
            "pid": __import__("os").getpid(),
        }
        self._file.write(json.dumps(header, ensure_ascii=False) + "\n")
        return self

    def __exit__(self, *args: Any) -> None:
        if self._file:
            footer = {
                "event": "soak_end",
                "timestamp": time.strftime("%Y-%m-%dT%H:%M:%S", time.localtime()),
            }
            self._file.write(json.dumps(footer, ensure_ascii=False) + "\n")
            self._file.close()

    def write_snapshot(self, snapshot: SoakSnapshot) -> None:
        if self._file:
            self._file.write(snapshot.to_jsonl() + "\n")
            self._file.flush()

    def write_degradation(self, check: DegradationCheck) -> None:
        if self._file:
            record = asdict(check)
            record["event"] = "degradation_check"
            self._file.write(json.dumps(record, ensure_ascii=False) + "\n")
            self._file.flush()


def compute_degradation(snapshots: list[SoakSnapshot], metric: str) -> DegradationCheck:
    if len(snapshots) < 6:
        # Too few samples to judge — mark insufficient (NOT passed) so callers
        # don't report "no degradation / OK" off an empty signal.
        return DegradationCheck(passed=False, metric=metric, warning="insufficient samples",
                                sufficient=False)

    recent = snapshots[-6:]
    xs = list(range(len(recent)))
    vals = [_get_metric(s, metric) for s in recent]

    n = len(xs)
    sum_x = sum(xs)
    sum_y = sum(vals)
    sum_xy = sum(x * y for x, y in zip(xs, vals))
    sum_xx = sum(x * x for x in xs)

    denom = n * sum_xx - sum_x * sum_x
    if denom == 0:
        return DegradationCheck(passed=True, metric=metric, warning="zero denominator")

    slope = (n * sum_xy - sum_x * sum_y) / denom

    warning = ""
    passed = True
    if metric in ("rss_mb", "vms_mb") and slope > 5.0:
        passed = False
        warning = f"{metric} rising at {slope:.2f}/sample — possible memory leak"
    elif metric == "tests_failed" and slope > 0.5:
        passed = False
        warning = f"{metric} increasing at {slope:.2f}/sample — failure rate rising"

    return DegradationCheck(passed=passed, metric=metric, slope=slope, warning=warning)


def _get_metric(snapshot: SoakSnapshot, metric: str) -> float:
    mapping: dict[str, float] = {
        "rss_mb": snapshot.rss_mb,
        "vms_mb": snapshot.vms_mb,
        "cpu_percent": snapshot.cpu_percent,
        "tests_passed": float(snapshot.tests_passed),
        "tests_failed": float(snapshot.tests_failed),
    }
    return mapping.get(metric, 0.0)
