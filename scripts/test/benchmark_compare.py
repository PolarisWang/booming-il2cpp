#!/usr/bin/env python3
"""
benchmark_compare.py — Unified benchmark regression detection for chaos-il2cpp tests.

Supports two comparison modes:

Mode 1: Chronological (default) — compares current run vs historical baseline
  Inputs: Native [BENCH] lines, foundation-dll JSONL, batch JSON reports
  Threshold: %-based (5% warn, 15% fail)

Mode 2: 3-way (cross-mode) — compares managed vs native vs interpreter
  Inputs: Three JSON metric files (one per mode)
  Threshold: ratio-based (native >= 2x managed, interpreter <= 50x managed)
  Delegates to benchmark_comparison.py for core logic.

Usage:
  # Chronological: compare current run against baseline
  python benchmark_compare.py --current results/benchmarks/current.jsonl

  # Chronological: parse native [BENCH] stdout
  python benchmark_compare.py --bench-log test_output.txt

  # 3-way: compare three modes
  python benchmark_compare.py --comparison-mode 3way \
      --managed managed_metrics.json --native native_metrics.json
"""

import argparse
import csv
import json
import os
import re
import sys
from collections import defaultdict
from pathlib import Path

# Default thresholds for chronological mode
WARN_PCT = 5.0    # 5-15% regression → WARN
FAIL_PCT = 15.0   # >15% regression → FAIL

BASELINE_DIR = Path(__file__).resolve().parent.parent / "results" / "baselines"

# Try importing 3-way comparison module (from toolchain)
_3WAY_MODULE = None
_3WAY_PATH = Path(__file__).resolve().parents[3] / "build" / "toolchains" / "run" / "testing" / "benchmark_comparison.py"
if _3WAY_PATH.exists():
    import importlib.util
    _spec = importlib.util.spec_from_file_location("benchmark_comparison", _3WAY_PATH)
    if _spec and _spec.loader:
        _3WAY_MODULE = importlib.util.module_from_spec(_spec)
        try:
            _spec.loader.exec_module(_3WAY_MODULE)
        except Exception:
            _3WAY_MODULE = None


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


def _run_3way_comparison(args) -> None:
    """Run 3-way cross-mode comparison via benchmark_comparison module."""
    if _3WAY_MODULE is None:
        print("ERROR: 3-way comparison module (benchmark_comparison.py) not found.", file=sys.stderr)
        print(f"  Expected at: {_3WAY_PATH}", file=sys.stderr)
        sys.exit(2)

    def _load_metrics(path_str: str | None) -> dict | None:
        if not path_str:
            return None
        path = Path(path_str)
        if not path.exists():
            print(f"ERROR: metrics file not found: {path}", file=sys.stderr)
            sys.exit(2)
        try:
            return json.loads(path.read_text(encoding="utf-8"))
        except (OSError, json.JSONDecodeError) as e:
            print(f"ERROR: failed to parse {path}: {e}", file=sys.stderr)
            sys.exit(2)

    managed = _load_metrics(args.managed)
    native = _load_metrics(args.native)
    interpreter = _load_metrics(args.interpreter)

    comparison = _3WAY_MODULE.compute_comparison(managed, native, interpreter)
    verdict = _3WAY_MODULE.evaluate_targets(comparison)

    print(f"\n{'='*60}")
    print("  3-Way Benchmark Comparison")
    print(f"{'='*60}")

    native_speedup = comparison.get("nativeSpeedup")
    interp_overhead = comparison.get("interpreterOverhead")
    print(f"\n  Native speedup:     {native_speedup}" if native_speedup is not None else "\n  Native speedup:     no data")
    print(f"  Interpreter overhead: {interp_overhead}" if interp_overhead is not None else "  Interpreter overhead: no data")

    for key, v in verdict.get("verdicts", {}).items():
        label = v.get("label", "?")
        status = "PASS" if v.get("pass") else "FAIL" if v.get("pass") is False else "SKIP"
        print(f"  [{status}] {key}: {label}")

    overall = "PASS" if verdict.get("overallPass") else "FAIL"
    print(f"\n  Overall: {overall}")
    print(f"{'='*60}\n")

    # Save baseline if requested
    if args.update_baseline and _3WAY_MODULE:
        subject = args.slug or "unknown"
        _3WAY_MODULE.save_baseline(subject, comparison, verdict)
        print(f"Baseline saved for '{subject}'")

    sys.exit(0 if verdict.get("overallPass") else 1)


def main():
    parser = argparse.ArgumentParser(
        description="Benchmark regression detection for chaos-il2cpp tests")

    # Comparison mode
    parser.add_argument("--comparison-mode", choices=["chronological", "3way"], default="chronological",
                        help="Comparison mode: chronological (default, vs baseline) or 3way (cross-mode)")

    # Chronological mode args
    parser.add_argument("--current", help="Path to current run data (chronological mode)")
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

    # 3-way mode args
    parser.add_argument("--managed", help="Path to managed metrics JSON (3-way mode)")
    parser.add_argument("--native", help="Path to native metrics JSON (3-way mode)")
    parser.add_argument("--interpreter", help="Path to interpreter metrics JSON (3-way mode)")

    args = parser.parse_args()

    # Dispatch to 3-way mode
    if args.comparison_mode == "3way":
        _run_3way_comparison(args)
        return  # unreachable

    # Chronological mode (existing behavior)
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
