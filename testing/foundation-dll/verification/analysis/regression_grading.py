"""Regression grading — benchmark performance regression grading for CI gates.

Computes a regression grade per chunk and per assembly based on
per-method performance deltas against historical averages.

Usage:
    grade = compute_grade(today_stats, prev_stats, thresholds)
    # → {"grade": "hard", "totalMethods": 50, "regressedMethods": 3, ...}
"""
from __future__ import annotations

from typing import Any

# Default thresholds
_SOFT_THRESHOLD = 10.0   # duration increase >10% = soft regression
_HARD_THRESHOLD = 20.0   # duration increase >20% = hard regression
_ALLOC_THRESHOLD = 15.0  # allocation increase >15% = regression


def compute_grade(
    today_per_method: list[dict],
    prev_per_method: list[dict] | None = None,
    soft_threshold: float = _SOFT_THRESHOLD,
    hard_threshold: float = _HARD_THRESHOLD,
    alloc_threshold: float = _ALLOC_THRESHOLD,
) -> dict[str, Any]:
    """Compute regression grade for a chunk's benchmark results.

    Args:
        today_per_method: Current run's per-method stats from benchmark-trend.json.
        prev_per_method: Previous run's per-method stats (from history).
        soft_threshold: Duration increase % for soft regression.
        hard_threshold: Duration increase % for hard regression.
        alloc_threshold: Allocation increase % for regression.

    Returns:
        dict with keys:
            grade: "none" | "soft" | "hard"
            totalMethods: Number of methods compared.
            regressedMethods: Count of methods with hard regression.
            degradedMethods: Count of methods with soft regression.
            improvedMethods: Count of methods with significant improvement.
            allocRegressedMethods: Count of methods with allocation regression.
    """
    if not today_per_method:
        return {
            "grade": "none",
            "totalMethods": 0,
            "regressedMethods": 0,
            "degradedMethods": 0,
            "improvedMethods": 0,
            "allocRegressedMethods": 0,
        }

    total = len(today_per_method)
    regressed = 0
    degraded = 0
    improved = 0
    alloc_regressed = 0

    for i, t_stat in enumerate(today_per_method):
        t_dur = t_stat.get("meanDurationMs", 0)
        if t_dur <= 0:
            continue

        # Compare against previous run (if available)
        p_stat = prev_per_method[i] if prev_per_method and i < len(prev_per_method) else None
        if p_stat is None:
            continue

        p_dur = p_stat.get("meanDurationMs", 0)
        if p_dur <= 0:
            continue

        delta = (t_dur - p_dur) / p_dur * 100

        if delta > hard_threshold:
            regressed += 1
        elif delta > soft_threshold:
            degraded += 1
        elif delta < -hard_threshold:
            improved += 1

        # Allocation check
        t_alloc = t_stat.get("meanAllocatedBytes", 0)
        p_alloc = p_stat.get("meanAllocatedBytes", 0)
        if t_alloc > 0 and p_alloc > 0:
            alloc_delta = (t_alloc - p_alloc) / p_alloc * 100
            if alloc_delta > alloc_threshold:
                alloc_regressed += 1

    if regressed > 0:
        grade = "hard"
    elif degraded > 0:
        grade = "soft"
    else:
        grade = "none"

    return {
        "grade": grade,
        "totalMethods": total,
        "regressedMethods": regressed,
        "degradedMethods": degraded,
        "improvedMethods": improved,
        "allocRegressedMethods": alloc_regressed,
    }


def compute_assembly_grade(
    chunk_grades: list[dict[str, Any]],
) -> dict[str, Any]:
    """Aggregate chunk-level grades into an assembly-level grade.

    The assembly grade is the worst grade among its chunks.
    """
    if not chunk_grades:
        return {"grade": "none", "totalMethods": 0, "chunks": 0}

    grades = [g.get("grade", "none") for g in chunk_grades]

    grade_order = {"none": 0, "soft": 1, "hard": 2}
    worst = max(grades, key=lambda g: grade_order.get(g, 0))

    return {
        "grade": worst,
        "totalMethods": sum(g.get("totalMethods", 0) for g in chunk_grades),
        "chunks": len(chunk_grades),
    }
