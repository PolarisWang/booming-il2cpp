"""Nightly summary — generate Markdown report from nightly-delta.json.

Reads nightly-delta.json and produces a human-readable Markdown summary
suitable for CI output, PR comments, or email notifications.

Usage:
    python -m verification.nightly_summary --delta nightly-delta.json \\
        --output nightly-summary.md
"""

from __future__ import annotations

import json
import sys
from datetime import datetime, timezone
from pathlib import Path
from typing import Any


def _fmt_pct(value: float | None, always_sign: bool = False) -> str:
    if value is None:
        return "—"
    sign = "+" if always_sign and value >= 0 else ""
    return f"{sign}{value:.1f}%"


def _fmt_delta(value: float | None, unit: str = "") -> str:
    if value is None:
        return "—"
    if value == 0:
        return "="
    sign = "+" if value > 0 else ""
    return f"{sign}{value:.1f}{unit}"


def _delta_emoji(delta: float | None, good_is_positive: bool = False) -> str:
    """Choose emoji based on delta direction."""
    if delta is None:
        return ""
    if good_is_positive:
        return "✅" if delta >= 0 else "🔻"
    return "🔻" if delta >= 0 else "✅"


def generate_summary(delta_data: dict[str, Any]) -> str:
    """Generate a Markdown night report from delta data."""
    overall = delta_data.get("overall", {})
    regressions = delta_data.get("regressions", [])
    improvements = delta_data.get("improvements", [])
    assemblies = delta_data.get("assemblies", [])
    chunks = delta_data.get("chunks", {})
    report_date = delta_data.get("date", "?")

    lines: list[str] = []

    # ── Header ──
    lines.append(f"## Nightly Build Report — {report_date}")
    lines.append("")

    # ── Overall Summary ──
    lines.append("### Overall")
    lines.append("")
    lines.append(f"| Metric | Value |")
    lines.append(f"|--------|-------|")
    lines.append(f"| Assemblies | {overall.get('totalAssemblies', 0)} |")
    lines.append(f"| Chunks | {overall.get('chunksVerified', 0)} / {overall.get('totalChunks', 0)} verified |")
    lines.append(f"| Fact pass rate | {_fmt_pct(overall.get('factPassRate'))} "
                 f"{_delta_emoji(overall.get('factPassRateDelta'))} |")
    lines.append(f"| Benchmark methods | {overall.get('totalBenchmarked', 0)} |")
    lines.append(f"| Coverage gap | {overall.get('totalCoverageGap', 0)} methods unverified |")
    lines.append(f"| Assemblies with history | {overall.get('assembliesWithHistory', 0)} |")
    lines.append("")

    # ── Regressions ──
    lines.append(f"### Regressions ⚠️ ({len(regressions)})")
    lines.append("")
    if regressions:
        lines.append("| Assembly | Chunk | Metric | Before | After | Δ |")
        lines.append("|----------|-------|--------|--------|-------|---|")
        for r in regressions:
            before = r.get("before")
            after = r.get("after")
            metric = r.get("metric", "?")
            fmt_before = f"{before:.1f}" if before is not None else "—"
            fmt_after = f"{after:.1f}" if after is not None else "—"
            if metric == "coverage_gap":
                fmt_before = str(int(before)) if before is not None else "—"
                fmt_after = str(int(after)) if after is not None else "—"
            elif metric == "bench_duration":
                fmt_before = f"{before:.4f}ms" if before is not None else "—"
                fmt_after = f"{after:.4f}ms" if after is not None else "—"
            lines.append(f"| {r.get('assembly', '?')} | {r.get('slug', '?')} | "
                         f"{metric} | {fmt_before} | {fmt_after} | **{r.get('delta', '?'):+.1f}** |")
    else:
        lines.append("_No regressions detected._")
    lines.append("")

    # ── Improvements ──
    lines.append(f"### Improvements ✅ ({len(improvements)})")
    lines.append("")
    if improvements:
        lines.append("| Assembly | Chunk | Metric | Before | After | Δ |")
        lines.append("|----------|-------|--------|--------|-------|---|")
        for r in improvements[:20]:  # cap display at 20
            before = r.get("before")
            after = r.get("after")
            metric = r.get("metric", "?")
            fmt_before = f"{before:.1f}" if before is not None else "—"
            fmt_after = f"{after:.1f}" if after is not None else "—"
            if metric == "coverage_gap":
                fmt_before = str(int(before)) if before is not None else "—"
                fmt_after = str(int(after)) if after is not None else "—"
            elif metric == "bench_duration":
                fmt_before = f"{before:.4f}ms" if before is not None else "—"
                fmt_after = f"{after:.4f}ms" if after is not None else "—"
            lines.append(f"| {r.get('assembly', '?')} | {r.get('slug', '?')} | "
                         f"{metric} | {fmt_before} | {fmt_after} | **{r.get('delta', '?'):+.1f}** |")
    else:
        lines.append("_No improvements detected._")
    lines.append("")

    # ── Per-Assembly Detail ──
    lines.append("### Per-Assembly Detail")
    lines.append("")
    if assemblies:
        lines.append("| Assembly | Fact | ΔFact | Bench(ms) | ΔBench | Coverage | HU |")
        lines.append("|----------|------|-------|-----------|--------|----------|-----|")
        for a in sorted(assemblies, key=lambda x: x.get("assembly", "")):
            name = a.get("assembly", "?")
            rate = a.get("factPassRate")
            rate_str = _fmt_pct(rate) if rate is not None else "—"
            delta_str = _fmt_delta(a.get("factPassRateDelta"), "%")
            dur = a.get("benchComparison", {}).get("aggregate", {}).get("meanDurationMs")
            dur_str = f"{dur:.4f}" if dur else "—"
            gap = a.get("totalCoverageGap", 0)
            hu = a.get("hotupdate", {})
            hu_str = "✅" if hu.get("chunksPatchFailed", 0) == 0 and hu.get("totalFailed", 0) == 0 else "❌"

            # Find benchmark delta from per-chunk aggregates
            bench_delta_sum = 0
            bench_delta_count = 0
            for ck, cd in chunks.items():
                if ck.startswith(name + "/"):
                    bd = cd.get("benchDurationDelta")
                    if bd is not None:
                        bench_delta_sum += bd
                        bench_delta_count += 1
            bench_delta_str = _fmt_delta(
                round(bench_delta_sum / bench_delta_count, 1) if bench_delta_count > 0 else None, "%"
            )

            lines.append(f"| {name} | {rate_str} | {delta_str} | {dur_str} | {bench_delta_str} | {gap} | {hu_str} |")
    else:
        lines.append("_No assembly data available._")
    lines.append("")

    # ── Footer ──
    lines.append("---")
    lines.append(f"_Generated at {datetime.now(timezone.utc).strftime('%Y-%m-%d %H:%M:%S UTC')}_")
    lines.append("")

    return "\n".join(lines)


def main() -> int:
    import argparse

    parser = argparse.ArgumentParser(description="Generate Markdown nightly summary from delta JSON")
    parser.add_argument("--delta", default="nightly-delta.json",
                        help="Path to nightly-delta.json (default: nightly-delta.json)")
    parser.add_argument("--output", default="nightly-summary.md",
                        help="Output path for Markdown (default: nightly-summary.md)")
    args = parser.parse_args()

    delta_path = Path(args.delta)
    if not delta_path.exists():
        print(f"  [nightly-summary] ERROR: delta file not found: {delta_path}", file=sys.stderr)
        return 1

    delta = json.loads(delta_path.read_text(encoding="utf-8"))
    summary = generate_summary(delta)

    output = Path(args.output)
    output.parent.mkdir(parents=True, exist_ok=True)
    output.write_text(summary, encoding="utf-8")
    print(f"  [nightly-summary] Written to {output}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
