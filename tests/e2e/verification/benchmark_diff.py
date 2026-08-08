"""Benchmark diff tool — cross-run regression detection for comparison data.

Reads two comparison-YYYY-MM-DD.json history files, computes per-method
deltas in chaosAotVsNet8Pct (and optionally net10VsNet8Pct, chaosJitVsNet8Pct),
and classifies each method as regressed/improved/unchanged.

Usage:
    python -m verification.benchmark_diff \\
        --baseline _dll/reports/history/comparison-2026-06-03.json \\
        --target  _dll/reports/history/comparison-2026-06-04.json \\
        --threshold 10 \\
        --output regression-report.json

Output:
    - stdout summary table
    - regression-report.json (optional)
"""

from __future__ import annotations

import argparse
import json
import sys
from pathlib import Path
from typing import Any


def _load_comparison(path: Path) -> dict[str, Any]:
    """Load a comparison-*.json history file."""
    if not path.exists():
        print(f"ERROR: file not found: {path}", file=sys.stderr)
        sys.exit(1)
    return json.loads(path.read_text(encoding="utf-8"))


def _build_method_map(comparison: dict) -> dict[str, dict[str, Any]]:
    """Build {methodSubjectId: {chaosAotVsNet8Pct, net10VsNet8Pct, chaosJitVsNet8Pct}}."""
    method_map: dict[str, dict[str, Any]] = {}
    # The history format has perChunk with per-chunk aggregate only (no per-method detail).
    # The full detail is in the per-chunk comparison.json files.
    # For history-level diff, we compare per-chunk aggregate values.
    # Detailed per-method diff requires reading per-chunk comparison.json files.
    for chunk in comparison.get("perChunk", []):
        slug = chunk.get("slug", "")
        agg = chunk.get("aggregate", {})
        method_map[slug] = {
            "chaosAotVsNet8Pct": agg.get("chaosAotVsNet8Pct", {}).get("mean"),
            "chaosJitVsNet8Pct": agg.get("chaosJitVsNet8Pct", {}).get("mean"),
            "net10VsNet8Pct": agg.get("net10VsNet8Pct", {}).get("mean"),
        }
    return method_map


def _build_per_method_map(
    comparison: dict,
    assembly: str,
    foundation_dir: Path,
) -> dict[str, dict[str, float | None]]:
    """Build per-method map from per-chunk comparison.json files.

    Reads each chunk's own comparison.json for full per-method detail.
    Falls back to per-chunk aggregate if comparison.json doesn't exist.
    """
    chunks_dir = foundation_dir / "chunks"

    per_method: dict[str, dict[str, float | None]] = {}

    for chunk in comparison.get("perChunk", []):
        slug = chunk.get("slug", "")
        comp_path = chunks_dir / slug / "results" / "comparison.json"
        if comp_path.exists():
            comp_data = json.loads(comp_path.read_text(encoding="utf-8"))
            for m in comp_data.get("methods", []):
                msid = m.get("methodSubjectId", "")
                if msid:
                    per_method[msid] = {
                        "chaosAotVsNet8Pct": m.get("chaosAotVsNet8Pct"),
                        "chaosJitVsNet8Pct": m.get("chaosJitVsNet8Pct"),
                        "net10VsNet8Pct": m.get("net10VsNet8Pct"),
                    }

    return per_method


