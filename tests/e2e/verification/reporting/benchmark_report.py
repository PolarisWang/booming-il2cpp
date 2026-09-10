#!/usr/bin/env python3
"""Roll up per-DLL benchmark reports into one standardized benchmark report.

Why
---
Same pattern as fact_report.py: each DLL writes benchmark-summary.json but
nothing reads them together.  The question — 'how many methods benchmarked,
what are the performance characteristics, which are stale?' — needs a cross-DLL
view.  Framework-only: actual baseline data collection is deferred to M3.

Input
-----
Each DLL's _dll/reports/latest/benchmark-summary.json (produced by the
translation pipeline's benchmark stage).

Output
------
  JSON with per-DLL and aggregate benchmark counts and performance summary.
  Optional Markdown for embedding in CI step summary.
"""
from __future__ import annotations

import argparse
import json
import sys
from datetime import datetime, timezone
from pathlib import Path

SCHEMA_VERSION = 1


def _load_json(path: Path) -> dict | None:
    try:
        return json.loads(path.read_text(encoding="utf-8"))
    except (OSError, json.JSONDecodeError):
        return None


def _parse_ts(raw: str | None) -> datetime | None:
    if not raw:
        return None
    try:
        dt = datetime.fromisoformat(raw.replace("Z", "+00:00"))
    except ValueError:
        return None
    return dt.replace(tzinfo=timezone.utc) if dt.tzinfo is None else dt


def collect(translation_root: Path, stale_days: float) -> dict:
    now = datetime.now(timezone.utc)
    dlls: list[dict] = []

    for dll_dir in sorted(p for p in translation_root.iterdir() if p.is_dir()):
        path = dll_dir / "_dll" / "reports" / "latest" / "benchmark-summary.json"
        if not path.is_file():
            continue

        s = _load_json(path)
        if s is None:
            continue

        ts = _parse_ts(s.get("timestamp"))
        age = (now - ts).total_seconds() / 86400.0 if ts else None

        perf = s.get("aggregatePerformance") or {}
        chunks = s.get("chunkSummaries") or []
        benchmarked_methods = 0
        total_iters = 0
        for c in chunks:
            bm = c.get("benchmark") or {}
            benchmarked_methods += bm.get("methodCount", 0)
            total_iters += bm.get("iterations", 0)

        dlls.append({
            "assemblyName": s.get("assemblyName") or dll_dir.name,
            "timestamp": s.get("timestamp"),
            "ageDays": round(age, 2) if age is not None else None,
            "stale": age is not None and age > stale_days,
            "chunkCount": s.get("totalChunks") or 0,
            "totalBenchmarkedMethods": s.get("totalBenchmarkedMethods") or 0,
            "benchmarkedMethods": benchmarked_methods,
            "totalIterations": total_iters,
            "aggregate": {
                "meanDurationMs": perf.get("meanDurationMs"),
                "meanOpsPerSecond": perf.get("meanOpsPerSecond"),
                "meanCv": perf.get("aggregateMeanCv"),
                "totalOutliers": perf.get("totalOutliers", 0),
            },
        })

    tot_bm = sum(d["totalBenchmarkedMethods"] for d in dlls)
    tot_iter = sum(d["totalIterations"] for d in dlls)
    durations = [d["aggregate"]["meanDurationMs"] for d in dlls
                 if d["aggregate"]["meanDurationMs"] is not None]

    return {
        "schemaVersion": SCHEMA_VERSION,
        "generatedAt": now.isoformat(),
        "staleThresholdDays": stale_days,
        "totals": {
            "dllCount": len(dlls),
            "totalBenchmarkedMethods": tot_bm,
            "totalIterations": tot_iter,
            "meanDurationMs": round(sum(durations) / len(durations), 4) if durations else None,
            "minDurationMs": min(durations) if durations else None,
            "maxDurationMs": max(durations) if durations else None,
            "staleDlls": sum(1 for d in dlls if d["stale"]),
        },
        "dlls": dlls,
    }


def render_markdown(r: dict) -> str:
    t = r["totals"]
    lines = [
        "# Benchmark 报告（跨 DLL 汇总）",
        "",
        f"> 生成于 {r['generatedAt']} · stale 阈值 {r['staleThresholdDays']} 天",
        "",
        "## 总量",
        "",
        f"- DLL：{t['dllCount']}",
        f"- Benchmark 方法：{t['totalBenchmarkedMethods']}（总迭代 {t['totalIterations']} 次）",
        f"- 平均耗时：{t['meanDurationMs']} ms（min {t['minDurationMs']} / max {t['maxDurationMs']}）",
        f"- 陈旧报告：{t['staleDlls']} 个",
        "",
        "## 逐 DLL",
        "",
        "| DLL | Benchmark 方法 | 迭代 | 平均耗时(ms) | Ops/s | CV | Outliers | 年龄(天) | 状态 |",
        "|-----|--------------|------|------------|-------|----|---------|---------|------|",
    ]
    for d in sorted(r["dlls"], key=lambda x: x["assemblyName"]):
        a = d["aggregate"]
        flags = "⏰ stale" if d["stale"] else "✅"
        md = a["meanDurationMs"]
        os = a["meanOpsPerSecond"]
        cv = a["meanCv"]
        out = a["totalOutliers"]
        lines.append(
            f"| {d['assemblyName']} | {d['totalBenchmarkedMethods']} | "
            f"{d['totalIterations']} | {md or '—'} | {os or '—'} | "
            f"{cv or '—'} | {out} | {d['ageDays'] or '—'} | {flags} |")

    return "\n".join(lines) + "\n"


def main(argv: list[str] | None = None) -> int:
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--translation-root", default="tests/e2e/translation")
    ap.add_argument("--output", required=True)
    ap.add_argument("--markdown")
    ap.add_argument("--stale-days", type=float, default=14.0)
    args = ap.parse_args(argv)

    root = Path(args.translation_root)
    if not root.is_dir():
        print(f"ERROR: translation root not found: {root}", file=sys.stderr)
        return 2

    report = collect(root, args.stale_days)
    Path(args.output).parent.mkdir(parents=True, exist_ok=True)
    Path(args.output).write_text(
        json.dumps(report, indent=2, ensure_ascii=False) + "\n", encoding="utf-8")

    if args.markdown:
        Path(args.markdown).write_text(render_markdown(report), encoding="utf-8")

    t = report["totals"]
    print(f"benchmark-report -> {args.output}")
    print(f"  DLLs              {t['dllCount']}")
    print(f"  benchmark methods  {t['totalBenchmarkedMethods']} ({t['totalIterations']} iter)")
    print(f"  mean dur           {t['meanDurationMs']} ms")
    print(f"  stale              {t['staleDlls']}")
    return 0


if __name__ == "__main__":
    sys.exit(main())