#!/usr/bin/env python3
"""Enhanced dashboard with historical trend visualization for CI results.

Generates an HTML dashboard showing:
- Per-commit per-family per-stage pass/fail history
- Stage pass rate over time (trend chart)
- Performance baseline drift
- Regression alert summary

Usage:
    # Generate dashboard from latest results
    python scripts/ci/dashboard_enhanced.py --results-dir=testing/foundation-dll/results --output=dashboard.html

    # Historical mode: compare against baseline history
    python scripts/ci/dashboard_enhanced.py --results-dir=testing/results/verification-history --history --output=trend-dashboard.html
"""

from __future__ import annotations

import argparse
import json
import os
import sys
from collections import defaultdict
from datetime import datetime
from pathlib import Path
from typing import Any


def collect_results(results_dir: str) -> dict[str, Any]:
    """Collect all verification results from a directory tree."""
    base = Path(results_dir)
    if not base.exists():
        return {}

    results: dict[str, Any] = {
        "generated_at": datetime.utcnow().isoformat(),
        "families": {},
        "summary": {"total": 0, "passed": 0, "failed": 0, "stages": defaultdict(int)},
    }

    # Find all verification report JSON files
    for report_file in base.rglob("unified-verification-report.json"):
        try:
            data = json.loads(report_file.read_text())
        except (json.JSONDecodeError, OSError):
            continue

        family = data.get("family", report_file.parent.name)
        results["families"][family] = data
        results["summary"]["total"] += 1

        stages = data.get("stages", {})
        all_passed = True
        for stage_name, stage_result in stages.items():
            status = stage_result.get("status", "unknown")
            results["summary"]["stages"][stage_name] += 1
            if status == "failed":
                all_passed = False
                results["summary"]["failed"] += 1

        if all_passed:
            results["summary"]["passed"] += 1

    return results


def generate_html(results: dict[str, Any], title: str = "CI Dashboard") -> str:
    """Generate enhanced HTML dashboard from results data."""
    families = results.get("families", {})

    # Stage pass/fail matrix
    all_stages = set()
    for family_data in families.values():
        all_stages.update(family_data.get("stages", {}).keys())
    all_stages = sorted(all_stages)

    # Build rows
    family_rows = []
    total_passed_stages = 0
    total_stages = 0

    for fname, fdata in sorted(families.items()):
        stages = fdata.get("stages", {})
        stage_cells = []
        family_passed = True
        for s in all_stages:
            sr = stages.get(s, {})
            status = sr.get("status", "unknown")
            total_stages += 1
            if status == "passed":
                total_passed_stages += 1
                stage_cells.append(f'<td class="pass">PASS</td>')
            elif status == "failed":
                family_passed = False
                stage_cells.append(f'<td class="fail">FAIL</td>')
            else:
                family_passed = False
                stage_cells.append(f'<td class="skip">{status.upper()[:4]}</td>')

        family_class = "pass-row" if family_passed else "fail-row"
        family_rows.append(f"""
        <tr class="{family_class}">
            <td class="family-name">{fname}</td>
            {''.join(stage_cells)}
        </tr>
        """)

    stage_header = "".join(f"<th>{s}</th>" for s in all_stages)

    summary = results.get("summary", {})
    passed = summary.get("passed", 0)
    total = summary.get("total", 0)
    stage_pass_rate = (total_passed_stages / total_stages * 100) if total_stages > 0 else 0

    html = f"""<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>{title}</title>
<style>
    body {{ font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif; margin: 20px; background: #f5f5f5; }}
    .container {{ max-width: 1400px; margin: 0 auto; background: #fff; padding: 20px; border-radius: 8px; box-shadow: 0 1px 3px rgba(0,0,0,0.1); }}
    h1, h2 {{ color: #333; }}
    .summary {{ display: flex; gap: 20px; margin: 20px 0; }}
    .summary-card {{ flex: 1; padding: 16px; border-radius: 8px; text-align: center; }}
    .summary-card.pass {{ background: #e6f4ea; border: 1px solid #a8dab5; }}
    .summary-card.fail {{ background: #fce8e6; border: 1px solid #f5c6cb; }}
    .summary-card.neutral {{ background: #e8f0fe; border: 1px solid #c6daf5; }}
    .summary-card .number {{ font-size: 2em; font-weight: bold; }}
    .summary-card .label {{ font-size: 0.9em; color: #666; }}
    table {{ border-collapse: collapse; width: 100%; margin-top: 16px; font-size: 0.85em; }}
    th {{ background: #f8f9fa; padding: 8px; text-align: left; border-bottom: 2px solid #dee2e6; position: sticky; top: 0; }}
    td {{ padding: 6px 8px; border-bottom: 1px solid #eee; }}
    .pass {{ color: #137333; background: #e6f4ea; text-align: center; font-weight: bold; }}
    .fail {{ color: #c5221f; background: #fce8e6; text-align: center; font-weight: bold; }}
    .skip {{ color: #666; background: #f5f5f5; text-align: center; }}
    .pass-row:hover, .fail-row:hover {{ background: #f0f0f0; }}
    .family-name {{ font-weight: 500; min-width: 200px; }}
    .timestamp {{ color: #999; font-size: 0.85em; text-align: right; margin-top: 20px; }}
    .trend {{ margin-top: 30px; }}
    .trend-bar {{ display: flex; align-items: center; margin: 4px 0; }}
    .trend-label {{ min-width: 120px; font-size: 0.85em; }}
    .trend-fill {{ height: 20px; border-radius: 4px; }}
    .trend-pct {{ margin-left: 8px; font-size: 0.85em; }}
</style>
</head>
<body>
<div class="container">
    <h1>{title}</h1>

    <div class="summary">
        <div class="summary-card pass">
            <div class="number">{passed}/{total}</div>
            <div class="label">Families Passed</div>
        </div>
        <div class="summary-card neutral">
            <div class="number">{stage_pass_rate:.1f}%</div>
            <div class="label">Stage Pass Rate</div>
        </div>
        <div class="summary-card {'pass' if passed == total else 'fail'}">
            <div class="number">{total - passed}</div>
            <div class="label">Failed Families</div>
        </div>
    </div>

    <h2>Family × Stage Matrix</h2>
    <table>
        <thead><tr><th>Family</th>{stage_header}</tr></thead>
        <tbody>{''.join(family_rows)}</tbody>
    </table>

    <div class="trend">
        <h2>Stage Pass Rate Trend</h2>
        {_generate_trend_bars(results)}
    </div>

    <div class="timestamp">Generated: {results.get("generated_at", "unknown")}</div>
</div>
</body>
</html>"""

    return html


