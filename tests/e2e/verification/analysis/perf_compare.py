"""Perf compare — cross-run profile regression detection.

Compares two profile data sets (baseline vs target) and flags regressions
in GC pause, allocation volume, fast path rate, and code size.

Usage:
    from verification.analysis.perf_compare import compare_profiles, print_comparison

    result = compare_profiles(baseline_data, target_data, threshold_pct=10)
    print_comparison(result)
"""

from __future__ import annotations

from typing import Any


def _pct_change(baseline: float, target: float) -> float | None:
    """Compute (target - baseline) / baseline * 100."""
    if baseline == 0:
        return None
    return round((target - baseline) / baseline * 100, 2)


def compare_profiles(
    baseline: dict[str, Any],
    target: dict[str, Any],
    threshold_pct: float = 10.0,
) -> dict[str, Any]:
    """Compare two profile summaries and detect regressions.

    Args:
        baseline: Baseline profile summary (from PerfBaseline.get() or profile.json summary).
        target: Target profile summary to compare against baseline.
        threshold_pct: Percentage change threshold for flagging a regression (default: 10%).

    Returns:
        Dict with keys: regressions, improvements, unchanged, summary.
    """
    b_metrics = baseline.get("metrics", baseline)
    t_metrics = target.get("metrics", target)

    # Fields to compare
    compare_fields = [
        ("totalGcPauseNs", "GC total pause time", False),
        ("maxGcPauseNs", "GC max pause time", False),
        ("totalNurseryAllocBytes", "Nursery allocation volume", False),
        ("totalAllocCount", "Allocation count", False),
        ("fastPathRate", "Fast path rate", True),  # higher is better
        ("meanNurseryAllocBytes", "Mean allocation per method", False),
        ("meanAllocCount", "Mean alloc count per method", False),
    ]

    section_fields = [
        ("text", ".text size", False),
        ("data", ".data size", False),
        ("pdata", ".pdata size", False),
        ("total", "Total file size", False),
    ]

    regressions: list[dict] = []
    improvements: list[dict] = []
    unchanged: list[dict] = []

    for key, label, higher_is_better in compare_fields:
        b_val = b_metrics.get(key)
        t_val = t_metrics.get(key)
        if b_val is None or t_val is None:
            continue

        change = _pct_change(float(b_val), float(t_val))
        if change is None:
            continue

        entry = {
            "metric": key,
            "label": label,
            "baseline": b_val,
            "target": t_val,
            "changePct": change,
        }

        is_regression = (change > threshold_pct and not higher_is_better) or \
                        (change < -threshold_pct and higher_is_better)
        is_improvement = (change < -threshold_pct and not higher_is_better) or \
                         (change > threshold_pct and higher_is_better)

        if is_regression:
            regressions.append(entry)
        elif is_improvement:
            improvements.append(entry)
        else:
            unchanged.append(entry)

    # Section size comparison
    b_sections = baseline.get("sectionSizes", {})
    t_sections = target.get("sectionSizes", {})
    for key, label, _ in section_fields:
        b_val = b_sections.get(key)
        t_val = t_sections.get(key)
        if b_val is None or t_val is None or b_val == 0:
            continue

        change = _pct_change(float(b_val), float(t_val))
        if change is None:
            continue

        entry = {
            "metric": f"section.{key}",
            "label": label,
            "baseline": b_val,
            "target": t_val,
            "changePct": change,
        }

        if change > threshold_pct:
            regressions.append(entry)
        elif change < -threshold_pct:
            improvements.append(entry)
        else:
            unchanged.append(entry)

    return {
        "summary": {
            "totalCompared": len(compare_fields) + len(section_fields),
            "regressed": len(regressions),
            "improved": len(improvements),
            "unchanged": len(unchanged),
        },
        "regressions": regressions,
        "improvements": improvements,
        "unchanged": unchanged,
    }


def print_comparison(result: dict[str, Any]) -> None:
    """Print a human-readable comparison report."""
    summary = result["summary"]
    print(f"  [perf-compare] Compared {summary['totalCompared']} metrics")
    print(f"  [perf-compare] Regressed: {summary['regressed']}, "
          f"Improved: {summary['improved']}, Unchanged: {summary['unchanged']}")

    if result["regressions"]:
        print(f"  [perf-compare] Regressions:")
        for r in result["regressions"]:
            print(f"    🔴 {r['label']}: {r['baseline']} → {r['target']} ({r['changePct']:+.2f}%)")

    if result["improvements"]:
        print(f"  [perf-compare] Improvements:")
        for r in result["improvements"]:
            print(f"    🟢 {r['label']}: {r['baseline']} → {r['target']} ({r['changePct']:+.2f}%)")

    if summary["regressed"] > 0:
        print(f"  [perf-compare] ⚠️  {summary['regressed']} regression(s) detected")
