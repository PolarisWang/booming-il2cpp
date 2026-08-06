#!/usr/bin/env python3
"""
generate_report.py — Lightweight test dashboard generator for chaos-il2cpp.

Scans results/ and generates a self-contained dashboard.html with:
  - Summary stats (pass/fail/total)
  - Per-family pass/fail table with timestamps
  - Benchmark trend table (from JSONL history)
  - Stage-level breakdown

Usage:
  python generate_report.py
  python generate_report.py --output results/dashboard.html
  python generate_report.py --watch               # Regenerate on data changes
"""

import argparse
import json
import os
import sys
from collections import defaultdict
from datetime import datetime
from pathlib import Path

RESULTS_DIR = Path(__file__).resolve().parent.parent / "results"
OUTPUT = RESULTS_DIR / "dashboard.html"


def find_batch_reports() -> list[dict]:
    """Find all batch-report*.json files in results/ and subdirectories."""
    reports = []
    for path in sorted(RESULTS_DIR.rglob("batch-report*.json")):
        try:
            with open(path) as f:
                data = json.load(f)
            data["_path"] = str(path.relative_to(RESULTS_DIR))
            reports.append(data)
        except (json.JSONDecodeError, OSError):
            pass
    return reports


def find_jsonl_history() -> dict[str, list]:
    """Find all benchmark-history.jsonl files and group by family."""
    families = defaultdict(list)
    for path in sorted(RESULTS_DIR.rglob("perf/benchmark-history.jsonl")):
        # Extract family slug from path: results/foundation-dll/.../<family>/perf/
        family = path.parent.parent.name if path.parent.parent.name != "foundation-dll" else path.parent.name
        with open(path) as f:
            for line in f:
                try:
                    rec = json.loads(line.strip())
                    families[family].append(rec)
                except json.JSONDecodeError:
                    pass
    return dict(families)


def compute_trend(records: list[dict]) -> dict:
    """Compute trend summary for a family's benchmark history.

    Returns first/last elapsed_ms and pct change for each method.
    """
    if not records:
        return {}
    by_method = defaultdict(list)
    for r in records:
        key = r.get("methodSubjectId", f"method_{r.get('methodIndex', '?')}")
        by_method[key].append(r)

    trend = {}
    for method, entries in by_method.items():
        entries.sort(key=lambda x: x.get("timestamp", ""))
        first = entries[0].get("metrics", {}).get("elapsedMilliseconds", 0)
        last = entries[-1].get("metrics", {}).get("elapsedMilliseconds", 0)
        pct = ((last - first) / first * 100) if first > 0 else 0
        trend[method] = {
            "first_ts": entries[0].get("timestamp", "")[:16],
            "last_ts": entries[-1].get("timestamp", "")[:16],
            "first_ms": round(first, 3),
            "last_ms": round(last, 3),
            "pct_change": round(pct, 2),
            "count": len(entries),
        }
    return trend


