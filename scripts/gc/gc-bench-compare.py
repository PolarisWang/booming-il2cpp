#!/usr/bin/env python3
"""gc-bench-compare — GC benchmark regression comparison tool.

Parses BENCH| format output from two benchmark runs and reports
regressions with configurable thresholds.

Usage:
  # Compare two saved output files
  python gc-bench-compare.py --baseline baseline.txt --current current.txt

  # Pipe from stdin for one side
  ./test_gc_regression_benchmark.exe | python gc-bench-compare.py --baseline baseline.txt

  # Custom thresholds
  python gc-bench-compare.py --baseline a.txt --current b.txt --warn 3 --fail 8

BENCH| format:
  BENCH|<name>|<metric>|<value>|<unit>

Output format:
  <PASS|WARN|FAIL>  <name>/<metric>: <baseline_value> <unit> → <current_value> <unit>  (<ratio>x)
"""
from __future__ import annotations

import argparse
import re
import sys
from pathlib import Path
from typing import Any


# ── Parsing ─────────────────────────────────────────────────────────────────

BENCH_RE = re.compile(r"^BENCH\|([^|]+)\|([^|]+)\|([^|]+)\|([^|]*)$")


def parse_bench_file(path: str | Path | None) -> dict[str, dict[str, float]]:
    """Parse a BENCH| file into {name: {metric: value}}.

    If path is None or '-', reads from stdin.
    Returns a dict of {name: {metric: float_value}}.
    """
    result: dict[str, dict[str, float]] = {}

    if path is None or path == "-":
        # Don't close stdin via context manager.
        for line in sys.stdin:
            line = line.strip()
            m = BENCH_RE.match(line)
            if not m:
                continue
            name = m.group(1)
            metric = m.group(2)
            raw_value = m.group(3)
            try:
                value = float(raw_value)
            except ValueError:
                continue
            if name not in result:
                result[name] = {}
            result[name][metric] = value
    else:
        with open(str(path), "r", encoding="utf-8") as f:
            for line in f:
                line = line.strip()
                m = BENCH_RE.match(line)
                if not m:
                    continue
                name = m.group(1)
                metric = m.group(2)
                raw_value = m.group(3)
                try:
                    value = float(raw_value)
                except ValueError:
                    continue
                if name not in result:
                    result[name] = {}
                result[name][metric] = value

    return result


# ── Comparison ──────────────────────────────────────────────────────────────

# Default thresholds (% change that triggers WARN / FAIL)
DEFAULT_WARN_PCT = 5.0
DEFAULT_FAIL_PCT = 10.0


def _is_higher_better(metric: str) -> bool:
    """Heuristic: return True if higher values are better for this metric."""
    higher_better_keywords = [
        "throughput", "mb_per_s", "kb_s", "allocs_per_sec", "count",
    ]
    m_lower = metric.lower()
    for kw in higher_better_keywords:
        if kw in m_lower:
            return True
    return False


def compare_runs(
    baseline: dict[str, dict[str, float]],
    current: dict[str, dict[str, float]],
    warn_pct: float = DEFAULT_WARN_PCT,
    fail_pct: float = DEFAULT_FAIL_PCT,
) -> list[dict[str, Any]]:
    """Compare two parsed BENCH| runs and return a list of result entries.

    Each entry has:
      - name: benchmark name
      - metric: metric name
      - baseline_val: value from baseline
      - current_val: value from current
      - unit: "ratio" (inferred)
      - change_pct: percentage change (positive = improvement if higher_is_better)
      - higher_is_better: bool
      - verdict: "PASS" | "WARN" | "FAIL" | "NEW" | "MISSING"
    """
    results: list[dict[str, Any]] = []
    all_names = sorted(set(baseline.keys()) | set(current.keys()))

    for name in all_names:
        base_metrics = baseline.get(name, {})
        curr_metrics = current.get(name, {})

        if not base_metrics:
            # Entire benchmark is new.
            for metric, val in sorted(curr_metrics.items()):
                results.append({
                    "name": name,
                    "metric": metric,
                    "baseline_val": None,
                    "current_val": val,
                    "change_pct": None,
                    "higher_is_better": _is_higher_better(metric),
                    "verdict": "NEW",
                })
            continue

        if not curr_metrics:
            # Entire benchmark is missing.
            for metric, val in sorted(base_metrics.items()):
                results.append({
                    "name": name,
                    "metric": metric,
                    "baseline_val": val,
                    "current_val": None,
                    "change_pct": None,
                    "higher_is_better": _is_higher_better(metric),
                    "verdict": "MISSING",
                })
            continue

        all_metrics = sorted(set(base_metrics.keys()) | set(curr_metrics.keys()))
        for metric in all_metrics:
            base_val = base_metrics.get(metric)
            curr_val = curr_metrics.get(metric)

            if base_val is None and curr_val is not None:
                verdict = "NEW"
            elif curr_val is None and base_val is not None:
                verdict = "MISSING"
            elif base_val is not None and curr_val is not None and base_val != 0:
                change_pct = ((curr_val - base_val) / abs(base_val)) * 100.0
                higher = _is_higher_better(metric)
                abs_change = abs(change_pct)
                if abs_change >= fail_pct:
                    # Check direction: if change is in the bad direction, FAIL
                    if (higher and change_pct < 0) or (not higher and change_pct > 0):
                        verdict = "FAIL"
                    else:
                        verdict = "PASS"  # improvement beyond fail threshold is still PASS
                elif abs_change >= warn_pct:
                    if (higher and change_pct < 0) or (not higher and change_pct > 0):
                        verdict = "WARN"
                    else:
                        verdict = "PASS"
                else:
                    verdict = "PASS"
            else:
                verdict = "PASS"
                change_pct = None

            results.append({
                "name": name,
                "metric": metric,
                "baseline_val": base_val,
                "current_val": curr_val,
                "change_pct": change_pct,
                "higher_is_better": _is_higher_better(metric),
                "verdict": verdict,
            })

    return results


