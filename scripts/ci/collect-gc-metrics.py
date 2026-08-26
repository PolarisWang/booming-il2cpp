#!/usr/bin/env python3
"""Collect GC performance metrics from the nightly `gc-stress` ctest capture and
bridge them to the `gc.perf.yaml` baseline.

The native GC benchmark/stress executables emit JIT-collector-compatible lines
when they finish measuring a scenario:

    BENCH,<scenario>,P50=..,P95=..,P99=..,AVG=..,N=..     (ns units)

This collector:
  1. Parses those `BENCH,` lines from a captured ctest/executable output file
     (same format as collect-jit-metrics.py parse_bench_lines).
  2. Converts each scenario's pause values from ns to SECONDS (gc.perf.yaml
     mandates `unit: "seconds"`).
  3. Reads tests/runner/baselines/gc.perf.yaml (the factual baseline) and emits:
       --output          -> gc-metrics-current.json  {benchmarks:{...}} (comparator input)
       --baseline-output -> gc-baseline.json         {benchmarks:{...}} (yaml pause_seconds in seconds)
       --thresholds      -> gc-thresholds.json       {thresholds:{...}, severe:{...}} (tol_pct -> multiplier)

compare-baseline.py then diffs current vs baseline using the GC thresholds
(continue-on-error in the nightly workflow — perf is advisory, correctness is
gated by the stress tier proper).

Usage:
    python collect-gc-metrics.py \
        --ctest-output=gc-stress-output.txt \
        --gc-perf-yaml=tests/runner/baselines/gc.perf.yaml \
        --output=gc-metrics-current.json \
        --baseline-output=gc-baseline.json \
        --thresholds=gc-thresholds.json
"""

import argparse
import json
import os
import sys


def parse_bench_lines(text: str) -> dict:
    """Parse `BENCH,<name>,KEY=VAL,...` lines (mirror collect-jit-metrics.py).

    Multiple `BENCH,<name>,...` lines with the same name overwrite (last wins).
    Returns {name: {KEY: float}}.
    """
    metrics = {}
    for line in text.splitlines():
        if not line.startswith("BENCH,"):
            continue
        parts = line.split(",")
        if len(parts) < 3:
            continue
        name = parts[1]
        kv_pairs = {}
        for kv in parts[2:]:
            if "=" in kv:
                key, val = kv.split("=", 1)
                try:
                    kv_pairs[key] = float(val)
                except ValueError:
                    kv_pairs[key] = val
        metrics[name] = kv_pairs
    return metrics


def read_text(path: str) -> str:
    for enc in ("utf-8", "gbk", "latin-1"):
        try:
            with open(path, encoding=enc) as f:
                return f.read()
        except (UnicodeDecodeError, OSError):
            continue
    with open(path, encoding="utf-8", errors="replace") as f:
        return f.read()


def load_yaml(path: str) -> dict:
    import yaml
    with open(path, encoding="utf-8") as f:
        return yaml.safe_load(f) or {}


def _to_sec(kv: dict, key: str):
    """Convert a ns value to seconds; return None if absent/zero."""
    v = kv.get(key)
    if v is None or v == 0:
        return None
    return round(v / 1e9, 6)


def main():
    parser = argparse.ArgumentParser(description="Collect GC perf metrics")
    parser.add_argument("--ctest-output", required=True,
                        help="Captured GC stress ctest/executable output file")
    parser.add_argument("--gc-perf-yaml", default="tests/runner/baselines/gc.perf.yaml",
                        help="Path to gc.perf.yaml baseline")
    parser.add_argument("--output", required=True,
                        help="Current metrics JSON output path")
    parser.add_argument("--baseline-output",
                        help="JSON-ified baseline output path (optional)")
    parser.add_argument("--thresholds",
                        help="GC thresholds JSON output path (optional)")
    args = parser.parse_args()

    text = read_text(args.ctest_output)
    parsed = parse_bench_lines(text)

    # Current metrics: BENCH, values in ns -> seconds.  Shape matches the JIT
    # comparator (compare-baseline.py iterates current["benchmarks"]).
    current = {"benchmarks": {}, "code_size": {}, "test_counts": {}}
    for name, kv in parsed.items():
        by_key = {}
        for k, v in kv.items():
            if k in ("P50", "P95", "P99", "AVG"):
                sec = v / 1e9 if isinstance(v, (int, float)) else None
                by_key[k] = round(sec, 6) if sec is not None else None
            elif k in ("N", "MIN", "MAX"):
                by_key[k] = v
        current["benchmarks"][name] = by_key

    os.makedirs(os.path.dirname(args.output) or ".", exist_ok=True)
    with open(args.output, "w") as f:
        json.dump(current, f, indent=2)
    print(f"[GC metrics] parsed {len(current['benchmarks'])} scenario(s) "
          f"-> {args.output}", file=sys.stderr)

    # Baseline: translate gc.perf.yaml pause_seconds into the comparator shape.
    # Values are null today (pending-first-green-run); the comparator skips null.
    if args.baseline_output and os.path.exists(args.gc_perf_yaml):
        y = load_yaml(args.gc_perf_yaml)
        pause = y.get("pause_seconds", {})
        baseline = {"benchmarks": {}, "code_size": {}, "test_counts": {}}
        for scen, cfg in pause.items():
            entry = {}
            for key, yaml_key in (("P50", "p50"), ("P95", "p95"), ("P99", "p99")):
                v = (cfg or {}).get(yaml_key)
                entry[key] = v if v is not None else None
            baseline["benchmarks"][scen] = entry
        with open(args.baseline_output, "w") as f:
            json.dump(baseline, f, indent=2)
        print(f"[GC metrics] baseline -> {args.baseline_output}", file=sys.stderr)

    # Thresholds: tol_pct -> multiplier (1 + tol_pct/100) for each scenario and
    # each P-key it declares in gc.perf.yaml.  Shape matches compare-baseline.py
    # --thresholds ("thresholds"/"severe" keys).
    if args.thresholds and os.path.exists(args.gc_perf_yaml):
        y = load_yaml(args.gc_perf_yaml)
        pause = y.get("pause_seconds", {})
        thresholds, severe = {}, {}
        for scen, cfg in pause.items():
            cfg = cfg or {}
            tol = cfg.get("tol_pct", 50)
            mult = 1.0 + float(tol) / 100.0
            # Only the P-keys the scenario actually declares get a threshold.
            keys = [k for k in ("p50", "p95", "p99") if k in cfg]
            upper = ["P" + k[1:] for k in keys]
            thresholds[scen] = {k: mult for k in upper}
            # Leave "severe" empty -> comparator falls back to its default 1.25.
            severe[scen] = {k: 1.25 for k in upper}
        thresholds_cfg = {"thresholds": thresholds, "severe": severe}
        with open(args.thresholds, "w") as f:
            json.dump(thresholds_cfg, f, indent=2)
        print(f"[GC metrics] thresholds -> {args.thresholds}", file=sys.stderr)

    return 0


if __name__ == "__main__":
    sys.exit(main())
