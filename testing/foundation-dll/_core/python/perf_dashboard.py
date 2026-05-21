"""Performance Dashboard — self-contained HTML with Chart.js visualizations.

Generates a static HTML page showing:
  - Technology summary table (geometric mean ops/s per technology)
  - Per-method grouped bar chart (all technologies side by side)
  - Historical trend view over multiple runs

Usage:
    from _core.python.perf_dashboard import generate_dashboard
    html = generate_dashboard(ctx)
    Path("dashboard.html").write_text(html, encoding="utf-8")
"""

from __future__ import annotations

import json
import math
import sys
from pathlib import Path
from typing import Any

# Ensure _core package is importable
_HERE = Path(__file__).resolve().parent
_CORE_PACKAGE = _HERE.parents[1]
if str(_CORE_PACKAGE) not in sys.path:
    sys.path.insert(0, str(_CORE_PACKAGE))

# Ensure testing/ is importable for _pipeline shared components
_TESTING_ROOT = _CORE_PACKAGE.parent
if str(_TESTING_ROOT) not in sys.path:
    sys.path.insert(0, str(_TESTING_ROOT))

from _core.python.models import FamilyContext
from _core.python.perf_compare import (
    TECHNOLOGY_NAMES,
    compare_across_technologies,
    geometric_mean,
    summarize_technologies,
)
from _core.python.perf_store import _load_jsonl, _store_dir


_DEFAULT_TECHNOLOGIES = [
    "chaos-aot",
    "chaos-jit",
    "chaos-hu-aot",
    "chaos-hu-jit",
    "net8-jit",
    "net10-jit",
    "mono",
]


def _short_label(mid: str) -> str:
    if "::" in mid:
        return mid.rsplit("::", 1)[-1]
    return mid


def _load_benchmark_history(ctx: FamilyContext) -> list[dict[str, Any]]:
    path = _store_dir(ctx) / "benchmark-history.jsonl"
    return _load_jsonl(path)


def _group_by_timestamp(
    records: list[dict[str, Any]],
) -> list[tuple[str, list[dict[str, Any]]]]:
    groups: dict[str, list[dict[str, Any]]] = {}
    for r in records:
        ts = r.get("timestamp", "")
        groups.setdefault(ts, []).append(r)
    sorted_ts = sorted(groups.keys())
    return [(ts, groups[ts]) for ts in sorted_ts]