# ── Output ──────────────────────────────────────────────────────────────────


def format_results(results: list[dict[str, Any]], baseline_path: str, current_path: str) -> str:
    """Format comparison results as human-readable text."""
    lines: list[str] = []
    lines.append(f"GC Benchmark Regression Comparison")
    lines.append(f"  Baseline: {baseline_path}")
    lines.append(f"  Current:  {current_path}")
    lines.append(f"  Thresholds: WARN >={DEFAULT_WARN_PCT}%, FAIL >={DEFAULT_FAIL_PCT}%")
    lines.append("")

    counts = {"PASS": 0, "WARN": 0, "FAIL": 0, "NEW": 0, "MISSING": 0}
    for r in results:
        counts[r["verdict"]] = counts.get(r["verdict"], 0) + 1

        if r["verdict"] in ("PASS", "NEW", "MISSING") and r["baseline_val"] is not None:
            # Show PASS entries compactly (one line per benchmark+metric)
            label = f"  [{r['verdict']}]"
        elif r["verdict"] == "PASS" and r["baseline_val"] is None:
            continue  # skip new entries in compact mode
        else:
            label = f"> [{r['verdict']}]"

        if r["baseline_val"] is not None and r["current_val"] is not None:
            change_str = f"{r['change_pct']:+.1f}%" if r["change_pct"] is not None else ""
            arrow = "↑" if r["higher_is_better"] == (r["change_pct"] or 0) > 0 else "↓"
            lines.append(
                f"  {label}  {r['name']}/{r['metric']}: "
                f"{r['baseline_val']:.1f} → {r['current_val']:.1f}  "
                f"({arrow} {change_str})"
            )
        elif r["baseline_val"] is None:
            lines.append(f"  {label}  {r['name']}/{r['metric']}: {r['current_val']:.1f}  (NEW)")
        else:
            lines.append(f"  {label}  {r['name']}/{r['metric']}: {r['baseline_val']:.1f}  (MISSING)")

    lines.append("")
    lines.append(f"Summary: {counts['PASS']} pass, {counts['WARN']} warn, "
                 f"{counts['FAIL']} fail, {counts['NEW']} new, {counts['MISSING']} missing")
    lines.append("")

    return "\n".join(lines)


def format_json(results: list[dict[str, Any]]) -> str:
    """Format results as JSON."""
    import json
    return json.dumps(results, indent=2, ensure_ascii=False)


# ── CLI ─────────────────────────────────────────────────────────────────────


def main() -> None:
    parser = argparse.ArgumentParser(
        description="GC benchmark regression comparison tool",
    )
    parser.add_argument(
        "--baseline", "-b",
        required=True,
        help="Baseline BENCH| output file (or '-' for stdin)",
    )
    parser.add_argument(
        "--current", "-c",
        required=True,
        help="Current BENCH| output file (or '-' for stdin)",
    )
    parser.add_argument(
        "--warn", "-w",
        type=float,
        default=DEFAULT_WARN_PCT,
        help=f"Warn threshold in %% (default: {DEFAULT_WARN_PCT}%%)",
    )
    parser.add_argument(
        "--fail", "-f",
        type=float,
        default=DEFAULT_FAIL_PCT,
        help=f"Fail threshold in %% (default: {DEFAULT_FAIL_PCT}%%)",
    )
    parser.add_argument(
        "--json",
        action="store_true",
        help="Output JSON instead of human-readable text",
    )
    args = parser.parse_args()

    baseline = parse_bench_file(args.baseline)
    current = parse_bench_file(args.current)

    results = compare_runs(baseline, current, args.warn, args.fail)

    if args.json:
        print(format_json(results))
    else:
        print(format_results(results, args.baseline, args.current))

    # Exit code: 0 if no FAIL, 1 if any FAIL.
    if any(r["verdict"] == "FAIL" for r in results):
        sys.exit(1)


if __name__ == "__main__":
    main()
