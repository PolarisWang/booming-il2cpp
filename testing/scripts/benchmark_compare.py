#!/usr/bin/env python3
"""
benchmark_compare.py — Generic benchmark regression detection for chaos-il2cpp tests.

Supports three input formats:
  1. Native [BENCH] lines from ctest stdout:  [BENCH] name: value_ns
  2. Foundation-dll JSONL:                    JSON-per-line with metrics.elapsedMilliseconds
  3. Batch JSON report:                       batch-report.json with elapsed_seconds per family

Usage:
  # Compare current run against baseline
  python benchmark_compare.py --current results/benchmarks/current.jsonl

  # Specify baseline explicitly
  python benchmark_compare.py --current results/benchmarks/current.jsonl \\
                              --baseline results/baselines/last-good.jsonl

  # Update baseline after review
  python benchmark_compare.py --current results/benchmarks/current.jsonl --update-baseline

  # Parse native [BENCH] stdout
  python benchmark_compare.py --bench-log test_output.txt

Exit codes:
  0 — PASS (all metrics within threshold)
  1 — WARN (some metrics 5-15% regression)
  2 — FAIL (some metrics >15% regression)
"""

import argparse
import csv
import json
import os
import re
import sys
from collections import defaultdict
from pathlib import Path

# Default thresholds
WARN_PCT = 5.0    # 5-15% regression → WARN
FAIL_PCT = 15.0   # >15% regression → FAIL

BASELINE_DIR = Path(__file__).resolve().parent.parent / "results" / "baselines"


# ── Parsers ─────────────────────────────────────────────────────────────

def parse_bench_log(path: str) -> dict:
    """Parse native [BENCH] lines from ctest output.

    [BENCH] gen1_pause_ns: 12345 ns
    Returns { "gen1_pause_ns": 12345.0 }
    """
    metrics = {}
    pattern = re.compile(r'\[BENCH\]\s+(\w+):\s+([\d.]+)\s+ns')
    with open(path, "r") as f:
        for line in f:
            m = pattern.search(line)
            if m:
                metrics[m.group(1)] = float(m.group(2))
    return {"_source": path, "metrics": metrics}


def parse_jsonl(path: str) -> dict:
    """Parse foundation-dll JSONL format.

    Each line is a JSON object with methodSubjectId, technology,
    metrics.elapsedMilliseconds (or metrics.opsPerSecond).
    Returns per-method metrics grouped by technology.
    """
    result = {"_source": path, "by_technology": defaultdict(list)}
    with open(path, "r") as f:
        for line in f:
            line = line.strip()
            if not line:
                continue
            record = json.loads(line)
            tech = record.get("technology", "unknown")
            result["by_technology"][tech].append({
                "method": record.get("methodSubjectId", "unknown"),
                "methodIndex": record.get("methodIndex", -1),
                "elapsed_ms": record.get("metrics", {}).get("elapsedMilliseconds", 0),
                "ops_per_sec": record.get("metrics", {}).get("opsPerSecond", 0),
                "iterations": record.get("iterations", 0),
            })
    return result


def parse_batch_json(path: str) -> dict:
    """Parse batch-report.json format.

    Returns per-family elapsed_seconds.
    """
    result = {"_source": path, "families": {}}
    with open(path, "r") as f:
        report = json.load(f)
    for entry in report.get("results", []):
        slug = entry.get("slug", "unknown")
        result["families"][slug] = {
            "elapsed_seconds": entry.get("duration_seconds", 0),
            "status": entry.get("status", "unknown"),
            "coverage": entry.get("coverage", {}),
        }
    return result


# ── Baseline I/O ────────────────────────────────────────────────────────

def load_baseline(path: str) -> dict:
    """Load a baseline JSON file."""
    with open(path, "r") as f:
        return json.load(f)


def save_baseline(data: dict, slug: str):
    """Save data as the baseline for the given slug."""
    BASELINE_DIR.mkdir(parents=True, exist_ok=True)
    path = BASELINE_DIR / f"{slug}.json"
    with open(path, "w") as f:
        json.dump(data, f, indent=2)
    print(f"[BASELINE] Saved to {path}")