def generate_html(reports: list[dict], history: dict) -> str:
    """Generate self-contained dashboard HTML."""
    # Compute aggregate stats
    if reports:
        latest = max(reports, key=lambda r: r.get("timestamp", ""))
        families = latest.get("results", [])
        total = len(families)
        passed = sum(1 for f in families if f.get("status") == "passed")
        failed_count = sum(1 for f in families if f.get("status") != "passed")
        elapsed_total = latest.get("elapsed_seconds", 0)
        timestamp = latest.get("timestamp", "unknown")
    else:
        total = passed = failed_count = 0
        elapsed_total = 0
        timestamp = "no data"
        families = []

    pass_rate = round(passed / total * 100, 1) if total > 0 else 0

    # Build per-family stage breakdown rows
    family_rows = []
    for f in families:
        slug = f.get("slug", "?")
        status = f.get("status", "?")
        dur = f.get("duration_seconds", 0)
        stages = f.get("stages", {})
        stage_badges = "".join(
            f'<span class="badge badge-{s.get("status", "na")}">{n}</span> '
            for n, s in stages.items()
        )
        coverage = f.get("coverage", {})
        cov_str = f"{coverage.get('stagesPassed', 0)}/{coverage.get('stagesTotal', 0)}"
        family_rows.append((slug, status, dur, stage_badges, cov_str))

    # Build trend table
    trend_rows = []
    for family_slug, records in sorted(history.items()):
        trend = compute_trend(records)
        for method, t in sorted(trend.items()):
            cls = "trend-fail" if t["pct_change"] > 15 else "trend-warn" if t["pct_change"] > 5 else "trend-ok"
            sign = "+" if t["pct_change"] > 0 else ""
            trend_rows.append(
                f"<tr><td>{family_slug}</td><td class='method-cell'>{method}</td>"
                f"<td>{t['first_ms']}</td><td>{t['last_ms']}</td>"
                f"<td class='{cls}'>{sign}{t['pct_change']}%</td>"
                f"<td>{t['count']}</td>"
                f"<td class='ts-cell'>{t['first_ts']}</td>"
                f"<td class='ts-cell'>{t['last_ts']}</td></tr>"
            )

    html = f"""<!DOCTYPE html>
<html lang="zh-CN">
<head>
<meta charset="UTF-8">
<title>Chaos IL2CPP Test Dashboard</title>
<style>
* {{ margin: 0; padding: 0; box-sizing: border-box; }}
body {{ font-family: -apple-system, "Microsoft YaHei", "Segoe UI", sans-serif; background: #f0f2f5; color: #333; }}
.header {{ background: linear-gradient(135deg, #1a237e, #283593); color: #fff; padding: 20px 32px; }}
.header h1 {{ font-size: 1.3rem; }}
.header .ts {{ font-size: 0.8rem; opacity: 0.7; margin-top: 4px; }}
.stats {{ display: flex; gap: 12px; padding: 16px 32px; background: #fff; border-bottom: 1px solid #e0e0e0; }}
.stat-card {{ padding: 8px 16px; text-align: center; border-radius: 6px; min-width: 80px; }}
.stat-card .num {{ font-size: 1.5rem; font-weight: 700; }}
.stat-card .label {{ font-size: 0.75rem; color: #888; }}
.stat-total .num {{ color: #1565c0; }}
.stat-pass .num {{ color: #2e7d32; }}
.stat-fail .num {{ color: #c62828; }}
.stat-rate .num {{ color: #f57f17; }}
.section {{ padding: 16px 32px; }}
.section h2 {{ font-size: 1rem; color: #555; margin-bottom: 12px; }}
table {{ width: 100%; border-collapse: collapse; background: #fff; border-radius: 8px; overflow: hidden; box-shadow: 0 1px 3px rgba(0,0,0,0.08); }}
th {{ text-align: left; padding: 10px 12px; background: #fafafa; border-bottom: 2px solid #eee; font-size: 0.8rem; color: #666; }}
td {{ padding: 8px 12px; border-bottom: 1px solid #f5f5f5; font-size: 0.85rem; }}
tr:hover td {{ background: #f8f9ff; }}
.slug-cell {{ font-family: 'Consolas', monospace; font-weight: 600; }}
.method-cell {{ font-family: 'Consolas', monospace; font-size: 0.78rem; max-width: 400px; overflow: hidden; text-overflow: ellipsis; }}
.ts-cell {{ color: #999; font-size: 0.78rem; }}
.badge {{ display: inline-block; padding: 1px 8px; border-radius: 8px; font-size: 0.72rem; margin: 1px; }}
.badge-passed {{ background: #e8f5e9; color: #2e7d32; }}
.badge-failed {{ background: #fbe9e7; color: #c62828; }}
.badge-skipped {{ background: #fff8e1; color: #f57f17; }}
.status-passed {{ color: #2e7d32; font-weight: 600; }}
.status-failed {{ color: #c62828; font-weight: 600; }}
.trend-ok {{ color: #2e7d32; }}
.trend-warn {{ color: #f57f17; }}
.trend-fail {{ color: #c62828; font-weight: 600; }}
.footer {{ padding: 16px 32px; color: #999; font-size: 0.78rem; border-top: 1px solid #e0e0e0; margin-top: 24px; }}
</style>
</head>
<body>
<div class="header">
  <h1>Chaos IL2CPP Test Dashboard</h1>
  <div class="ts">Last batch: {timestamp} | Generated: {datetime.now().strftime('%Y-%m-%d %H:%M')}</div>
</div>
<div class="stats">
  <div class="stat-card stat-total"><div class="num">{total}</div><div class="label">Families</div></div>
  <div class="stat-card stat-pass"><div class="num">{passed}</div><div class="label">Passed</div></div>
  <div class="stat-card stat-fail"><div class="num">{failed_count}</div><div class="label">Failed</div></div>
  <div class="stat-card stat-rate"><div class="num">{pass_rate}%</div><div class="label">Pass Rate</div></div>
  <div class="stat-card stat-total"><div class="num">{elapsed_total:.0f}s</div><div class="label">Total Time</div></div>
</div>
"""

    # Family table
    html += """<div class="section">
<h2>Family Results</h2>
<table>
<tr><th>Family</th><th>Status</th><th>Duration (s)</th><th>Stages</th><th>Coverage</th></tr>
"""
    for slug, status, dur, stages, cov in family_rows:
        status_cls = f"status-{status}"
        html += f"<tr><td class='slug-cell'>{slug}</td>"
        html += f"<td class='{status_cls}'>{status}</td>"
        html += f"<td>{dur:.1f}</td><td>{stages}</td><td>{cov}</td></tr>"
    html += "</table></div>"

    # Trend table
    if trend_rows:
        html += """<div class="section">
<h2>Benchmark Trends</h2>
<table>
<tr><th>Family</th><th>Method</th><th>First (ms)</th><th>Last (ms)</th><th>Change</th><th>Runs</th><th>First Run</th><th>Last Run</th></tr>
"""
        html += "".join(trend_rows)
        html += "</table></div>"

    # Report history summary
    if len(reports) > 1:
        # Older batch reports
        html += """<div class="section">
<h2>Report History</h2>
<table>
<tr><th>File</th><th>Timestamp</th><th>Passed</th><th>Failed</th><th>Total</th></tr>
"""
        for r in sorted(reports, key=lambda x: x.get("timestamp", "")):
            fname = r.get("_path", "?")
            ts = r.get("timestamp", "?")
            p = r.get("passed", 0)
            fl = r.get("failed", 0)
            t = r.get("total_families", 0)
            html += f"<tr><td>{fname}</td><td>{ts}</td><td>{p}</td><td>{fl}</td><td>{t}</td></tr>"
        html += "</table></div>"

    html += f"""<div class="footer">
Chaos IL2CPP — Auto-generated by generate_report.py at {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}
</div>
</body></html>"""
    return html


def main():
    parser = argparse.ArgumentParser(description="Generate test dashboard")
    parser.add_argument("--output", default=str(OUTPUT), help="Output HTML path")
    parser.add_argument("--watch", action="store_true", help="Regenerate on changes (not yet implemented)")
    args = parser.parse_args()

    reports = find_batch_reports()
    history = find_jsonl_history()

    if not reports and not history:
        print("WARNING: No batch reports or benchmark history found in", RESULTS_DIR)
        print("Run a test batch first to populate data.")

    html = generate_html(reports, history)
    output_path = Path(args.output)
    output_path.parent.mkdir(parents=True, exist_ok=True)
    with open(output_path, "w", encoding="utf-8") as f:
        f.write(html)

    print(f"Dashboard generated: {output_path}")
    print(f"  Batch reports: {len(reports)}")
    print(f"  Families: {len(reports[0].get('results', [])) if reports else 0}")
    print(f"  Benchmark history: {sum(len(v) for v in history.values()) if history else 0} records")


if __name__ == "__main__":
    main()