def generate_dashboard(ctx: FamilyContext) -> str:
    """Generate a self-contained HTML dashboard page."""

    # ── Gather data ──────────────────────────────────────────────────
    summaries = summarize_technologies(ctx)
    rows = compare_across_technologies(ctx)
    history_records = _load_benchmark_history(ctx)

    # Technology colors
    tech_colors = {
        "chaos-aot": "rgba(54, 162, 235, 0.85)",
        "chaos-jit": "rgba(255, 159, 64, 0.85)",
        "chaos-hu-aot": "rgba(75, 192, 192, 0.85)",
        "chaos-hu-jit": "rgba(153, 102, 255, 0.85)",
        "net8-jit": "rgba(255, 99, 132, 0.85)",
        "net10-jit": "rgba(255, 205, 86, 0.85)",
        "mono": "rgba(201, 203, 207, 0.85)",
    }

    # Determine which technologies have data
    active_techs = [s.technology for s in summaries if s.ok_count > 0]
    if not active_techs:
        active_techs = _DEFAULT_TECHNOLOGIES[:]
    display_techs = [t for t in active_techs if t in TECHNOLOGY_NAMES]

    # Per-method data for bar chart
    method_labels = [r.label for r in rows]
    method_datasets = []
    for tech in display_techs:
        color = tech_colors.get(tech, "rgba(150, 150, 150, 0.85)")
        data_vals = []
        for r in rows:
            v = r.values.get(tech, 0)
            if v > 1e9:
                v_rounded = round(v / 1e9, 2)
            elif v > 1e6:
                v_rounded = round(v / 1e6, 2)
            else:
                v_rounded = round(v, 0)
            data_vals.append(v_rounded)
        method_datasets.append({
            "label": TECHNOLOGY_NAMES.get(tech, tech),
            "data": data_vals,
            "backgroundColor": color,
            "borderColor": color.replace("0.85", "1"),
            "borderWidth": 1,
        })

    # History data (grouped by timestamp)
    history_ts_groups = _group_by_timestamp(history_records)
    # Only keep groups with at least one of the active technologies
    history_datasets: list[dict[str, Any]] = []
    history_labels: list[str] = []

    if len(history_ts_groups) >= 2:
        # Build per-technology series
        tech_series: dict[str, list[float]] = {}
        for ts, group in history_ts_groups:
            history_labels.append(ts[-8:] if len(ts) > 8 else ts)  # HH:MM:SS
            # Group by technology
            tech_ops: dict[str, list[float]] = {}
            for r in group:
                t = r.get("technology", "")
                ops = r.get("metrics", {}).get("opsPerSecond", 0)
                if ops > 0:
                    tech_ops.setdefault(t, []).append(ops)
            for tech in display_techs:
                ops_list = tech_ops.get(tech, [])
                gm = geometric_mean(ops_list) if ops_list else 0
                tech_series.setdefault(tech, []).append(gm)

        for tech in display_techs:
            color = tech_colors.get(tech, "rgba(150,150,150,0.85)")
            vals = tech_series.get(tech, [])
            if any(v > 0 for v in vals):
                history_datasets.append({
                    "label": TECHNOLOGY_NAMES.get(tech, tech),
                    "data": vals,
                    "borderColor": color.replace("0.85", "1"),
                    "backgroundColor": color,
                    "fill": False,
                    "tension": 0.2,
                })

    # Format large numbers for display
    def _fmt_ops(v: float) -> str:
        if v >= 1e9:
            return f"{v/1e9:.2f}B"
        if v >= 1e6:
            return f"{v/1e6:.2f}M"
        if v >= 1e3:
            return f"{v/1e3:.1f}K"
        return f"{v:.0f}"

    # ── Build HTML ────────────────────────────────────────────────────
    contract_name = f"{ctx.assembly} / {ctx.slug}"

    # Overview table rows (from summaries)
    table_rows = ""
    for s in summaries:
        name = TECHNOLOGY_NAMES.get(s.technology, s.technology)
        gm_str = _fmt_ops(s.geometric_mean_ops) if s.ok_count > 0 else "—"
        status = "yes" if s.ok_count > 0 else "no"
        table_rows += (
            f"            <tr>\n"
            f"              <td>{name}</td>\n"
            f"              <td>{s.method_count}</td>\n"
            f"              <td>{s.ok_count}</td>\n"
            f"              <td>{gm_str}</td>\n"
            f"              <td class=\"{status}\">{'&#10003;' if s.ok_count > 0 else '&#10007;'}</td>\n"
            f"            </tr>\n"
        )

    # Raw data for Chart.js (embedded as JSON)
    chart_data = json.dumps({
        "labels": method_labels,
        "datasets": method_datasets,
    })
    history_data = json.dumps({
        "labels": history_labels,
        "datasets": history_datasets,
    })

    # Per-method detail data (precomputed to avoid f-string issues with braces)
    rows_detail = []
    for r in rows:
        techs = {TECHNOLOGY_NAMES.get(t, t): _fmt_ops(r.values.get(t, 0)) for t in display_techs}
        rows_detail.append({
            "label": r.label[:40],
            "mid": r.method_subject_id,
            "index": r.method_index,
            "techs": techs,
        })
    rows_json = json.dumps(rows_detail)

    html = f"""<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>Performance Dashboard — {contract_name}</title>
<script src="https://cdn.jsdelivr.net/npm/chart.js@4.4.7/dist/chart.umd.min.js"></script>
<style>
  * {{ margin: 0; padding: 0; box-sizing: border-box; }}
  body {{ font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif; background: #f5f5f5; color: #333; padding: 24px; }}
  h1 {{ font-size: 1.6rem; margin-bottom: 4px; }}
  .subtitle {{ color: #666; font-size: 0.9rem; margin-bottom: 24px; }}
  .card {{ background: #fff; border-radius: 8px; box-shadow: 0 1px 3px rgba(0,0,0,0.12); padding: 20px; margin-bottom: 20px; }}
  .card h2 {{ font-size: 1.1rem; margin-bottom: 12px; color: #444; }}
  table {{ width: 100%; border-collapse: collapse; }}
  th, td {{ padding: 8px 12px; text-align: left; border-bottom: 1px solid #eee; font-size: 0.9rem; }}
  th {{ font-weight: 600; color: #555; }}
  td.yes {{ color: #2e7d32; }}
  td.no {{ color: #c62828; }}
  .chart-container {{ position: relative; height: 400px; }}
  .chart-row {{ display: flex; gap: 20px; flex-wrap: wrap; }}
  .chart-col {{ flex: 1; min-width: 400px; }}
  .method-grid {{ display: grid; grid-template-columns: repeat(auto-fill, minmax(280px, 1fr)); gap: 12px; }}
  .method-card {{ background: #fafafa; border: 1px solid #e0e0e0; border-radius: 6px; padding: 12px; }}
  .method-card .name {{ font-size: 0.8rem; font-family: 'Consolas', 'Courier New', monospace; word-break: break-all; margin-bottom: 6px; color: #555; }}
  .method-card .ops {{ font-size: 1.1rem; font-weight: 600; }}
  .method-card .tech {{ font-size: 0.75rem; color: #888; }}
  .status-dot {{ display: inline-block; width: 8px; height: 8px; border-radius: 50%; margin-right: 6px; }}
  .status-dot.yes {{ background: #4caf50; }}
  .status-dot.no {{ background: #f44336; }}
  @media (max-width: 700px) {{ .chart-col {{ min-width: 100%; }} }}
</style>
</head>
<body>

<h1>{contract_name}</h1>
<p class="subtitle">Generated at {__import__('time').strftime('%Y-%m-%d %H:%M:%S')} &mdash; {len(rows)} methods across {len(display_techs)} technologies</p>

<div class="card">
  <h2>Technology Overview</h2>
  <table>
    <thead>
      <tr><th>Technology</th><th>Methods</th><th>OK</th><th>GeoMean ops/s</th><th>Data</th></tr>
    </thead>
    <tbody>
{table_rows}
    </tbody>
  </table>
</div>

<div class="card">
  <h2>Cross-Technology Comparison</h2>
  <div class="chart-container">
    <canvas id="barChart"></canvas>
  </div>
</div>

<div class="card">
  <h2>Per-Method Details</h2>
  <div class="method-grid" id="methodGrid">
  </div>
</div>

<script>
const barData = {chart_data};
const historyData = {history_data};

new Chart(document.getElementById('barChart'), {{
  type: 'bar',
  data: barData,
  options: {{
    responsive: true,
    maintainAspectRatio: false,
    interaction: {{ mode: 'index', intersect: false }},
    plugins: {{
      legend: {{ position: 'top' }},
      tooltip: {{
        callbacks: {{
          label: function(ctx) {{
            let v = ctx.raw;
            if (v >= 1000) return ctx.dataset.label + ': ' + v.toLocaleString();
            return ctx.dataset.label + ': ' + v;
          }}
        }}
      }}
    }},
    scales: {{
      x: {{ grid: {{ display: false }} }},
      y: {{
        beginAtZero: true,
        title: {{ display: true, text: 'ops/s (scaled)' }}
      }}
    }}
  }}
}});

// Per-method detail cards
const rows = {rows_json};

const grid = document.getElementById('methodGrid');
rows.forEach(function(row) {{
  let card = document.createElement('div');
  card.className = 'method-card';
  let techHtml = '';
  for (let [tech, ops] of Object.entries(row.techs)) {{
    techHtml += '<div class="tech">' + tech + ': <span class="ops">' + ops + '</span> ops/s</div>';
  }}
  card.innerHTML = '<div class="name">#' + row.index + ' ' + row.mid + '</div>' + techHtml;
  grid.appendChild(card);
}});

if (historyData.labels.length > 1) {{
  var histCanvas = document.createElement('canvas');
  histCanvas.id = 'historyChart';
  document.querySelector('.card:last-child').insertAdjacentHTML('afterend',
    '<div class="card"><h2>Historical Trend</h2><div class="chart-container"><canvas id="historyChart"></canvas></div></div>'
  );
  new Chart(document.getElementById('historyChart'), {{
    type: 'line',
    data: historyData,
    options: {{
      responsive: true,
      maintainAspectRatio: false,
      plugins: {{ legend: {{ position: 'top' }} }},
      scales: {{
        x: {{ grid: {{ display: false }} }},
        y: {{ beginAtZero: true, title: {{ display: true, text: 'Geometric Mean ops/s' }} }}
      }}
    }}
  }});
}}
</script>
</body>
</html>"""
    return html


def main() -> None:
    import argparse

    parser = argparse.ArgumentParser(description="Generate performance dashboard HTML")
    parser.add_argument("family_slug", help="Family slug (e.g., convert-char)")
    parser.add_argument("--assembly", default="System.Private.CoreLib")
    parser.add_argument("--output", "-o", default=None, help="Output HTML path")

    args = parser.parse_args()

    from pathlib import Path as _Path
    _here = _Path(__file__).resolve().parent
    _core_pkg = _here.parents[1]
    _slug_dir = _core_pkg / args.assembly / args.family_slug

    ctx = FamilyContext(
        slug=args.family_slug,
        assembly=args.assembly,
        root_dir=_slug_dir,
    )

    html = generate_dashboard(ctx)

    output_path = args.output or _slug_dir / "perf-dashboard.html"
    output_path = _Path(output_path)
    output_path.write_text(html, encoding="utf-8")
    print(f"Dashboard written to: {output_path}")


if __name__ == "__main__":
    main()
