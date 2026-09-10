#!/usr/bin/env python3
"""Roll up per-DLL fact reports into one standardized fact report.

Why
---
Each DLL writes its own `_dll/reports/latest/fact-summary.json`, but nothing
reads them together.  The question a reviewer actually asks — "how much of the
foundation is verified, what is uncovered, and which numbers are stale?" — needs
a cross-DLL view that no single per-DLL file can answer.

Inputs are the per-DLL summaries that the translation pipeline already emits; no
data is recomputed here.  Output is one JSON document plus a Markdown summary.

Concepts
--------
pass rate        totalPassed / totalFactMethods, per DLL and aggregate.
uncovered chunk  a chunk dir present on disk with no fact entry in the summary
                 (the pipeline never produced results for it), or a chunk
                 reported by the summary whose dir is gone.
stale            the report's own timestamp is older than --stale-days.  A
                 summary can be internally consistent and still describe a tree
                 that has since changed; staleness is the only signal for that.

Usage
-----
    python fact_report.py --translation-root tests/e2e/translation \
        --output fact-report.json [--markdown fact-report.md] [--stale-days 14]
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
    """Parse the pipeline's naive-local timestamps as UTC.

    The producers write `datetime.now().isoformat()` with no offset.  Treating
    them as UTC is an approximation, but staleness is measured in days against a
    multi-week spread, so a timezone skew of hours cannot flip the verdict.
    """
    if not raw:
        return None
    try:
        dt = datetime.fromisoformat(raw.replace("Z", "+00:00"))
    except ValueError:
        return None
    return dt.replace(tzinfo=timezone.utc) if dt.tzinfo is None else dt


def _discover_chunks(dll_dir: Path) -> list[str]:
    """Chunk slugs present on disk (each is a dir under <dll>/chunks/)."""
    chunks = dll_dir / "chunks"
    if not chunks.is_dir():
        return []
    return sorted(
        p.name for p in chunks.iterdir()
        if p.is_dir() and not p.name.startswith(".")
    )


def collect(translation_root: Path, stale_days: float) -> dict:
    now = datetime.now(timezone.utc)
    dlls: list[dict] = []

    for dll_dir in sorted(p for p in translation_root.iterdir() if p.is_dir()):
        summary_path = dll_dir / "_dll" / "reports" / "latest" / "fact-summary.json"
        disk_chunks = _discover_chunks(dll_dir)

        if not summary_path.is_file():
            # A DLL with chunks but no summary is the interesting case: it was
            # attempted and produced nothing.  A DLL with neither is just not
            # part of this run's selection and is not reported as a gap.
            if disk_chunks:
                dlls.append({
                    "assemblyName": dll_dir.name,
                    "status": "no-report",
                    "diskChunks": disk_chunks,
                    "uncoveredChunks": disk_chunks,
                })
            continue

        s = _load_json(summary_path)
        if s is None:
            dlls.append({
                "assemblyName": dll_dir.name,
                "status": "unreadable-report",
                "diskChunks": disk_chunks,
                "uncoveredChunks": disk_chunks,
            })
            continue

        # The pipeline records `.hephaestus-cache` as a chunk entry, but it is a
        # build cache, not a testable chunk.  Filter it on both sides — disk
        # discovery already skips dotdirs, and forgetting it here reported a
        # phantom "vanished chunk" for nearly every DLL.
        reported = {
            c.get("slug") for c in s.get("chunkSummaries", [])
            if c.get("slug") and not c["slug"].startswith(".")
        }
        disk_set = set(disk_chunks)
        # Two distinct gaps: never ran (on disk, not reported) and vanished
        # (reported, no longer on disk).  Collapsing them would hide a deleted
        # chunk behind a missing one.
        never_ran = sorted(disk_set - reported)
        vanished = sorted(reported - disk_set)

        ts = _parse_ts(s.get("timestamp"))
        age_days = (now - ts).total_seconds() / 86400.0 if ts else None

        total = s.get("totalFactMethods") or 0
        passed = s.get("totalPassed") or 0
        dlls.append({
            "assemblyName": s.get("assemblyName") or dll_dir.name,
            "status": "ok",
            "timestamp": s.get("timestamp"),
            "ageDays": round(age_days, 2) if age_days is not None else None,
            "stale": age_days is not None and age_days > stale_days,
            "chunkCount": s.get("totalChunks") or 0,
            "factMethods": total,
            "passed": passed,
            "failed": max(total - passed, 0),
            "passRate": round(passed / total * 100, 2) if total else None,
            "valueWarnings": s.get("chunksWithValueWarnings") or 0,
            "diskChunks": disk_chunks,
            "reportedChunks": sorted(reported),
            "uncoveredChunks": never_ran,
            "vanishedChunks": vanished,
        })

    ok = [d for d in dlls if d["status"] == "ok"]
    tot = sum(d["factMethods"] for d in ok)
    pas = sum(d["passed"] for d in ok)

    return {
        "schemaVersion": SCHEMA_VERSION,
        "generatedAt": now.isoformat(),
        "staleThresholdDays": stale_days,
        "totals": {
            "dllCount": len(dlls),
            "reportedDlls": len(ok),
            "factMethods": tot,
            "passed": pas,
            "failed": max(tot - pas, 0),
            "passRate": round(pas / tot * 100, 2) if tot else None,
            "staleDlls": sum(1 for d in ok if d["stale"]),
            "dllsWithUncoveredChunks": sum(
                1 for d in ok if d["uncoveredChunks"] or d["vanishedChunks"]),
            "dllsMissingReport": sum(1 for d in dlls if d["status"] != "ok"),
        },
        "dlls": dlls,
    }


def render_markdown(r: dict) -> str:
    t = r["totals"]
    out = [
        "# Fact 报告（跨 DLL 汇总）",
        "",
        f"> 生成于 {r['generatedAt']} · stale 阈值 {r['staleThresholdDays']} 天",
        "",
        "## 总量",
        "",
        f"- DLL：{t['reportedDlls']}/{t['dllCount']} 有报告"
        f"（{t['dllsMissingReport']} 缺失）",
        f"- Fact：{t['passed']}/{t['factMethods']} 通过"
        f"（{t['passRate']}%），失败 {t['failed']}",
        f"- 陈旧报告：{t['staleDlls']} 个 DLL",
        f"- 存在未覆盖 chunk 的 DLL：{t['dllsWithUncoveredChunks']}",
        "",
        "## 逐 DLL",
        "",
        "| DLL | 通过率 | Fact | 失败 | Chunk | 年龄(天) | 状态 |",
        "|-----|--------|------|------|-------|---------|------|",
    ]
    for d in sorted(r["dlls"], key=lambda x: x["assemblyName"]):
        if d["status"] != "ok":
            out.append(
                f"| {d['assemblyName']} | — | — | — | "
                f"{len(d['diskChunks'])} | — | ⚠️ {d['status']} |")
            continue
        flags = []
        if d["stale"]:
            flags.append("⏰ stale")
        if d["uncoveredChunks"]:
            flags.append(f"❓{len(d['uncoveredChunks'])} 未覆盖")
        if d["vanishedChunks"]:
            flags.append(f"👻{len(d['vanishedChunks'])} 消失")
        if d["failed"]:
            flags.append(f"❌{d['failed']}")
        out.append(
            f"| {d['assemblyName']} | {d['passRate']}% | {d['passed']}/{d['factMethods']} "
            f"| {d['failed']} | {d['chunkCount']} | {d['ageDays']} "
            f"| {' '.join(flags) or '✅'} |")

    uncovered = [d for d in r["dlls"] if d.get("uncoveredChunks") or d.get("vanishedChunks")]
    if uncovered:
        out += ["", "## 未覆盖 / 消失 chunk", ""]
        for d in uncovered:
            if d.get("uncoveredChunks"):
                out.append(f"- **{d['assemblyName']}** 未覆盖: "
                           f"{', '.join(d['uncoveredChunks'])}")
            if d.get("vanishedChunks"):
                out.append(f"- **{d['assemblyName']}** 报告有但磁盘无: "
                           f"{', '.join(d['vanishedChunks'])}")
    return "\n".join(out) + "\n"


def main(argv: list[str] | None = None) -> int:
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--translation-root", default="tests/e2e/translation",
                    help="Root containing per-DLL translation dirs")
    ap.add_argument("--output", required=True, help="JSON report output path")
    ap.add_argument("--markdown", help="Optional Markdown summary output path")
    ap.add_argument("--stale-days", type=float, default=14.0,
                    help="Age (days) past which a report is flagged stale")
    args = ap.parse_args(argv)

    root = Path(args.translation_root)
    if not root.is_dir():
        print(f"ERROR: translation root not found: {root}", file=sys.stderr)
        return 2

    report = collect(root, args.stale_days)

    out = Path(args.output)
    out.parent.mkdir(parents=True, exist_ok=True)
    out.write_text(json.dumps(report, indent=2, ensure_ascii=False) + "\n",
                   encoding="utf-8")

    if args.markdown:
        md = Path(args.markdown)
        md.parent.mkdir(parents=True, exist_ok=True)
        md.write_text(render_markdown(report), encoding="utf-8")

    t = report["totals"]
    print(f"fact-report -> {out}")
    print(f"  DLLs      {t['reportedDlls']}/{t['dllCount']} reported"
          f" ({t['dllsMissingReport']} missing)")
    print(f"  fact      {t['passed']}/{t['factMethods']} ({t['passRate']}%)")
    print(f"  stale     {t['staleDlls']}")
    print(f"  uncovered {t['dllsWithUncoveredChunks']} DLLs with gaps")
    return 0


if __name__ == "__main__":
    sys.exit(main())
