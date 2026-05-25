"""Pipeline Verification HTML Dashboard — self-contained HTML report.

Reads unified-verification-report.json and renders a full 15-stage dashboard
with stage status, managed fact results, benchmark data, and key ratios.
"""

from __future__ import annotations

import json
from pathlib import Path
from typing import Any


def _status_badge(status: str) -> str:
    colors = {"passed": "#4caf50", "failed": "#f44336",
              "skipped": "#ff9800", "error": "#9c27b0"}
    c = colors.get(status, "#999")
    return f'<span style="display:inline-block;padding:2px 10px;border-radius:10px;font-size:0.8rem;font-weight:600;color:#fff;background:{c}">{status}</span>'


def _stage_label(stage: str) -> str:
    labels = {
        "preflight": "0 Preflight",
        "codegen": "1 Codegen (AOT)",
        "jit_codegen": "2 JitCodegen",
        "managed_fact": "3 Fact (.NET8)",
        "fact": "4 Fact AOT",
        "fact_jit": "5 Fact JIT",
        "audit": "6 Audit",
        "asm_compare": "7 AsmCompare",
        "microbench": "8 Microbench",
        "benchmark": "9 Benchmark (5-way)",
        "hotupdate": "10 HotUpdate AOT Fact",
        "hotupdate_aot_benchmark": "11 HotUpdate AOT Bench",
        "hotupdate_jit_fact": "12 HotUpdate JIT Fact",
        "hotupdate_jit_benchmark": "13 HotUpdate JIT Bench",
        "dashboard": "14 Dashboard",
        "aggregate": "15 Aggregate",
    }
    return labels.get(stage, stage)