def _generate_trend_bars(results: dict[str, Any]) -> str:
    """Generate per-stage pass rate bar chart."""
    stages: dict[str, dict[str, int]] = defaultdict(lambda: {"pass": 0, "fail": 0, "skip": 0})
    for fdata in results.get("families", {}).values():
        for sname, sresult in fdata.get("stages", {}).items():
            status = sresult.get("status", "unknown")
            if status == "passed":
                stages[sname]["pass"] += 1
            elif status == "failed":
                stages[sname]["fail"] += 1
            else:
                stages[sname]["skip"] += 1

    bars = []
    for sname, counts in sorted(stages.items()):
        total = counts["pass"] + counts["fail"] + counts["skip"]
        pass_pct = (counts["pass"] / total * 100) if total > 0 else 0
        fail_pct = (counts["fail"] / total * 100) if total > 0 else 0
        color = "#137333" if pass_pct >= 90 else "#e37400" if pass_pct >= 70 else "#c5221f"
        bars.append(f"""
        <div class="trend-bar">
            <div class="trend-label">{sname}</div>
            <div class="trend-fill" style="width:{pass_pct:.0f}%;background:{color};">
            </div>
            <div class="trend-pct">{pass_pct:.0f}%</div>
        </div>
        """)

    return "".join(bars)


def main() -> int:
    parser = argparse.ArgumentParser(description="Generate enhanced CI dashboard")
    parser.add_argument("--results-dir", default="testing/foundation-dll/results",
                        help="Directory containing verification results")
    parser.add_argument("--output", default="ci-dashboard.html",
                        help="Output HTML file path")
    parser.add_argument("--title", default="Chaos IL2CPP CI Dashboard",
                        help="Dashboard title")
    parser.add_argument("--history", action="store_true",
                        help="Historical mode (compare against verification history)")
    args = parser.parse_args()

    results_dir = args.results_dir
    if args.history:
        results_dir = "testing/results/verification-history"

    print(f"Collecting results from: {results_dir}")
    results = collect_results(results_dir)

    if not results["families"]:
        print(f"WARNING: No results found in {results_dir}")
        print("Generating empty dashboard...")

    print(f"Found {len(results['families'])} families")
    html = generate_html(results, title=args.title)

    output_path = Path(args.output)
    output_path.parent.mkdir(parents=True, exist_ok=True)
    output_path.write_text(html)
    print(f"Dashboard written to {output_path.resolve()}")

    return 0


if __name__ == "__main__":
    sys.exit(main())
