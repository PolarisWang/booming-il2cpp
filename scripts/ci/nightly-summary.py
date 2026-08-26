#!/usr/bin/env python3
"""Emit a compact Markdown success-path summary for a nightly run to $GITHUB_STEP_SUMMARY.

Each nightly workflow (gc-stress / native-nightly / foundation-dll) produces a
test_driver --json report (structure defined by TestReport: version, ok, total
{passed,failed,total}, known, layers{layer:{groups{}}}, duration_s, ...).  A green
nightly currently surfaces NOTHING to a human — only a red one files an issue.  This
script renders the key numbers (overall pass/fail/known + per-layer group outcomes)
so the GitHub run page shows what actually ran and passed.

Usage:
    python nightly-summary.py --report <test_driver-report.json> [--metrics <gc-metrics-current.json>]
    # appends Markdown to $GITHUB_STEP_SUMMARY (or prints it if that env is unset)
"""

import argparse
import json
import os
import sys


def _load(path: str) -> dict:
    with open(path, encoding="utf-8") as f:
        return json.load(f)


def render(report: dict, metrics: dict | None = None) -> str:
    lines = []
    total = report.get("total", {})
    ok = report.get("ok", False)
    dur = report.get("duration_s", 0.0)
    # Section heading uses the workflow mode; caller can pass a title.
    lines.append("### Native/GC nightly summary")
    lines.append("")
    lines.append(f"- **Overall**: {'OK' if ok else 'FAILED'}")
    lines.append(f"- **Pass**: {total.get('passed', 0)} / {total.get('total', 0)} "
                 f"(fail {total.get('failed', 0)}, known {report.get('known', 0)})")
    if dur:
        lines.append(f"- **Duration**: {dur:.0f}s")
    # Per-layer/per-group outcomes
    layers = report.get("layers", {})
    for layer, lc in layers.items():
        for gname, g in lc.get("groups", {}).items():
            flag = "PASS" if g.get("ok") else "FAIL"
            parts = [f"- **{layer}/{gname}**: {flag} "
                     f"(pass {g.get('passed',0)} fail {g.get('failed',0)} "
                     f"total {g.get('total',0)} known {g.get('known',0)})"]
            if g.get("error"):
                parts.append(f"  - infra error: {g['error'][:200]}")
            unexpected = g.get("unexpected", [])
            if unexpected:
                parts.append(f"  - UNEXPECTED-FAIL ({len(unexpected)}): {', '.join(unexpected[:5])}")
            stale = g.get("stale_known", [])
            if stale:
                parts.append(f"  - stale-known (now passing): {', '.join(stale)}")
            lines.append("".join(parts))
    # Optional perf metrics (gc-metrics-current.json)
    if metrics:
        bench = metrics.get("benchmarks", {})
        if bench:
            lines.append("- **GC perf scenarios**:")
            for scen, kv in sorted(bench.items()):
                p50 = kv.get("P50")
                p95 = kv.get("P95")
                if p50 is not None:
                    lines.append(f"  - {scen}: P50={p50:.6f}s P95={p95:.6f}s N={kv.get('N','?')}")
    lines.append("")
    return "\n".join(lines)


def main() -> int:
    parser = argparse.ArgumentParser(description="Nightly success-path summary → step summary")
    parser.add_argument("--report", required=True, help="test_driver --json report")
    parser.add_argument("--metrics", default=None, help="optional gc-metrics-current.json")
    args = parser.parse_args()

    if not os.path.exists(args.report):
        print(f"[nightly-summary] report not found: {args.report}", file=sys.stderr)
        return 1
    report = _load(args.report)
    metrics = _load(args.metrics) if args.metrics and os.path.exists(args.metrics) else None
    md = render(report, metrics)

    summary_path = os.environ.get("GITHUB_STEP_SUMMARY")
    if summary_path:
        # GitHub Actions: append to the run-page summary file.
        os.makedirs(os.path.dirname(summary_path), exist_ok=True)
        with open(summary_path, "a", encoding="utf-8") as f:
            f.write(md + "\n")
        print(f"[nightly-summary] appended to $GITHUB_STEP_SUMMARY ({summary_path})", file=sys.stderr)
    else:
        # Local: just print so it can be redirected.
        print(md)
    return 0


if __name__ == "__main__":
    sys.exit(main())