def generate_html(report: dict[str, Any]) -> str:
    """Generate a self-contained HTML dashboard page from a unified report dict."""
    family = report.get("family", "?")
    assembly = report.get("assembly", "?")
    overall = report.get("overall_status", "?")
    coverage = report.get("coverage", {})
    dashboard = report.get("dashboard", {})
    stages = report.get("stages", {})
    regression = report.get("regression", {})

    # ── Stage summary rows ────────────────────────────────────────────
    stage_rows = ""
    for stage_name in ["preflight", "codegen", "jit_codegen", "managed_fact",
                        "fact", "fact_jit", "audit", "asm_compare", "microbench",
                        "benchmark", "hotupdate", "hotupdate_aot_benchmark",
                        "hotupdate_jit_fact", "hotupdate_jit_benchmark",
                        "dashboard"]:
        sr = stages.get(stage_name, {})
        status = sr.get("status", "skipped")
        summary = sr.get("summary", "")
        stage_rows += (
            f"<tr>"
            f"  <td>{_stage_label(stage_name)}</td>"
            f"  <td>{_status_badge(status)}</td>"
            f"  <td style='font-size:0.8rem;color:#666;max-width:400px;overflow:hidden;text-overflow:ellipsis;white-space:nowrap'>{summary}</td>"
            f"</tr>\n"
        )

    # ── Managed fact section ──────────────────────────────────────────
    mf = dashboard.get("managedFact", {})
    mf_html = ""
    if mf:
        total = mf.get("totalMethods", 0)
        passed = mf.get("passedMethods", 0)
        failed = mf.get("failedMethods", 0)
        skipped = mf.get("skippedMethods", 0)
        tfm = mf.get("tfm", "net8.0")
        mf_html = f"""
        <div class="card">
          <h2>📋 Managed Fact ({tfm})</h2>
          <div class="metric-row">
            <div class="metric"><span class="metric-value">{total}</span><span class="metric-label">Total</span></div>
            <div class="metric passed"><span class="metric-value">{passed}</span><span class="metric-label">Passed</span></div>
            <div class="metric failed"><span class="metric-value">{failed}</span><span class="metric-label">Failed</span></div>
            <div class="metric skipped"><span class="metric-value">{skipped}</span><span class="metric-label">Skipped</span></div>
          </div>
        </div>
        """

    # ── IR Translation section ────────────────────────────────────────
    ir = dashboard.get("irTranslation", {})
    ir_html = ""
    if ir and ir.get("totalMethods", 0) > 0:
        ir_html = f"""
        <div class="card">
          <h2>🔧 IR Translation</h2>
          <table>
            <tr><th>Metric</th><th>Value</th></tr>
            <tr><td>Total Methods</td><td>{ir.get("totalMethods", 0)}</td></tr>
            <tr><td>IR Expansion Ratio</td><td>{ir.get("overallIrExpansionRatio", 0)}x</td></tr>
            <tr><td>JIT Instruction Avg</td><td>{ir.get("jitInstructionAvg", 0)}</td></tr>
            <tr><td>AOT Instruction Avg</td><td>{ir.get("aotInstructionAvg", 0)}</td></tr>
            <tr><td>External Runtime Calls</td><td>{ir.get("totalExternalRuntimeCalls", 0)}</td></tr>
            <tr><td>Virtual Dispatches</td><td>{ir.get("totalVirtualDispatches", 0)}</td></tr>
            <tr><td>Boxing Ops</td><td>{ir.get("totalBoxingOps", 0)}</td></tr>
          </table>
        </div>
        """

    # ── Performance / benchmark section ───────────────────────────────
    perf = dashboard.get("performance", {})
    perf_html = ""
    if perf and perf.get("nativeAotGeometricMeanNs"):
        chaos_class = perf.get("chaosClassification", {})
        class_rows = ""
        for cls, cnt in sorted(chaos_class.items()):
            class_rows += f"<tr><td>{cls}</td><td>{cnt}</td></tr>\n"
        perf_html = f"""
        <div class="card">
          <h2>⚡ Performance</h2>
          <table>
            <tr><th>Metric</th><th>Value</th></tr>
            <tr><td>AOT GeoMean</td><td>{perf.get("nativeAotGeometricMeanNs", 0):.1f} ns/op</td></tr>
            <tr><td>JIT GeoMean</td><td>{perf.get("nativeJitGeometricMeanNs", 0):.1f} ns/op</td></tr>
            <tr><td>AOT vs .NET8 JIT Speedup</td><td>{perf.get("chaosAotVsNet8Speedup", 0):.2f}x</td></tr>
            <tr><td>AOT Faster Count</td><td>{perf.get("chaosAotFasterCount", 0)}</td></tr>
            <tr><td>AOT Slower Count</td><td>{perf.get("chaosAotSlowerCount", 0)}</td></tr>
          </table>
          {f'<h3 style="margin-top:12px">Chaos AOT Classification</h3><table>{"".join(class_rows)}</table>' if class_rows else ''}
        </div>
        """

    # ── HotUpdate section ─────────────────────────────────────────────
    hu = dashboard.get("hotupdate", {})
    hu_html = ""
    if hu and hu.get("aotSlowdownFactor"):
        hu_html = f"""
        <div class="card">
          <h2>🔄 HotUpdate</h2>
          <table>
            <tr><th>Metric</th><th>Value</th></tr>
            <tr><td>AOT Slowdown Factor</td><td>{hu.get("aotSlowdownFactor", 0)}x</td></tr>
            <tr><td>AOT Avg Slowdown</td><td>{hu.get("aotAvgSlowdownPercent", 0)}%</td></tr>
            <tr><td>JIT Slowdown Factor</td><td>{hu.get("jitSlowdownFactor", 0)}x</td></tr>
            <tr><td>JIT Avg Slowdown</td><td>{hu.get("jitAvgSlowdownPercent", 0)}%</td></tr>
          </table>
        </div>
        """

    # ── Key Ratios section ────────────────────────────────────────────
    kr = dashboard.get("keyRatios", {})
    kr_html = ""
    if kr:
        kr_html = f"""
        <div class="card">
          <h2>📊 Key Ratios</h2>
          <div class="metric-row">
            <div class="metric"><span class="metric-value">{kr.get("irExpansionRatio", 0)}x</span><span class="metric-label">IR Expansion</span></div>
            <div class="metric"><span class="metric-value">{kr.get("asmPassRate", 0)}%</span><span class="metric-label">ASM Pass Rate</span></div>
            <div class="metric"><span class="metric-value">{kr.get("nativeFasterRatio", 0)}x</span><span class="metric-label">Native Faster</span></div>
            <div class="metric"><span class="metric-value">{kr.get("interpreterSlowdownFactor", 0)}x</span><span class="metric-label">Interp Slowdown</span></div>
          </div>
        </div>
        """

    # ── Coverage section ──────────────────────────────────────────────
    cov_html = ""
    if coverage:
        cov_html = """
        <div class="card">
          <h2>📈 Coverage</h2>
          <div class="metric-row">
        """
        for k, v in coverage.items():
            cov_html += f'<div class="metric"><span class="metric-value">{v:.1f}%</span><span class="metric-label">{k}</span></div>\n'
        cov_html += """
          </div>
        </div>
        """

    # ── Regression section ────────────────────────────────────────────
    reg_html = ""
    if regression:
        bm = regression.get("benchmark", {})
        if bm and bm.get("status") != "skipped":
            has_reg = regression.get("hasRegression", False)
            regs = regression.get("regressions", [])
            reg_html = f"""
            <div class="card">
              <h2>⚠️ Regression</h2>
              <p>Has regression: {'<strong style="color:#f44336">YES</strong>' if has_reg else '<span style="color:#4caf50">No</span>'}</p>
            """
            if regs:
                reg_html += '<table><tr><th>Runtime</th><th>Method</th><th>Delta</th></tr>'
                for r in regs:
                    reg_html += f"<tr><td>{r.get('runtime','')}</td><td>{r.get('subject_id','')}</td><td>{r.get('delta_pct',0):+.1f}%</td></tr>"
                reg_html += '</table>'
            reg_html += '</div>'

    # ── Full HTML ─────────────────────────────────────────────────────
    overall_color = {"passed": "#4caf50", "failed": "#f44336", "skipped": "#ff9800"}.get(overall, "#999")

    html = f"""<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>Pipeline Verification Dashboard — {family}</title>
<style>
  * {{ margin: 0; padding: 0; box-sizing: border-box; }}
  body {{ font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif; background: #f5f5f5; color: #333; padding: 24px; }}
  h1 {{ font-size: 1.5rem; }}
  .subtitle {{ color: #666; font-size: 0.85rem; margin: 4px 0 20px; }}
  .overall {{ display: inline-block; padding: 4px 16px; border-radius: 12px; font-weight: 700; font-size: 0.95rem; color: #fff; background: {overall_color}; }}
  .card {{ background: #fff; border-radius: 8px; box-shadow: 0 1px 3px rgba(0,0,0,0.12); padding: 20px; margin-bottom: 20px; }}
  .card h2 {{ font-size: 1.05rem; margin-bottom: 12px; color: #444; }}
  .card h3 {{ font-size: 0.95rem; margin-bottom: 8px; color: #555; }}
  table {{ width: 100%; border-collapse: collapse; font-size: 0.88rem; }}
  th, td {{ padding: 6px 10px; text-align: left; border-bottom: 1px solid #eee; }}
  th {{ font-weight: 600; color: #555; background: #fafafa; }}
  .metric-row {{ display: flex; gap: 16px; flex-wrap: wrap; }}
  .metric {{ background: #fafafa; border: 1px solid #e0e0e0; border-radius: 6px; padding: 12px 20px; text-align: center; min-width: 100px; }}
  .metric-value {{ display: block; font-size: 1.5rem; font-weight: 700; }}
  .metric-label {{ display: block; font-size: 0.75rem; color: #888; margin-top: 2px; }}
  .metric.passed .metric-value {{ color: #2e7d32; }}
  .metric.failed .metric-value {{ color: #c62828; }}
  .metric.skipped .metric-value {{ color: #e65100; }}
  @media (max-width: 700px) {{ .metric {{ min-width: 80px; }} }}
</style>
</head>
<body>

<h1>{assembly} / {family}</h1>
<p class="subtitle">
  Generated at {report.get("timestamp", "?")} &mdash;
  <span class="overall">{overall.upper()}</span>
  &mdash; Duration: {report.get("duration_ms", 0) / 1000:.1f}s
</p>

{cov_html}

<div class="card">
  <h2>📋 Stage Results</h2>
  <table>
    <thead><tr><th>Stage</th><th>Status</th><th>Summary</th></tr></thead>
    <tbody>{stage_rows}</tbody>
  </table>
</div>

{mf_html}

{kr_html}

{ir_html}

{perf_html}

{hu_html}

{reg_html}

</body>
</html>"""
    return html


def generate_html_report(report_path: Path, output_path: Path | None = None) -> Path:
    """Read unified report JSON and write HTML dashboard."""
    report = json.loads(report_path.read_text(encoding="utf-8"))
    if output_path is None:
        output_path = report_path.with_name("dashboard.html")
    html = generate_html(report)
    output_path.write_text(html, encoding="utf-8")
    print(f"  [dashboard-html] Written to {output_path}")
    return output_path