def find_baseline(slug: str) -> str | None:
    """Find the most recent baseline for a slug."""
    path = BASELINE_DIR / f"{slug}.json"
    return str(path) if path.exists() else None


# ── Comparison ──────────────────────────────────────────────────────────

def compare_metric(name: str, current: float, baseline: float) -> dict:
    """Compare a single metric value against baseline.

    Returns dict with: name, current, baseline, pct_change, level
    """
    if baseline == 0:
        pct = float("inf") if current > 0 else 0
    else:
        pct = ((current - baseline) / baseline) * 100.0

    if pct > FAIL_PCT:
        level = "FAIL"
    elif pct > WARN_PCT:
        level = "WARN"
    elif pct < -FAIL_PCT:
        level = "IMPROVED"
    else:
        level = "PASS"

    return {
        "name": name,
        "current": current,
        "baseline": baseline,
        "pct_change": round(pct, 2),
        "level": level,
    }


def compare_native(current: dict, baseline: dict) -> list[dict]:
    """Compare native [BENCH] metrics against baseline."""
    results = []
    cur_metrics = current.get("metrics", {})
    base_metrics = baseline.get("metrics", {})
    all_keys = set(cur_metrics.keys()) | set(base_metrics.keys())
    for key in sorted(all_keys):
        cv = cur_metrics.get(key)
        bv = base_metrics.get(key)
        if cv is not None and bv is not None:
            results.append(compare_metric(key, cv, bv))
        elif cv is not None:
            results.append({
                "name": key, "current": cv, "baseline": None,
                "pct_change": None, "level": "NEW",
            })
        else:
            results.append({
                "name": key, "current": None, "baseline": bv,
                "pct_change": None, "level": "REMOVED",
            })
    return results


def compare_jsonl(current: dict, baseline: dict, tech: str = "chaos-aot") -> list[dict]:
    """Compare per-method elapsed_ms for a given technology."""
    results = []
    cur_methods = {m["methodIndex"]: m for m in current.get("by_technology", {}).get(tech, [])}
    base_methods = {m["methodIndex"]: m for m in baseline.get("by_technology", {}).get(tech, [])}
    all_indices = set(cur_methods.keys()) | set(base_methods.keys())
    for idx in sorted(all_indices):
        cm = cur_methods.get(idx)
        bm = base_methods.get(idx)
        if cm and bm:
            r = compare_metric(
                cm.get("method", f"method_{idx}"),
                cm.get("elapsed_ms", 0),
                bm.get("elapsed_ms", 0),
            )
            r["iterations"] = cm.get("iterations", 0)
            results.append(r)
        elif cm:
            results.append({
                "name": cm.get("method", f"method_{idx}"),
                "current": cm.get("elapsed_ms", 0), "baseline": None,
                "pct_change": None, "level": "NEW",
                "iterations": cm.get("iterations", 0),
            })
    return results


def compare_batch(current: dict, baseline: dict) -> list[dict]:
    """Compare per-family duration from batch reports."""
    results = []
    cur_fams = current.get("families", {})
    base_fams = baseline.get("families", {})
    all_slugs = set(cur_fams.keys()) | set(base_fams.keys())
    for slug in sorted(all_slugs):
        cf = cur_fams.get(slug)
        bf = base_fams.get(slug)
        if cf and bf:
            results.append(compare_metric(
                slug, cf["elapsed_seconds"], bf["elapsed_seconds"],
            ))
        elif cf:
            results.append({
                "name": slug, "current": cf["elapsed_seconds"],
                "baseline": None, "pct_change": None, "level": "NEW",
            })
    return results


# ── Report ──────────────────────────────────────────────────────────────