def _diff_per_method(
    baseline_map: dict[str, dict[str, float | None]],
    target_map: dict[str, dict[str, float | None]],
    threshold: float,
) -> dict[str, Any]:
    """Diff per-method maps and classify changes.

    Returns regression report dict.
    """
    regressed: list[dict] = []
    improved: list[dict] = []
    new_methods: list[str] = []
    missing_methods: list[str] = []
    unchanged_count = 0

    all_keys = set(baseline_map) | set(target_map)

    for msid in sorted(all_keys):
        if msid not in baseline_map:
            new_methods.append(msid)
            continue
        if msid not in target_map:
            missing_methods.append(msid)
            continue

        b = baseline_map[msid].get("chaosAotVsNet8Pct")
        t = target_map[msid].get("chaosAotVsNet8Pct")
        if b is None or t is None:
            unchanged_count += 1
            continue

        delta = t - b
        entry = {
            "methodSubjectId": msid,
            "baselinePct": b,
            "targetPct": t,
            "delta": round(delta, 2),
        }

        if delta >= threshold:
            improved.append(entry)
        elif delta <= -threshold:
            regressed.append(entry)
        else:
            unchanged_count += 1

    return {
        "summary": {
            "total": len(all_keys),
            "regressed": len(regressed),
            "improved": len(improved),
            "unchanged": unchanged_count,
            "new": len(new_methods),
            "missing": len(missing_methods),
        },
        "details": {
            "regressed": regressed,
            "improved": improved,
            "new": new_methods,
            "missing": missing_methods,
        },
    }


def _diff_aggregate(
    baseline_comparison: dict,
    target_comparison: dict,
) -> dict:
    """Diff per-chunk aggregate values (for summary-level comparison)."""
    baseline_map = _build_method_map(baseline_comparison)
    target_map = _build_method_map(target_comparison)

    diffs: list[dict] = []
    for slug in sorted(set(baseline_map) | set(target_map)):
        b = baseline_map.get(slug, {})
        t = target_map.get(slug, {})
        b_aot = b.get("chaosAotVsNet8Pct")
        t_aot = t.get("chaosAotVsNet8Pct")
        delta_aot = round(t_aot - b_aot, 2) if (b_aot is not None and t_aot is not None) else None

        diffs.append({
            "slug": slug,
            "baselineChaosAotMeanPct": b_aot,
            "targetChaosAotMeanPct": t_aot,
            "deltaChaosAotPct": delta_aot,
        })

    return {"perChunk": diffs}


def _print_report(
    report: dict,
    baseline_date: str,
    target_date: str,
    threshold: float,
):
    """Print regression report to stdout."""
    summary = report["summary"]
    details = report["details"]

    print(f"{'='*60}")
    print(f"  Benchmark Regression Report")
    print(f"{'='*60}")
    print(f"  Baseline: {baseline_date}   Target: {target_date}   Threshold: {threshold}pp")
    print()

    if details["regressed"]:
        print(f"  Regressed ({summary['regressed']}):")
        for r in details["regressed"]:
            label = r.get("methodSubjectId") or r.get("slug", "?")
            msid_short = str(label)[:90]
            print(f"    🔴 {msid_short}")
            print(f"       {r['baselinePct']}% -> {r['targetPct']}%  ({r['delta']:+.2f}pp)")
        print()

    if details["improved"]:
        print(f"  Improved ({summary['improved']}):")
        for r in details["improved"][:10]:  # Show top 10 only
            label = r.get("methodSubjectId") or r.get("slug", "?")
            msid_short = str(label)[:90]
            print(f"    🟢 {msid_short}")
            print(f"       {r['baselinePct']}% -> {r['targetPct']}%  ({r['delta']:+.2f}pp)")
        if len(details["improved"]) > 10:
            print(f"    ... and {len(details['improved']) - 10} more")
        print()

    if details.get("new"):
        print(f"  New methods: {len(details['new'])}")
        print()
    if details.get("missing"):
        print(f"  Missing methods: {len(details['missing'])}")
        print()

    print(f"  Summary:")
    print(f"    Total:      {summary['total']}")
    print(f"    Regressed:  {summary['regressed']}")
    print(f"    Improved:   {summary['improved']}")
    print(f"    Unchanged:  {summary['unchanged']}")
    print(f"    New:        {summary['new']}")
    print(f"    Missing:    {summary['missing']}")
    print(f"{'='*60}")


