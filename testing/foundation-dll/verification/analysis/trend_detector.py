"""Trend detector — sliding window analysis for nightly build metrics.

Computes trend direction and slope for metrics over a configurable
history window (default: 7 days).

Usage:
    trend = compute_trend(history_values)
    # → {"direction": "expanding", "slope": 2.3, "days": 7}
"""
from __future__ import annotations

from typing import Any


def compute_trend(
    recent_values: list[float],
    window: int = 7,
    threshold: float = 0.05,
) -> dict[str, Any]:
    """Compute trend direction and slope for a series of values.

    Args:
        recent_values: Most recent values first (index 0 = today).
        window: Full history window size (default: 7 days).
        threshold: Relative change threshold for direction detection.

    Returns:
        dict with keys:
            direction: "improving" | "stable" | "degrading" | "insufficient_data"
            slope: Linear regression slope per day (positive = increasing).
            daysMeasured: Number of data points used.
            trendPct: (recent_avg - older_avg) / older_avg as percentage.
    """
    if len(recent_values) < 2:
        return {
            "direction": "insufficient_data",
            "slope": 0.0,
            "daysMeasured": len(recent_values),
            "trendPct": 0.0,
        }

    # Split into recent half (last N/2 days) and older half (first N/2 days)
    mid = max(1, len(recent_values) // 2)
    older = recent_values[mid:]
    newer = recent_values[:mid]

    older_avg = sum(older) / len(older)
    newer_avg = sum(newer) / len(newer)

    if older_avg == 0:
        return {
            "direction": "insufficient_data",
            "slope": 0.0,
            "daysMeasured": len(recent_values),
            "trendPct": 0.0,
        }

    trend_pct = (newer_avg - older_avg) / older_avg
    slope = trend_pct / max(1, mid)  # per-day slope

    # Heuristic: good_is_decreasing means smaller values are better
    # (e.g. coverage gap).  The caller sets threshold manually.
    if abs(trend_pct) < threshold:
        direction = "stable"
    elif trend_pct > 0:
        direction = "degrading"
    else:
        direction = "improving"

    return {
        "direction": direction,
        "slope": round(slope, 4),
        "daysMeasured": len(recent_values),
        "trendPct": round(trend_pct * 100, 1),
    }


def compute_coverage_trend(
    daily_gaps: list[float],
    window: int = 7,
) -> dict[str, Any]:
    """Coverage-specific trend: smaller gaps are better.

    Wraps compute_trend with coverage-appropriate threshold (5%).
    """
    return compute_trend(daily_gaps, window=window, threshold=0.05)


def compute_benchmark_trend(
    daily_durations: list[float],
    window: int = 7,
) -> dict[str, Any]:
    """Benchmark-specific trend: smaller durations are better.

    Uses 10% threshold (benchmark variance is higher).
    """
    return compute_trend(daily_durations, window=window, threshold=0.10)


def compute_memory_trend(
    daily_allocations: list[float],
    window: int = 7,
) -> dict[str, Any]:
    """Memory-specific trend: smaller allocations are better.

    Uses 10% threshold.
    """
    return compute_trend(daily_allocations, window=window, threshold=0.10)