def print_report(results: list[dict], title: str = "Benchmark Comparison"):
    """Print a human-readable comparison report."""
    print(f"\n{'═' * 60}")
    print(f"  {title}")
    print(f"{'═' * 60}")

    levels = {"FAIL": [], "WARN": [], "PASS": [], "IMPROVED": [], "NEW": [], "REMOVED": []}
    for r in results:
        levels.setdefault(r["level"], []).append(r)

    for level, label, color in [
        ("FAIL", "REGRESSION (>15%)", "!"),
        ("WARN", "REGRESSION (5-15%)", "?"),
        ("IMPROVED", "IMPROVEMENT (>15%)", "+"),
        ("PASS", "OK (<5%)", " "),
        ("NEW", "NEW", "+"),
        ("REMOVED", "REMOVED", "-"),
    ]:
        items = levels.get(level, [])
        if items:
            print(f"\n  [{label}]")
            for r in items:
                cv = r.get("current", "?")
                bv = r.get("baseline", "?")
                pct = r.get("pct_change")
                if pct is not None:
                    sign = "+" if pct > 0 else ""
                    print(f"    {color} {r['name']}: {cv} vs {bv} ({sign}{pct}%)")
                else:
                    print(f"    {color} {r['name']}: current={cv}, baseline={bv}")

    passed = len(levels.get("PASS", [])) + len(levels.get("IMPROVED", []))
    failed = len(levels.get("FAIL", []))
    warned = len(levels.get("WARN", []))
    total = len(results)

    print(f"\n{'─' * 60}")
    print(f"  Total: {total} | PASS: {passed} | WARN: {warned} | FAIL: {failed}")
    print(f"{'─' * 60}\n")

    return failed, warned


def main():
    parser = argparse.ArgumentParser(
        description="Benchmark regression detection for chaos-il2cpp tests")
    parser.add_argument("--current", help="Path to current run data")
    parser.add_argument("--baseline", help="Path to baseline data (auto-detected if omitted)")
    parser.add_argument("--update-baseline", action="store_true",
                        help="Save current data as new baseline")
    parser.add_argument("--bench-log", help="Parse [BENCH] lines from a text log file")
    parser.add_argument("--batch-json", help="Parse batch-report.json format")
    parser.add_argument("--slug", default="default",
                        help="Slug for baseline file naming (default: 'default')")
    parser.add_argument("--threshold-warn", type=float, default=WARN_PCT,
                        help=f"Warn threshold %% (default: {WARN_PCT}%%)")
    parser.add_argument("--threshold-fail", type=float, default=FAIL_PCT,
                        help=f"Fail threshold %% (default: {FAIL_PCT}%%)")
    args = parser.parse_args()

    global WARN_PCT, FAIL_PCT
    WARN_PCT = args.threshold_warn
    FAIL_PCT = args.threshold_fail

    # Parse current data
    current = None
    if args.bench_log:
        current = parse_bench_log(args.bench_log)
    elif args.batch_json:
        current = parse_batch_json(args.batch_json)
    elif args.current:
        path = args.current
        if path.endswith(".jsonl"):
            current = parse_jsonl(path)
        elif path.endswith(".json"):
            current = parse_batch_json(path)
    else:
        print("ERROR: Specify --bench-log, --batch-json, or --current")
        sys.exit(2)

    if current is None or not current:
        print("ERROR: No data parsed from input")
        sys.exit(2)

    # Update baseline and exit
    if args.update_baseline:
        save_baseline(current, args.slug)
        print("Baseline updated — no comparison performed.")
        sys.exit(0)

    # Find baseline
    baseline_path = args.baseline or find_baseline(args.slug)
    if not baseline_path:
        print(f"No baseline found for '{args.slug}'. Use --update-baseline to create one.")
        save_baseline(current, args.slug)
        print("Current data saved as initial baseline.")
        sys.exit(0)

    # Load baseline
    baseline = load_baseline(baseline_path)

    # Compare
    if args.bench_log:
        results = compare_native(current, baseline)
        title = f"Native Benchmark Comparison ({args.slug})"
    elif args.batch_json or (args.current and args.current.endswith(".json")):
        results = compare_batch(current, baseline)
        title = f"Batch Report Comparison ({args.slug})"
    else:
        results = compare_jsonl(current, baseline)
        title = f"JSONL Benchmark Comparison ({args.slug})"

    # Report
    failed, warned = print_report(results, title)

    if failed > 0:
        sys.exit(2)
    elif warned > 0:
        sys.exit(1)
    else:
        sys.exit(0)


if __name__ == "__main__":
    main()
