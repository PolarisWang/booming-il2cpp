"""Batch Summary Dashboard — generate HTML overview of all families.

Usage:
    python batch_summary_dashboard.py [--report path/to/batch-report.json]
"""

from __future__ import annotations

import json
import time
from pathlib import Path

_HERE = Path(__file__).resolve().parent
_CORE_PACKAGE = _HERE.parents[1]
_TESTING_ROOT = _CORE_PACKAGE.parent


def generate_summary_html(report_path: Path) -> str:
    report = json.loads(report_path.read_text(encoding="utf-8"))
    results = report.get("results", [])

    passed = [r for r in results if r.get("status") == "passed"]
    failed = [r for r in results if r.get("status") != "passed"]

    total = len(results)
    n_passed = len(passed)
    n_failed = len(failed)
    pass_pct = round(n_passed / total * 100) if total > 0 else 0
    elapsed = report.get("elapsed_seconds", 0)
    elapsed_str = f"{elapsed // 60:.0f}m {elapsed % 60:.0f}s"
    timestamp = report.get("timestamp", time.strftime("%Y-%m-%dT%H:%M:%S"))
    assembly = report.get("assembly", "?")

    passed_rows = ""
    for r in passed:
        dur = r.get("duration_seconds", 0)
        label = f"{dur:.0f}s" if dur else "&nbsp;"
        passed_rows += (
            f"            <tr class=\"passed\">"
            f"<td><span class=\"dot dot-pass\"></span></td>"
            f"<td>{r['slug']}</td>"
            f"<td>passed</td>"
            f"<td>{label}</td></tr>\n"
        )

    failed_rows = ""
    for r in failed:
        dur = r.get("duration_seconds", 0)
        label = f"{dur:.0f}s" if dur else "&nbsp;"
        failed_rows += (
            f"            <tr class=\"failed\">"
            f"<td><span class=\"dot dot-fail\"></span></td>"
            f"<td>{r['slug']}</td>"
            f"<td>failed</td>"
            f"<td>{label}</td></tr>\n"
        )

    html = f"""<!DOCTYPE html>
<html lang="zh-CN">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>Foundation-DLL Batch Summary — {assembly}</title>
<style>
  * {{ margin: 0; padding: 0; box-sizing: border-box; }}
  body {{ font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif; background: #f0f2f5; color: #333; padding: 24px; }}
  h1 {{ font-size: 1.5rem; }}
  .subtitle {{ color: #666; font-size: 0.9rem; margin-bottom: 16px; }}

  .stats {{ display: flex; gap: 16px; margin-bottom: 20px; flex-wrap: wrap; }}
  .stat-card {{ background: #fff; border-radius: 8px; box-shadow: 0 1px 3px rgba(0,0,0,0.10); padding: 16px 24px; min-width: 120px; text-align: center; }}
  .stat-card .num {{ font-size: 2rem; font-weight: 700; }}
  .stat-card .label {{ font-size: 0.8rem; color: #888; margin-top: 4px; }}
  .stat-card.pass .num {{ color: #2e7d32; }}
  .stat-card.fail .num {{ color: #c62828; }}
  .stat-card.total .num {{ color: #1565c0; }}
  .stat-card.rate .num {{ color: #f57f17; }}

  .card {{ background: #fff; border-radius: 8px; box-shadow: 0 1px 3px rgba(0,0,0,0.10); padding: 20px; margin-bottom: 16px; }}
  .card h2 {{ font-size: 1rem; color: #555; margin-bottom: 12px; }}

  table {{ width: 100%; border-collapse: collapse; font-size: 0.9rem; }}
  th {{ text-align: left; padding: 8px 10px; border-bottom: 2px solid #eee; font-weight: 600; color: #555; }}
  td {{ padding: 6px 10px; border-bottom: 1px solid #f0f0f0; }}

  .dot {{ display: inline-block; width: 8px; height: 8px; border-radius: 50%; }}
  .dot-pass {{ background: #4caf50; }}
  .dot-fail {{ background: #f44336; }}

  .passed td:first-child {{ border-left: 3px solid #4caf50; }}
  .failed td:first-child {{ border-left: 3px solid #f44336; }}

  .progress-bar {{ height: 8px; border-radius: 4px; background: #e0e0e0; margin-top: 8px; overflow: hidden; }}
  .progress-fill {{ height: 100%; border-radius: 4px; background: linear-gradient(90deg, #4caf50, #81c784); transition: width 0.5s; }}

  .tag {{ display: inline-block; padding: 2px 8px; border-radius: 10px; font-size: 0.75rem; font-weight: 600; }}
  .tag-pass {{ background: #e8f5e9; color: #2e7d32; }}
  .tag-fail {{ background: #fbe9e7; color: #c62828; }}

  @media (max-width: 600px) {{
    .stats {{ gap: 8px; }}
    .stat-card {{ padding: 12px 16px; min-width: 80px; }}
    .stat-card .num {{ font-size: 1.4rem; }}
  }}

  .legend {{ display: flex; gap: 20px; margin-bottom: 12px; font-size: 0.85rem; color: #666; }}
</style>
</head>
<body>

<h1>{assembly} — Family Verification Summary</h1>
<p class="subtitle">{timestamp} &mdash; {total} families in {elapsed_str}</p>

<div class="stats">
  <div class="stat-card total">
    <div class="num">{total}</div>
    <div class="label">Total</div>
  </div>
  <div class="stat-card pass">
    <div class="num">{n_passed}</div>
    <div class="label">Passed</div>
  </div>
  <div class="stat-card fail">
    <div class="num">{n_failed}</div>
    <div class="label">Failed</div>
  </div>
  <div class="stat-card rate">
    <div class="num">{pass_pct}%</div>
    <div class="label">Pass Rate</div>
  </div>
</div>

<div class="card">
  <h2>Progress</h2>
  <div class="progress-bar">
    <div class="progress-fill" style="width: {pass_pct}%"></div>
  </div>
</div>

<div class="card">
  <h2>Results</h2>
  <div class="legend">
    <span><span class="dot dot-pass"></span> Passed ({n_passed})</span>
    <span><span class="dot dot-fail"></span> Failed ({n_failed})</span>
  </div>
  <table>
    <thead>
      <tr><th></th><th>Family</th><th>Status</th><th>Duration</th></tr>
    </thead>
    <tbody>
{passed_rows}{failed_rows}
    </tbody>
  </table>
</div>

</body>
</html>"""
    return html


def main() -> None:
    import argparse
    parser = argparse.ArgumentParser(description="Generate batch summary HTML")
    parser.add_argument("--report", default=None, help="Path to batch-report.json")
    parser.add_argument("--output", "-o", default=None, help="Output HTML path")

    args = parser.parse_args()

    if args.report:
        report_path = Path(args.report)
    else:
        report_path = _TESTING_ROOT / "results" / "batch-report.json"

    if not report_path.exists():
        print(f"Report not found: {report_path}")
        return

    html = generate_summary_html(report_path)

    if args.output:
        output_path = Path(args.output)
    else:
        output_path = report_path.with_suffix(".html")

    output_path.write_text(html, encoding="utf-8")
    print(f"Summary dashboard written to: {output_path}")


if __name__ == "__main__":
    main()
