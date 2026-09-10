#!/usr/bin/env python3
"""Compare current JIT metrics against a stored baseline.

Returns exit code 0 if all metrics are within threshold.
Returns exit code 1 if any regression exceeds the warning threshold.
Returns exit code 2 if any regression exceeds the severe threshold.

Usage:
    python compare-baseline.py --baseline=<path> --current=<path> [--thresholds=<path>]
"""

import argparse
import json
import sys


# Default threshold multipliers (current / baseline)
DEFAULT_THRESHOLDS = {
    # Benchmark P50 metrics (latency, microseconds)
    "upgrade-latency": {"P50": 1.10, "P99": 1.15},
    "compile-time-only": {"P50": 1.10, "P99": 1.15},
    "trampoline-alloc": {"P50": 1.10, "P99": 1.15},
    "multi-method-upgrade": {"PER_METHOD_US": 1.10, "TOTAL_US": 1.15},
    # Per-call overhead
    "aot-dispatch-overhead": {"PER_CALL_US": 1.10},
    "compiled-dispatch-overhead": {"PER_CALL_US": 1.10},
    # Code size
    "code_size": {"p50": 1.05, "p95": 1.05, "avg": 1.05},
}

# Severe thresholds (for exit code 2)
SEVERE_THRESHOLDS = {
    "upgrade-latency": {"P50": 1.25, "P99": 1.30},
    "compile-time-only": {"P50": 1.25, "P99": 1.30},
    "trampoline-alloc": {"P50": 1.25, "P99": 1.30},
    "multi-method-upgrade": {"PER_METHOD_US": 1.25, "TOTAL_US": 1.30},
    "aot-dispatch-overhead": {"PER_CALL_US": 1.25},
    "compiled-dispatch-overhead": {"PER_CALL_US": 1.25},
    "code_size": {"p50": 1.15, "p95": 1.15, "avg": 1.15},
}


def load_json(path: str) -> dict:
    with open(path) as f:
        return json.load(f)


def compare_metric(name: str, key: str, current: float, baseline: float,
                   threshold: float, severe: float) -> list[str]:
    """Compare a single metric. Returns list of warning/error messages."""
    if baseline == 0:
        return []
    ratio = current / baseline
    messages = []
    label = f"{name}/{key}"
    if ratio > severe:
        messages.append(
            f"SEVERE: {label} {ratio*100:.1f}% of baseline "
            f"(current={current:.1f}, baseline={baseline:.1f}, "
            f"threshold={severe*100:.0f}%)"
        )
    elif ratio > threshold:
        messages.append(
            f"WARNING: {label} {ratio*100:.1f}% of baseline "
            f"(current={current:.1f}, baseline={baseline:.1f}, "
            f"threshold={threshold*100:.0f}%)"
        )
    return messages


def main():
    parser = argparse.ArgumentParser(description="Compare JIT metrics against baseline")
    parser.add_argument("--baseline", required=True, help="Baseline JSON file path")
    parser.add_argument("--current", required=True, help="Current metrics JSON file path")
    parser.add_argument("--thresholds", help="Custom thresholds JSON file path")
    args = parser.parse_args()

    baseline = load_json(args.baseline)
    current = load_json(args.current)

    thresholds = DEFAULT_THRESHOLDS
    severe = SEVERE_THRESHOLDS
    if args.thresholds:
        custom = load_json(args.thresholds)
        if "thresholds" in custom:
            thresholds.update(custom["thresholds"])
        if "severe" in custom:
            severe.update(custom["severe"])

    all_warnings = []
    has_severe = False

    # Compare benchmark metrics
    for bench_name, bench_metrics in current.get("benchmarks", {}).items():
        base_metrics = baseline.get("benchmarks", {}).get(bench_name, {})
        for key, current_val in bench_metrics.items():
            if not isinstance(current_val, (int, float)):
                continue
            if key in ("N", "BATCH", "MIN", "MAX"):
                continue  # Skip sample counts and min/max
            base_val = base_metrics.get(key)
            if base_val is None or base_val == 0:
                continue
            thr = thresholds.get(bench_name, {}).get(key, 1.10)
            sev = severe.get(bench_name, {}).get(key, 1.25)
            msgs = compare_metric(f"bench/{bench_name}", key,
                                  float(current_val), float(base_val), thr, sev)
            for msg in msgs:
                if msg.startswith("SEVERE"):
                    has_severe = True
            all_warnings.extend(msgs)

    # Compare code size metrics
    for key in ("p50", "p95", "avg"):
        cur_val = current.get("code_size", {}).get(key, 0)
        base_val = baseline.get("code_size", {}).get(key, 0)
        if cur_val and base_val:
            thr = thresholds.get("code_size", {}).get(key, 1.05)
            sev = severe.get("code_size", {}).get(key, 1.15)
            msgs = compare_metric("code_size", key,
                                  float(cur_val), float(base_val), thr, sev)
            for msg in msgs:
                if msg.startswith("SEVERE"):
                    has_severe = True
            all_warnings.extend(msgs)

    # Report
    if not all_warnings:
        print("[PASS] All metrics within baseline thresholds.")
        sys.exit(0)

    print("[WARN] Performance regression detected:\n", file=sys.stderr)
    for msg in all_warnings:
        print(f"  {msg}", file=sys.stderr)

    if has_severe:
        print("\n[FAIL] Severe regressions found.", file=sys.stderr)
        sys.exit(2)
    else:
        print("\n[WARN] Warning-level regressions found.", file=sys.stderr)
        sys.exit(1)


if __name__ == "__main__":
    main()