def main():
    parser = argparse.ArgumentParser(
        description="Benchmark regression diff tool",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog=__doc__,
    )
    parser.add_argument("--baseline", required=True,
                        help="Path to baseline comparison-YYYY-MM-DD.json")
    parser.add_argument("--target", required=True,
                        help="Path to target comparison-YYYY-MM-DD.json")
    parser.add_argument("--foundation-dir", default=None,
                        help="Foundation DLL directory (for per-chunk comparison.json lookup)")
    parser.add_argument("--threshold", type=float, default=10,
                        help="Delta percentage-point threshold for regression (default: 10)")
    parser.add_argument("--output", default=None,
                        help="Write regression report JSON to this path")
    parser.add_argument("--per-method", action="store_true",
                        help="Enable per-method diff (requires --foundation-dir)")

    args = parser.parse_args()

    baseline_path = Path(args.baseline)
    target_path = Path(args.target)
    baseline = _load_comparison(baseline_path)
    target = _load_comparison(target_path)

    baseline_date = baseline_path.stem.replace("comparison-", "")
    target_date = target_path.stem.replace("comparison-", "")

    if args.per_method:
        if not args.foundation_dir:
            print("ERROR: --per-method requires --foundation-dir", file=sys.stderr)
            sys.exit(1)
        foundation_dir = Path(args.foundation_dir).resolve()
        baseline_map = _build_per_method_map(baseline, baseline.get("assemblyName", ""), foundation_dir)
        target_map = _build_per_method_map(target, target.get("assemblyName", ""), foundation_dir)
        report = _diff_per_method(baseline_map, target_map, args.threshold)
    else:
        # Aggregate-level diff (per chunk)
        report_summary = _diff_aggregate(baseline, target)
        # Build a simplified report
        regressed = [d for d in report_summary["perChunk"] if d.get("deltaChaosAotPct") is not None and d["deltaChaosAotPct"] <= -args.threshold]
        improved = [d for d in report_summary["perChunk"] if d.get("deltaChaosAotPct") is not None and d["deltaChaosAotPct"] >= args.threshold]
        unchanged = [d for d in report_summary["perChunk"] if d.get("deltaChaosAotPct") is not None and -args.threshold < d["deltaChaosAotPct"] < args.threshold]
        new_chunks = [d for d in report_summary["perChunk"] if d.get("baselineChaosAotMeanPct") is None and d.get("targetChaosAotMeanPct") is not None]
        missing_chunks = [d for d in report_summary["perChunk"] if d.get("targetChaosAotMeanPct") is None]

        report = {
            "summary": {
                "total": len(report_summary["perChunk"]),
                "regressed": len(regressed),
                "improved": len(improved),
                "unchanged": len(unchanged),
                "new": len(new_chunks),
                "missing": len(missing_chunks),
            },
            "details": {
                "regressed": [
                    {"slug": d["slug"], "baselinePct": d["baselineChaosAotMeanPct"],
                     "targetPct": d["targetChaosAotMeanPct"], "delta": d["deltaChaosAotPct"]}
                    for d in regressed
                ],
                "improved": [
                    {"slug": d["slug"], "baselinePct": d["baselineChaosAotMeanPct"],
                     "targetPct": d["targetChaosAotMeanPct"], "delta": d["deltaChaosAotPct"]}
                    for d in improved
                ],
                "new": [d["slug"] for d in new_chunks],
                "missing": [d["slug"] for d in missing_chunks],
            },
        }

    report["baselineDate"] = baseline_date
    report["targetDate"] = target_date
    report["threshold"] = args.threshold

    # Print report
    _print_report(report, baseline_date, target_date, args.threshold)

    # Write output JSON
    if args.output:
        output_path = Path(args.output)
        output_path.write_text(json.dumps(report, indent=2), encoding="utf-8")
        print(f"Report written to {output_path}")

    # Exit code: 1 if any regressions found
    if report["summary"]["regressed"] > 0:
        sys.exit(1)


if __name__ == "__main__":
    main()
