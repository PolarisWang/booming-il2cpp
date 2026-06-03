"""benchmark_comparison.py — 3-way benchmark comparison logic.

Computes ratios between managed / native / interpreter perf metrics and
evaluates pass/fail against configurable performance targets.

Also provides baseline persistence for regression detection across commits.
"""
from __future__ import annotations

import json
from pathlib import Path
from typing import Any

# ---------------------------------------------------------------------------
# Baseline storage paths
# ---------------------------------------------------------------------------

_BASELINE_DIR = Path(__file__).resolve().parent.parent / "artifact" / "verification" / "benchmark-baselines"


def _ensure_baseline_dir() -> Path:
    _BASELINE_DIR.mkdir(parents=True, exist_ok=True)
    return _BASELINE_DIR


def save_baseline(subject_id: str, comparison: dict[str, Any], verdict: dict[str, Any]) -> Path:
    """Save comparison result as a baseline for the given subject."""
    baseline_dir = _ensure_baseline_dir()
    path = baseline_dir / f"{subject_id}.json"
    data = {
        "subjectId": subject_id,
        "comparison": comparison,
        "verdict": verdict,
        "savedAt": __import__("datetime").datetime.now(
            __import__("datetime").timezone.utc
        ).isoformat().replace("+00:00", "Z"),
    }
    path.write_text(json.dumps(data, indent=2, ensure_ascii=False), encoding="utf-8")
    return path


def load_baseline(subject_id: str) -> dict[str, Any] | None:
    """Load the most recent baseline for a subject, or None."""
    path = _BASELINE_DIR / f"{subject_id}.json"
    if not path.exists():
        return None
    try:
        return json.loads(path.read_text(encoding="utf-8"))
    except (OSError, json.JSONDecodeError):
        return None


def list_baseline_subjects() -> list[str]:
    """Return sorted list of subject IDs that have baselines."""
    if not _BASELINE_DIR.is_dir():
        return []
    return sorted(p.stem for p in _BASELINE_DIR.iterdir() if p.suffix == ".json")


# ---------------------------------------------------------------------------
# Default performance targets
# ---------------------------------------------------------------------------

DEFAULT_TARGETS = {
    # AOT native must be at least 2x faster than C# managed baseline.
    "nativeVsManaged_throughputRatio": {"min": 2.0, "direction": "higher_is_better"},
    # HotUpdate interpreter must be no more than 50x slower than C# managed.
    "interpreterVsManaged_latencyRatio": {"max": 50.0, "direction": "lower_is_better"},
}

# ---------------------------------------------------------------------------
# Core comparison
# ---------------------------------------------------------------------------


def compute_comparison(
    managed_metrics: dict[str, Any] | None,
    native_metrics: dict[str, Any] | None,
    interpreter_metrics: dict[str, Any] | None,
) -> dict[str, Any]:
    """Return a comparison dict with ratios for every shared numeric metric.

    Ratios are computed as  mode_a / mode_b.  Missing metrics yield None;
    division-by-zero yields the string "N/A".
    """
    managed = dict(managed_metrics or {})
    native = dict(native_metrics or {})
    interpreter = dict(interpreter_metrics or {})

    def _ratio(numerator: Any, denominator: Any) -> float | str | None:
        if numerator is None or denominator is None:
            return None
        try:
            num = float(numerator)
            den = float(denominator)
        except (TypeError, ValueError):
            return None
        if den == 0.0:
            return "N/A"
        return round(num / den, 2)

    # Build ratio triplets for each metric present in any mode.
    all_metric_keys = sorted(
        {k for k in list(managed.keys()) + list(native.keys()) + list(interpreter.keys())}
    )

    native_vs_managed: dict[str, Any] = {}
    interpreter_vs_managed: dict[str, Any] = {}
    native_vs_interpreter: dict[str, Any] = {}

    for key in all_metric_keys:
        m = managed.get(key)
        n = native.get(key)
        i = interpreter.get(key)
        native_vs_managed[key] = _ratio(n, m)
        interpreter_vs_managed[key] = _ratio(i, m)
        native_vs_interpreter[key] = _ratio(n, i)

    # Convenience scalar aliases used by the dashboard.
    # meanDurationMs: lower native value → native is *faster* → speedup ratio = managed/native
    managed_lat = managed.get("meanDurationMs")
    native_lat = native.get("meanDurationMs")
    interp_lat = interpreter.get("meanDurationMs")

    native_speedup = _ratio(managed_lat, native_lat)          # > 1 means native is faster
    interp_overhead = _ratio(interp_lat, managed_lat)         # > 1 means interpreter is slower

    return {
        "nativeVsManaged": native_vs_managed,
        "interpreterVsManaged": interpreter_vs_managed,
        "nativeVsInterpreter": native_vs_interpreter,
        # Scalar summaries
        "nativeSpeedup": native_speedup,     # managed_lat / native_lat  (>1 = faster)
        "interpreterOverhead": interp_overhead,  # interp_lat / managed_lat (>1 = slower)
    }


# ---------------------------------------------------------------------------
# Target evaluation
# ---------------------------------------------------------------------------


def evaluate_targets(
    comparison: dict[str, Any],
    targets: dict[str, Any] | None = None,
) -> dict[str, Any]:
    """Return a verdict dict: pass/fail per target, plus an overall pass flag."""
    targets = targets or DEFAULT_TARGETS

    native_speedup = comparison.get("nativeSpeedup")
    interp_overhead = comparison.get("interpreterOverhead")

    verdicts: dict[str, Any] = {}

    # Native vs managed speedup
    native_target = targets.get("nativeVsManaged_throughputRatio", {})
    if native_speedup is not None and isinstance(native_speedup, (int, float)):
        min_val = native_target.get("min")
        pass_native = (min_val is None) or (float(native_speedup) >= float(min_val))
        verdicts["nativeVsManaged"] = {
            "actual": native_speedup,
            "target": min_val,
            "pass": pass_native,
            "label": f"{native_speedup:.2f}x speedup",
        }
    else:
        verdicts["nativeVsManaged"] = {"actual": native_speedup, "pass": None, "label": "no data"}

    # Interpreter overhead
    interp_target = targets.get("interpreterVsManaged_latencyRatio", {})
    if interp_overhead is not None and isinstance(interp_overhead, (int, float)):
        max_val = interp_target.get("max")
        pass_interp = (max_val is None) or (float(interp_overhead) <= float(max_val))
        verdicts["interpreterVsManaged"] = {
            "actual": interp_overhead,
            "target": max_val,
            "pass": pass_interp,
            "label": f"{interp_overhead:.2f}x overhead",
        }
    else:
        verdicts["interpreterVsManaged"] = {"actual": interp_overhead, "pass": None, "label": "no data"}

    overall_pass = all(
        v.get("pass") is True
        for v in verdicts.values()
        if v.get("pass") is not None
    )

    return {
        "verdicts": verdicts,
        "overallPass": overall_pass,
    }
