"""Dashboard HTML renderer — all HTML generation functions for the deep dashboard.

This module contains every _build_* and generate_* function that produces
HTML/CSS/JS output. It imports Chinese dictionaries and data utilities from
the sibling `dashboard` module.
"""

from __future__ import annotations

import json
import re
import time
from collections import defaultdict
from typing import Any

from .dashboard import (
    BENCHMARK_COMPARISON_CN,
    METRIC_CN,
    STAGE_CN,
    STAGE_KEYS,
    STATUS_CN,
    _BENCHMARK_TECH_LABELS,
    _BENCHMARK_TECH_ORDER,
    _BENCHMARK_TECH_SHORT,
    _compute_benchmark_comparisons,
    _escape_html,
    _load_perf_jsonl,
    _short_method_label,
    parse_family,
    stage_cn,
)

# ──────────────────────────────────────────────────────────────────────
# Module C: HTML generation helpers
# ──────────────────────────────────────────────────────────────────────


def _status_icon(status: str) -> str:
    icons = {"passed": "&#10003;", "failed": "&#10007;", "skipped": "&mdash;", "error": "!"}
    return icons.get(status, "?")


def _status_label(status: str) -> str:
    return STATUS_CN.get(status, status)


def _fmt_duration(ms: int | float) -> str:
    """Format milliseconds to human-readable Chinese string."""
    if not ms:
        return ""
    seconds = ms / 1000
    if seconds >= 60:
        m = int(seconds // 60)
        s = int(seconds % 60)
        return f"{m}分{s}秒"
    if seconds >= 1:
        return f"{seconds:.1f}秒"
    return f"{ms:.0f}毫秒"


def _fmt_seconds(s: float) -> str:
    if not s:
        return ""
    if s >= 60:
        m = int(s // 60)
        sec = int(s % 60)
        return f"{m}分{sec}秒"
    return f"{s:.1f}秒"


def _safe_json(obj: Any) -> str:
    """JSON serialize with Chinese-safe encoding."""
    return json.dumps(obj, ensure_ascii=False)


def _fmt_metric_value(key: str, value: float) -> str:
    """Format a metric value with appropriate precision."""
    unit = METRIC_CN.get(key, {}).get("unit", "")
    if key in ("irExpansionRatio",):
        return f"{value:.2f}{unit}"
    if key in ("stagePassRate", "asmPassRate", "nativeFasterRatio", "managedFasterRatio", "averageSpeedupPercent"):
        return f"{value:.1f}{unit}"
    if isinstance(value, float):
        if value >= 1e6:
            return f"{value / 1e6:.2f}M{unit}"
        if value >= 1e3:
            return f"{value / 1e3:.1f}K{unit}"
        return f"{value:.1f}{unit}"
    return str(value)


# ──────────────────────────────────────────────────────────────────────
# Benchmark table (per-method ns/op across all runtimes)
# ──────────────────────────────────────────────────────────────────────


def _build_benchmark_table(slug: str) -> str:
    """Build per-method benchmark comparison table (ns/op across all runtimes)."""
    records = _load_perf_jsonl(slug)
    if not records:
        return ""

    by_method: dict[int, dict[str, float]] = defaultdict(dict)
    method_labels: dict[int, str] = {}

    for r in records:
        tech = r.get("technology", "")
        idx = r.get("methodIndex", -1)
        if idx < 0 or tech not in _BENCHMARK_TECH_ORDER:
            continue

        mid = r.get("methodSubjectId", "")
        if idx not in method_labels:
            method_labels[idx] = _short_method_label(mid)

        metrics = r.get("metrics", {})
        ms = metrics.get("elapsedMilliseconds", 0)
        iters = r.get("iterations", 1)

        # HotUpdate records use postPatchNsPerOp
        if "postPatchNsPerOp" in metrics:
            ns_op = metrics["postPatchNsPerOp"]
        elif ms > 0 and iters > 0:
            ns_op = ms * 1e6 / iters
        else:
            continue

        # Only update if this is a later timestamp (keep latest data)
        by_method[idx][tech] = ns_op

    if not by_method:
        return ""

    # Build header
    header_cells = '<th style="text-align:left;">方法</th>'
    for tech in _BENCHMARK_TECH_ORDER:
        label = _BENCHMARK_TECH_LABELS.get(tech, tech)
        header_cells += f'<th style="text-align:right;"><div>{label}</div><div class="runtime-sub">ns/op</div></th>'

    # Build rows
    body_rows = ""
    for idx in sorted(by_method.keys()):
        methods = by_method[idx]
        label = method_labels.get(idx, f"#{idx}")

        # Find fastest and slowest (only among present values)
        valid = [(t, v) for t, v in methods.items() if v > 0]
        fastest_tech = min(valid, key=lambda x: x[1])[0] if len(valid) >= 2 else None
        slowest_tech = max(valid, key=lambda x: x[1])[0] if len(valid) >= 2 else None

        cells = f'<td>{_escape_html(label)}</td>'
        for tech in _BENCHMARK_TECH_ORDER:
            v = methods.get(tech)
            if v is not None and v >= 0:
                cls = ""
                if fastest_tech and tech == fastest_tech:
                    cls = ' cell-fastest'
                elif slowest_tech and tech == slowest_tech:
                    cls = ' cell-slowest'
                # Format: if >=1000ns show "1,234.5", if >=100ns show "123.4", else show "12.34" or "1.23"
                if v >= 10000:
                    val_str = f"{v:,.1f}"
                elif v >= 1000:
                    val_str = f"{v:,.1f}"
                elif v >= 10:
                    val_str = f"{v:.2f}"
                else:
                    val_str = f"{v:.2f}"
                cells += f'<td class="{cls}">{val_str}</td>'
            else:
                cells += '<td class="cell-na">-</td>'
        body_rows += f"<tr>{cells}</tr>"

    return f"""\
  <div class="benchmark-table-wrap">
    <table class="benchmark-table">
      <thead><tr>{header_cells}</tr></thead>
      <tbody>{body_rows}</tbody>
    </table>
  </div>"""


def _build_benchmark_section(comparisons: dict[str, Any]) -> str:
    """Build HTML for the benchmark performance comparison section."""
    if not comparisons.get("has_any_data"):
        return ""

    comps = comparisons.get("comparisons", {})
    techs = comparisons.get("technologies", {})

    rows = ""
    for key, order in [
        ("aot_vs_net8", 0),
        ("jit_vs_net8", 1),
        ("hu_aot_vs_aot", 2),
        ("hu_jit_vs_jit", 3),
    ]:
        c = comps.get(key, {})
        cn = BENCHMARK_COMPARISON_CN.get(key, {})
        status = c.get("status", "无数据")
        pct = c.get("pct")

        if pct is not None:
            arrow = "&#9650;" if pct >= 0 else "&#9660;"
            color = "#2e7d32" if pct >= 0 else "#c62828"
            prefix = "+" if pct >= 0 else ""
            value_html = f'<div class="comp-value" style="color:{color};">{arrow} {prefix}{pct:.1f}%</div>'
        else:
            value_html = f'<div class="comp-value na">{status}</div>'

        rows += f"""\
    <div class="comparison-card">
      <div class="comp-label">{cn.get("name", key)}</div>
      {value_html}
      <div class="comp-detail">{cn.get("description", "")}</div>
    </div>"""

    # Build technology summary line
    tech_summary = ""
    if techs:
        tech_parts = []
        for tech in ["chaos-aot", "chaos-jit", "chaos-hu-aot", "chaos-hu-jit", "net8-jit", "net10-jit", "mono"]:
            t = techs.get(tech)
            if t:
                ops_str = _fmt_metric_value("averageOpsPerSecond", t["gm_ops"])
                tech_parts.append(f'<span class="tech-tag">{tech}: {ops_str}</span>')
        if tech_parts:
            tech_summary = f'<div class="tech-summary">{" | ".join(tech_parts)}</div>'

    return f"""\
  <div class="benchmark-section">
    <h4 class="section-title">性能对比 &#9881;</h4>
    {tech_summary}
    <div class="comparison-grid">
{rows}
    </div>
  </div>"""


# ──────────────────────────────────────────────────────────────────────
# Stage rows
# ──────────────────────────────────────────────────────────────────────


def _build_stage_rows_from_coverage(slug: str, coverage: dict[str, Any] | None) -> str:
    """Build a summary row from coverage data when individual stage data is unavailable."""
    if not coverage:
        return ""
    passed = coverage.get("stagesPassed", 0)
    failed = coverage.get("stagesFailed", 0)
    skipped = coverage.get("stagesSkipped", 0)
    total = coverage.get("stagesTotal", 15)

    return f"""\
<tr class="stage-row status-{'passed' if failed == 0 else 'failed'}">
  <td style="width:22px;"><span class="stage-icon {'passed' if failed == 0 else 'failed'}">{'&#10003;' if failed == 0 else '&#10007;'}</span></td>
  <td colspan="2">
    <div class="stage-name-cn">汇总</div>
    <div class="stage-name-en">coverage summary</div>
  </td>
  <td colspan="2">
    <div style="font-size:0.85rem;">
      <span style="color:#2e7d32;">&#10003; 通过 {passed}</span>
      &nbsp;
      <span style="color:#c62828;">&#10007; 失败 {failed}</span>
      &nbsp;
      <span style="color:#ff9800;">&mdash; 跳过 {skipped}</span>
      &nbsp;|&nbsp; 共计 {total} 阶段
    </div>
    <div class="stage-desc" style="margin-top:4px;">
      &#9432; 阶段级详情数据在 batch 输出的 JSON 尾部被截断，无法完整恢复。
      可通过 <code>python -m testing.foundation_dll._core.python.cli {slug}</code> 单独运行以查看各阶段详情。
    </div>
  </td>
</tr>"""


def _build_stage_rows(slug: str, stages: dict[str, Any], has_data: bool, coverage: dict[str, Any] | None) -> str:
    """Build HTML for the stage detail table."""
    if not has_data:
        return f'<tr><td colspan="5" class="na-notice">暂无阶段数据（该 family 在第一轮 batch 中运行，未捕获详细输出）。<br>可通过 <code>python -m testing.foundation_dll._core.python.cli {slug}</code> 单独运行以获取详细数据。</td></tr>'

    # Check if we have actual stage data (from stdout_tail extraction)
    # Only show individual stage rows if at least 5 stages have data
    has_individual_stages = sum(1 for k in STAGE_KEYS if stages.get(k)) >= 5

    if not has_individual_stages:
        # We have coverage summary but no individual stage data (truncated tail)
        # Show the coverage summary row with explanation
        return _build_stage_rows_from_coverage(slug, coverage)

    rows = ""
    for key in STAGE_KEYS:
        cn = stage_cn(key)
        s = stages.get(key, {})
        stage_status = s.get("status", "unknown") if s else "na"
        status_cn = _status_label(stage_status)
        icon = _status_icon(stage_status)
        icon_cls = stage_status if stage_status in ("passed", "failed", "skipped", "error") else "na"
        duration = s.get("duration_ms", 0) if s else 0
        dur_str = _fmt_duration(duration) if duration else ""
        summary = _escape_html(s.get("summary", "")) if s else ""
        errors = s.get("errors", []) if s else []

        # Build error detail HTML
        error_html = ""
        for err in errors:
            err_text = _escape_html(err)
            # Try to match against common errors
            cn_errors = cn.get("common_errors", {})
            cn_hint = ""
            for pattern, hint in cn_errors.items():  # type: ignore[union-attr]
                if pattern.lower() in err.lower():
                    cn_hint = hint
                    break
            hint_html = f'<div class="err-cn">&#9656; {_escape_html(cn_hint)}</div>' if cn_hint else ""
            error_html += f'<div class="stage-error"><div class="err-title">&#9888; 错误</div><div class="err-msg">{err_text}</div>{hint_html}</div>'

        # Build failure_meaning if stage failed
        fail_hint = ""
        if stage_status == "failed":
            fail_hint = f'<div class="err-cn">{_escape_html(str(cn.get("failure_meaning", "")))}</div>'

        status_badge = f'<span class="badge badge-{stage_status if stage_status in ("passed","failed","skipped","error") else "na"}">{icon} {status_cn}</span>'

        rows += f"""\
<tr class="stage-row status-{stage_status if stage_status in ("passed","failed","skipped","error","na") else "na"}">
  <td style="width:22px;"><span class="stage-icon {icon_cls}">{icon}</span></td>
  <td style="width:180px;">
    <div class="stage-name-cn">{cn["name"]}</div>
    <div class="stage-name-en">{key}</div>
  </td>
  <td style="width:80px;">{status_badge}</td>
  <td style="width:80px;">{dur_str}</td>
  <td>
    <div class="stage-desc">{cn["description"]}</div>
    {fail_hint}
    {error_html}
    {("<div class=\"stage-desc\">" + summary[:200] + "</div>") if summary and not error_html else ""}
  </td>
</tr>"""

    return rows


# ──────────────────────────────────────────────────────────────────────
# Metrics / coverage
# ──────────────────────────────────────────────────────────────────────


def _build_metrics_grid(coverage: dict[str, Any] | None, dashboard: dict[str, Any] | None) -> str:
    """Build metrics grid from coverage and dashboard data."""
    items = ""

    if coverage:
        for key in ("stagesPassed", "stagesFailed", "stagesSkipped", "stagesTotal", "stagePassRate"):
            val = coverage.get(key)
            if val is not None:
                mc = METRIC_CN.get(key, {})
                items += f'<div class="metric-item"><div class="m-name">{mc.get("name", key)}</div><div class="m-value">{_fmt_metric_value(key, val)}</div></div>'

    if dashboard:
        key_ratios = dashboard.get("keyRatios", {}) if isinstance(dashboard, dict) else {}
        if isinstance(key_ratios, dict):
            for key in ("nativeFasterRatio", "managedFasterRatio", "irExpansionRatio", "asmPassRate"):
                val = key_ratios.get(key)
                if val is not None:
                    mc = METRIC_CN.get(key, {})
                    items += f'<div class="metric-item"><div class="m-name">{mc.get("name", key)}</div><div class="m-value">{_fmt_metric_value(key, val)}</div><div class="m-desc">{mc.get("description", "")}</div></div>'

        avg_speedup = dashboard.get("averageSpeedupPercent") if isinstance(dashboard, dict) else None
        if avg_speedup is not None:
            mc = METRIC_CN.get("averageSpeedupPercent", {})
            items += f'<div class="metric-item"><div class="m-name">{mc.get("name", "averageSpeedupPercent")}</div><div class="m-value">{_fmt_metric_value("averageSpeedupPercent", avg_speedup)}</div><div class="m-desc">{mc.get("description", "")}</div></div>'

    return items if items else '<div class="na-notice">暂无指标数据</div>'


def _build_coverage_bar(coverage: dict[str, Any] | None) -> str:
    """Build a visual coverage bar."""
    if not coverage:
        return ""
    passed = coverage.get("stagesPassed", 0)
    failed = coverage.get("stagesFailed", 0)
    skipped = coverage.get("stagesSkipped", 0)
    total = coverage.get("stagesTotal", 15)
    non_skipped = total - skipped
    rate = coverage.get("stagePassRate", 0)
    if total == 0:
        return ""

    # Bar segments: passed/failed proportion of TOTAL (including skipped as neutral)
    p_pct = passed / total * 100 if total else 0
    f_pct = failed / total * 100 if total else 0

    detail_parts = [f"通过 {passed}"]
    if failed:
        detail_parts.append(f"失败 {failed}")
    if skipped:
        detail_parts.append(f"跳过 {skipped}")

    color = "#4caf50" if rate >= 80 else ("#ff9800" if rate >= 50 else "#f44336")
    skipped_color = "#e0e0e0"

    bar_html = f"""\
    <div class="coverage-bar" style="display:flex;height:8px;border-radius:4px;overflow:hidden;background:{skipped_color};">"""
    if p_pct > 0:
        bar_html += f'<div style="width:{p_pct:.0f}%;background:{color};"></div>'
    if f_pct > 0:
        bar_html += f'<div style="width:{f_pct:.0f}%;background:#f44336;"></div>'
    if skipped > 0:
        s_pct = skipped / total * 100
        bar_html += f'<div style="width:{s_pct:.0f}%;background:#e0e0e0;"></div>'
    bar_html += "</div>"

    return f"""\
<div style="display:flex;justify-content:space-between;font-size:0.8rem;color:#888;margin-top:6px;">
  <span>阶段覆盖率 ({', '.join(detail_parts)})</span>
  <span>{passed}/{non_skipped} 非跳过 ({rate:.0f}%)</span>
</div>
{bar_html}"""


# ──────────────────────────────────────────────────────────────────────
# Family card
# ──────────────────────────────────────────────────────────────────────


def _build_family_card(f: dict[str, Any], index: int) -> str:
    """Build a single expandable family card."""
    slug_raw = f["slug"]  # unescaped for file lookup
    slug = _escape_html(slug_raw)
    status = f.get("status", "unknown")
    status_cn = _status_label(status)
    status_cls = status if status in ("passed", "failed", "skipped", "error", "crashed") else "na"
    duration = _fmt_seconds(f.get("duration_seconds", 0))
    coverage = f.get("coverage")
    dashboard_data = f.get("dashboard")
    stages = f.get("stages", {})
    has_data = f.get("has_stage_data", False)

    # Coverage summary for header
    cov_summary = ""
    if coverage:
        p = coverage.get("stagesPassed", 0)
        t = coverage.get("stagesTotal", 0)
        cov_summary = f'{p}/{t} 阶段通过' if t else ''

    stage_rows = _build_stage_rows(slug, stages, has_data, coverage)
    metrics_grid = _build_metrics_grid(coverage, dashboard_data)
    cov_bar = _build_coverage_bar(coverage)
    benchmark_section = _build_benchmark_section(_compute_benchmark_comparisons(slug_raw))
    benchmark_table = _build_benchmark_table(slug_raw)

    return f"""\
<div class="family-card" id="card-{index}" data-status="{status}">
  <div class="card-header" onclick="toggleCard({index})">
    <span class="expand-icon" id="icon-{index}">&#9654;</span>
    <span class="slug">{slug}</span>
    <span class="coverage-summary">{cov_summary}</span>
    <span class="badge badge-{status_cls}">{status_cn}</span>
    {('<span style="font-size:0.75rem;color:#999">' + duration + '</span>') if duration else ''}
  </div>
  <div class="card-body" id="body-{index}">
    <table class="stage-table">
      <thead>
        <tr><th></th><th>阶段</th><th>状态</th><th>耗时</th><th>详情</th></tr>
      </thead>
      <tbody>
        {stage_rows}
      </tbody>
    </table>
    {cov_bar}
    {('<h4 style="font-size:0.85rem;color:#555;margin:12px 0 4px;">关键指标</h4><div class="metrics-grid">' + metrics_grid + '</div>') if metrics_grid and metrics_grid != '<div class="na-notice">暂无指标数据</div>' else ''}
    {benchmark_section}
    {benchmark_table}
  </div>
</div>"""


# ──────────────────────────────────────────────────────────────────────
# CSS / JS generators
# ──────────────────────────────────────────────────────────────────────


def generate_css() -> str:
    return """\
* { margin: 0; padding: 0; box-sizing: border-box; }
body { font-family: -apple-system, BlinkMacSystemFont, "Microsoft YaHei", "PingFang SC", "Noto Sans SC", "Segoe UI", Roboto, sans-serif; background: #f0f2f5; color: #333; padding: 0; }
.header { background: linear-gradient(135deg, #1a237e, #283593); color: #fff; padding: 24px 32px; }
.header h1 { font-size: 1.4rem; font-weight: 600; }
.header .subtitle { font-size: 0.85rem; opacity: 0.8; margin-top: 4px; }
.stats { display: flex; gap: 12px; padding: 16px 32px; background: #fff; border-bottom: 1px solid #e0e0e0; flex-wrap: wrap; }
.stat-card { padding: 8px 20px; text-align: center; border-radius: 6px; min-width: 90px; }
.stat-card .num { font-size: 1.6rem; font-weight: 700; }
.stat-card .label { font-size: 0.75rem; color: #888; margin-top: 2px; }
.stat-card.total .num { color: #1565c0; }
.stat-card.pass .num { color: #2e7d32; }
.stat-card.fail .num { color: #c62828; }
.stat-card.rate .num { color: #f57f17; }
.filter-bar { display: flex; gap: 10px; padding: 12px 32px; background: #fff; border-bottom: 1px solid #e0e0e0; align-items: center; flex-wrap: wrap; }
.filter-bar input { padding: 6px 12px; border: 1px solid #ccc; border-radius: 4px; font-size: 0.85rem; width: 200px; }
.filter-bar input:focus { outline: none; border-color: #1565c0; }
.filter-btn { padding: 6px 14px; border: 1px solid #ccc; border-radius: 14px; background: #fff; cursor: pointer; font-size: 0.8rem; transition: all 0.15s; }
.filter-btn:hover { background: #f5f5f5; }
.filter-btn.active { background: #1565c0; color: #fff; border-color: #1565c0; }
.filter-btn.active-all { background: #1565c0; color: #fff; border-color: #1565c0; }
.filter-btn.active-pass { background: #2e7d32; color: #fff; border-color: #2e7d32; }
.filter-btn.active-fail { background: #c62828; color: #fff; border-color: #c62828; }
.filter-btn.toggle-all { margin-left: auto; background: #f5f5f5; }
.container { padding: 16px 32px; max-width: 1200px; margin: 0 auto; }
.family-count { font-size: 0.85rem; color: #888; padding: 8px 32px 0; }
.family-card { background: #fff; border-radius: 8px; box-shadow: 0 1px 3px rgba(0,0,0,0.10); margin-bottom: 8px; overflow: hidden; transition: box-shadow 0.2s; }
.family-card:hover { box-shadow: 0 2px 8px rgba(0,0,0,0.15); }
.card-header { display: flex; align-items: center; padding: 14px 20px; cursor: pointer; gap: 12px; user-select: none; }
.card-header .expand-icon { width: 20px; text-align: center; color: #999; font-size: 0.8rem; flex-shrink: 0; }
.card-header .slug { font-weight: 600; font-size: 0.95rem; flex: 1; font-family: 'Consolas', 'Courier New', monospace; }
.card-header .coverage-summary { font-size: 0.8rem; color: #888; margin-right: 8px; }
.badge { display: inline-block; padding: 2px 10px; border-radius: 10px; font-size: 0.75rem; font-weight: 600; }
.badge-passed { background: #e8f5e9; color: #2e7d32; }
.badge-failed { background: #fbe9e7; color: #c62828; }
.badge-skipped { background: #fff8e1; color: #f57f17; }
.badge-error { background: #f3e5f5; color: #6a1b9a; }
.badge-na { background: #f5f5f5; color: #9e9e9e; }
.card-body { display: none; padding: 0 20px 16px; }
.card-body.open { display: block; }
.stage-table { width: 100%; border-collapse: collapse; margin-top: 8px; font-size: 0.85rem; }
.stage-table th { text-align: left; padding: 8px 10px; border-bottom: 2px solid #eee; font-weight: 600; color: #555; font-size: 0.8rem; }
.stage-table td { padding: 7px 10px; border-bottom: 1px solid #f5f5f5; vertical-align: top; }
.stage-row { transition: background 0.1s; }
.stage-row:hover { background: #fafafa; }
.stage-row.status-passed { border-left: 3px solid #4caf50; }
.stage-row.status-failed { border-left: 3px solid #f44336; }
.stage-row.status-skipped { border-left: 3px solid #ff9800; }
.stage-row.status-error { border-left: 3px solid #9c27b0; }
.stage-row.status-na { border-left: 3px solid #bdbdbd; }
.stage-icon { display: inline-block; width: 20px; text-align: center; font-weight: 700; }
.stage-icon.passed { color: #4caf50; }
.stage-icon.failed { color: #f44336; }
.stage-icon.skipped { color: #ff9800; }
.stage-icon.error { color: #9c27b0; }
.stage-icon.na { color: #bdbdbd; }
.stage-name-cn { font-weight: 600; }
.stage-name-en { font-size: 0.75rem; color: #999; }
.stage-desc { font-size: 0.8rem; color: #666; margin-top: 2px; }
.stage-error { background: #fff5f5; border-radius: 4px; padding: 6px 10px; margin-top: 4px; font-size: 0.8rem; }
.stage-error .err-title { font-weight: 600; color: #c62828; }
.stage-error .err-msg { color: #555; margin-top: 2px; }
.stage-error .err-cn { color: #888; margin-top: 2px; font-style: italic; }
.coverage-bar { height: 6px; border-radius: 3px; background: #e0e0e0; margin: 10px 0; overflow: hidden; }
.coverage-fill { height: 100%; border-radius: 3px; transition: width 0.5s; }
.metrics-grid { display: grid; grid-template-columns: repeat(auto-fill, minmax(180px, 1fr)); gap: 8px; margin-top: 8px; }
.metric-item { background: #fafafa; border-radius: 4px; padding: 8px 12px; }
.metric-item .m-name { font-size: 0.75rem; color: #888; }
.metric-item .m-value { font-size: 1rem; font-weight: 600; color: #333; }
.metric-item .m-desc { font-size: 0.7rem; color: #aaa; margin-top: 2px; }
.na-notice { text-align: center; padding: 24px; color: #999; font-size: 0.9rem; }
.footer { text-align: center; padding: 20px; color: #999; font-size: 0.8rem; }
.hidden { display: none !important; }

/* Benchmark performance comparison section */
.benchmark-section { margin-top: 16px; padding-top: 12px; border-top: 1px solid #eee; }
.benchmark-section .section-title { font-size: 0.85rem; color: #555; margin-bottom: 8px; }
.tech-summary { font-size: 0.75rem; color: #888; margin-bottom: 8px; line-height: 1.6; }
.tech-tag { background: #f5f5f5; padding: 1px 6px; border-radius: 3px; white-space: nowrap; }
.comparison-grid { display: grid; grid-template-columns: 1fr 1fr; gap: 8px; }
.comparison-card { background: #fafafa; border: 1px solid #e8e8e8; border-radius: 6px; padding: 10px 14px; }
.comparison-card .comp-label { font-size: 0.8rem; color: #666; font-weight: 600; }
.comparison-card .comp-value { font-size: 1.2rem; font-weight: 700; margin: 4px 0; }
.comparison-card .comp-value.na { color: #bdbdbd; font-size: 0.8rem; font-weight: 400; }
.comparison-card .comp-detail { font-size: 0.72rem; color: #999; }

/* Benchmark detail table */
.benchmark-table-wrap { overflow-x: auto; margin-top: 8px; }
.benchmark-table { width: 100%; border-collapse: collapse; font-size: 0.78rem; }
.benchmark-table th { text-align: right; padding: 4px 8px; border-bottom: 1px solid #ddd; font-weight: 600; color: #555; white-space: nowrap; }
.benchmark-table th:first-child { text-align: left; }
.benchmark-table td { text-align: right; padding: 3px 8px; border-bottom: 1px solid #f0f0f0; white-space: nowrap; font-variant-numeric: tabular-nums; }
.benchmark-table td:first-child { text-align: left; font-weight: 500; color: #444; }
.benchmark-table tr:hover td { background: #fafafa; }
.benchmark-table .cell-fastest { color: #2e7d32; font-weight: 600; }
.benchmark-table .cell-slowest { color: #c62828; }
.benchmark-table .cell-na { color: #ccc; }
.benchmark-table .runtime-header { font-size: 0.7rem; color: #666; border-bottom: 2px solid #e0e0e0; }
.benchmark-table .runtime-sub { font-size: 0.62rem; color: #aaa; font-weight: 400; }
@media (max-width: 700px) {
  .stats { padding: 12px 16px; gap: 8px; }
  .filter-bar { padding: 10px 16px; }
  .filter-bar input { width: 140px; }
  .container { padding: 12px 16px; }
  .card-header { padding: 12px 14px; }
  .card-body { padding: 0 14px 12px; }
  .metrics-grid { grid-template-columns: repeat(2, 1fr); }
}
"""


def generate_js(report_data_json: str) -> str:
    return f"""\
const DATA = {report_data_json};
const parsed = DATA.parsed || [];

function toggleCard(id) {{
  const body = document.getElementById('body-' + id);
  const icon = document.getElementById('icon-' + id);
  if (!body) return;
  const isOpen = body.classList.toggle('open');
  icon.textContent = isOpen ? '▼' : '▶';
}}

function expandAll() {{
  parsed.forEach((_, i) => {{
    const body = document.getElementById('body-' + i);
    const icon = document.getElementById('icon-' + i);
    if (body) {{ body.classList.add('open'); icon.textContent = '▼'; }}
  }});
}}

function collapseAll() {{
  parsed.forEach((_, i) => {{
    const body = document.getElementById('body-' + i);
    const icon = document.getElementById('icon-' + i);
    if (body) {{ body.classList.remove('open'); icon.textContent = '▶'; }}
  }});
}}

function filterFamilies() {{
  const search = (document.getElementById('searchInput').value || '').toLowerCase();
  const activeStatus = document.querySelector('.filter-btn.active');
  const statusFilter = activeStatus ? activeStatus.dataset.status : 'all';

  let visible = 0;
  parsed.forEach((f, i) => {{
    const card = document.getElementById('card-' + i);
    if (!card) return;

    const matchSearch = !search || f.slug.toLowerCase().includes(search);
    const matchStatus = statusFilter === 'all' || f.status === statusFilter;

    if (matchSearch && matchStatus) {{
      card.classList.remove('hidden');
      visible++;
    }} else {{
      card.classList.add('hidden');
    }}
  }});

  document.getElementById('visibleCount').textContent = visible;
}}

function setFilter(btn, status) {{
  document.querySelectorAll('.filter-btn[data-status]').forEach(b => {{
    b.classList.remove('active', 'active-all', 'active-pass', 'active-fail');
  }});
  btn.classList.add('active');
  if (status === 'all') btn.classList.add('active-all');
  else if (status === 'passed') btn.classList.add('active-pass');
  else if (status === 'failed') btn.classList.add('active-fail');
  filterFamilies();
}}
"""


# ──────────────────────────────────────────────────────────────────────
# Main HTML generation
# ──────────────────────────────────────────────────────────────────────


def generate_html(report: dict[str, Any]) -> str:
    """Generate the complete self-contained HTML dashboard."""
    parsed = report.get("parsed") or [parse_family(r) for r in report.get("results", [])]
    total = len(parsed)
    n_passed = sum(1 for f in parsed if f.get("status") == "passed")
    n_failed = sum(1 for f in parsed if f.get("status") == "failed")
    n_skipped = sum(1 for f in parsed if f.get("status") == "skipped")
    n_crashed = sum(1 for f in parsed if f.get("status") == "crashed")
    n_other = total - n_passed - n_failed - n_skipped - n_crashed
    pass_rate = round(n_passed / total * 100) if total > 0 else 0
    elapsed = report.get("elapsed_seconds", 0)
    elapsed_str = f"{int(elapsed // 60)}分{int(elapsed % 60)}秒" if elapsed else ""

    # Build family cards
    cards_html = ""
    for i, f in enumerate(parsed):
        cards_html += _build_family_card(f, i)

    # Build JSON data for JS
    js_data = _safe_json({"parsed": [{"slug": f["slug"], "status": f["status"]} for f in parsed]})

    now_str = time.strftime("%Y-%m-%d %H:%M:%S")

    html = f"""<!DOCTYPE html>
<html lang="zh-CN">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>Foundation-DLL 深度验证仪表盘</title>
<style>
{generate_css()}
</style>
</head>
<body>

<div class="header">
  <h1>Foundation-DLL 深度验证仪表盘</h1>
  <div class="subtitle">
    System.Private.CoreLib &mdash; {total} 个 Family &mdash;
    生成时间: {now_str} &mdash;
    总耗时: {elapsed_str}
  </div>
</div>

<div class="stats">
  <div class="stat-card total">
    <div class="num">{total}</div>
    <div class="label">总 Families</div>
  </div>
  <div class="stat-card pass">
    <div class="num">{n_passed}</div>
    <div class="label">通过 &#10003;</div>
  </div>
  <div class="stat-card fail">
    <div class="num">{n_failed}</div>
    <div class="label">失败 &#10007;</div>
  </div>
  {f'<div class="stat-card" style="background:#fff8e1;"><div class="num" style="color:#f57f17;">{n_skipped}</div><div class="label">跳过</div></div>' if n_skipped > 0 else ''}
  {f'<div class="stat-card" style="background:#fbe9e7;"><div class="num" style="color:#c62828;">{n_crashed}</div><div class="label">崩溃</div></div>' if n_crashed > 0 else ''}
  <div class="stat-card rate">
    <div class="num">{pass_rate}%</div>
    <div class="label">通过率</div>
  </div>
  {f'<div class="stat-card"><div class="num">{n_other}</div><div class="label">其他</div></div>' if n_other > 0 else ''}
</div>

<div class="filter-bar">
  <input type="text" id="searchInput" placeholder="搜索 family 名称..." oninput="filterFamilies()">
  <button class="filter-btn active active-all" data-status="all" onclick="setFilter(this, 'all')">全部</button>
  <button class="filter-btn" data-status="passed" onclick="setFilter(this, 'passed')">通过</button>
  <button class="filter-btn" data-status="failed" onclick="setFilter(this, 'failed')">失败</button>
  <button class="filter-btn toggle-all" onclick="expandAll()">展开全部 &#9660;</button>
  <button class="filter-btn toggle-all" onclick="collapseAll()">折叠全部 &#9654;</button>
</div>

<div class="family-count">
  显示 <span id="visibleCount">{total}</span> / {total} 个 family
</div>

<div class="container">
  {cards_html}
</div>

<div class="footer">
  Foundation-DLL 深度验证仪表盘 &mdash; 自动生成
</div>

<script>
{generate_js(js_data)}
</script>
</body>
</html>"""
    return html
