"""stress_comparison.py — Regression detection for stress test results.

Compares current vs baseline metrics with configurable thresholds.
Mirrors benchmark_comparison.py patterns.
"""
from __future__ import annotations

from typing import Any

# Default thresholds for regression detection
DEFAULT_THRESHOLDS: dict[str, float] = {
    # GC stress metrics
    "patternVerificationFailures": 0.0,        # Any increase is regression
    "youngAvgPauseNs": 0.20,                   # >20% increase is regression
    "fullAvgPauseNs": 0.20,                    # >20% increase is regression
    "activeRegionsAfter": 0.30,                # >30% increase may indicate leak
    # Capacity stress metrics
    "failures": 0.0,                           # Any failure is regression
    # Threading stress metrics
    "opsPerSecond": -0.15,                     # >15% decrease is regression
    # Delegate stress metrics
    "allocCount": 0.0,                         # Allocations tracking
    "combineCount": 0.0,                       # Combine ops tracking
    "removeCount": 0.0,                        # Remove ops tracking
    "invokeCount": 0.0,                        # Invoke ops tracking
}


def detect_regression(
    current: dict[str, Any],
    baseline: dict[str, Any] | None,
    thresholds: dict[str, float] | None = None,
) -> dict[str, Any]:
    """Compare current vs baseline metrics and detect regressions.

    Args:
        current: Current run's metrics dict.
        baseline: Baseline run's metrics dict (None = no baseline).
        thresholds: Per-metric threshold overrides.

    Returns:
        {
            "hasRegression": bool,
            "regressions": [{"metric": str, "before": float, "after": float, "delta": str}],
            "improvements": [{"metric": str, "before": float, "after": float, "delta": str}],
            "overall": "passed" | "regressed" | "no_baseline" | "insufficient_data"
        }
    """
    if not baseline:
        return {
            "hasRegression": False,
            "regressions": [],
            "improvements": [],
            "overall": "no_baseline",
        }

    if not current:
        return {
            "hasRegression": False,
            "regressions": [],
            "improvements": [],
            "overall": "insufficient_data",
        }

    merged = dict(thresholds or {})
    for k, v in DEFAULT_THRESHOLDS.items():
        merged.setdefault(k, v)

    regressions: list[dict[str, Any]] = []
    improvements: list[dict[str, Any]] = []

    for metric, threshold in merged.items():
        if metric not in current or metric not in baseline:
            continue

        cur_val = _to_float(current[metric])
        base_val = _to_float(baseline[metric])
        if cur_val is None or base_val is None:
            continue

        if base_val == 0 and cur_val == 0:
            continue  # both zero = no signal

        delta = cur_val - base_val
        pct = delta / base_val if base_val != 0 else float("inf")

        # Higher-is-better metrics (opsPerSecond etc): negative threshold means "decrease beyond this is regression"
        if threshold < 0:
            # For higher-is-better: pct < threshold (e.g. -0.15) means >15% drop
            if pct < threshold:
                regressions.append({
                    "metric": metric,
                    "before": base_val,
                    "after": cur_val,
                    "delta": f"{pct * 100:+.1f}%",
                    "direction": "down",
                })
            elif pct > -threshold:  # symmetrical improvement detection
                improvements.append({
                    "metric": metric,
                    "before": base_val,
                    "after": cur_val,
                    "delta": f"{pct * 100:+.1f}%",
                    "direction": "up",
                })
        else:
            # Lower-is-better metrics (pause ns, failures): pct > threshold means regression
            if delta > 0 and pct > threshold:
                regressions.append({
                    "metric": metric,
                    "before": base_val,
                    "after": cur_val,
                    "delta": f"{pct * 100:+.1f}%",
                    "direction": "up",
                })
            elif delta < 0 and pct < -threshold:
                improvements.append({
                    "metric": metric,
                    "before": base_val,
                    "after": cur_val,
                    "delta": f"{pct * 100:+.1f}%",
                    "direction": "down",
                })

    has_regression = len(regressions) > 0
    overall = "regressed" if has_regression else "passed"

    return {
        "hasRegression": has_regression,
        "regressions": regressions,
        "improvements": improvements,
        "overall": overall,
    }


def _to_float(value: Any) -> float | None:
    try:
        return float(value)
    except (TypeError, ValueError):
        return None
