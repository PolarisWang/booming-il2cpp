#!/usr/bin/env python3
"""Roll up per-DLL hotupdate data into one standardized hotupdate report.

Why
---
Hotupdate results are embedded in each DLL's fact-summary.json (the `hotupdate`
field under `chunkSummaries`), but nothing reads them cross-DLL.  This pulls
hotupdate status for all 76+ chunks into one view: which chunks have patch data,
which failed, which have semantic changes, and which are stale.

Framework-only: actual hotupdate data collection and comprehensive validation
is deferred to M3.  What the pipeline already records is surfaced here.

Input
-----
Same fact-summary.json that fact_report.py reads — the hotupdate data lives
inside it, not in a separate file.
"""
from __future__ import annotations

import argparse
import json
import sys
from datetime import datetime, timezone
from pathlib import Path

SCHEMA_VERSION = 1
_HEPH_CACHE = ".hephaestus-cache"


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
        path = dll_dir / "_dll" / "reports" / "latest" / "fact-summary.json"
        if not path.is_file():
            continue

        s = _load_json(path)
        if s is None:
            continue

        ts = _parse_ts(s.get("timestamp"))
        age = (now - ts).total_seconds() / 86400.0 if ts else None

        # Collect per-chunk hotupdate data, filtering the build cache entry.
        chunks: list[dict] = []
        dll_pass, dll_fail, dll_sem_changed = 0, 0, 0
        dll_patch_failed, dll_crash = 0, 0
        dll_real_pass, dll_real_fail = 0, 0
        dll_smoke_pass, dll_smoke_fail = 0, 0
        has_data = False

        for c in s.get("chunkSummaries") or []:
            slug = c.get("slug")
            if not slug or slug == _HEPH_CACHE:
                continue

            hu = c.get("hotupdate")
            if hu is None:
                continue

            passed = hu.get("passed")
            failed = hu.get("failed")
            if passed is not None or failed is not None:
                has_data = True
                dll_pass += passed or 0
                dll_fail += failed or 0
                dll_real_pass += hu.get("realPassed", 0) or 0
                dll_real_fail += hu.get("realFailed", 0) or 0
                dll_smoke_pass += hu.get("smokePassed", 0) or 0
                dll_smoke_fail += hu.get("smokeFailed", 0) or 0
                if hu.get("semanticChangedCount"):
                    dll_sem_changed += hu["semanticChangedCount"]
                if hu.get("patchFailed"):
                    dll_patch_failed += 1
                if hu.get("crash"):
                    dll_crash += 1

            chunks.append({
                "slug": slug,
                "passed": passed,
                "failed": failed,
                "allSemantic": hu.get("allSemantic"),
                "allRevert": hu.get("allRevert"),
                "patchDataUsed": hu.get("patchDataUsed"),
                "patchFailed": hu.get("patchFailed"),
                "crash": hu.get("crash"),
                "assertFailed": hu.get("assertFailed", 0),
                "semanticChangedCount": hu.get("semanticChangedCount", 0),
                "truncated": hu.get("truncated"),
                # ── real-vs-smoke split (ENG-34919) ──
                "realPassed": hu.get("realPassed", 0),
                "realFailed": hu.get("realFailed", 0),
                "smokePassed": hu.get("smokePassed", 0),
                "smokeFailed": hu.get("smokeFailed", 0),
                "realTotal": hu.get("realTotal", 0),
                "smokeTotal": hu.get("smokeTotal", 0),
                "unverifiedSubjects": hu.get("unverifiedSubjects", []),
            })

        if not has_data:
            continue

        dlls.append({
            "assemblyName": s.get("assemblyName") or dll_dir.name,
            "timestamp": s.get("timestamp"),
            "ageDays": round(age, 2) if age is not None else None,
            "stale": age is not None and age > stale_days,
            "chunksWithHotupdate": len(chunks),
            "totalPassed": dll_pass,
            "totalFailed": dll_fail,
            "realPassed": dll_real_pass,
            "realFailed": dll_real_fail,
            "smokePassed": dll_smoke_pass,
            "smokeFailed": dll_smoke_fail,
            "semanticChangedCount": dll_sem_changed,
            "patchFailedChunks": dll_patch_failed,
            "crashChunks": dll_crash,
            "chunks": chunks,
        })

    tot_pass = sum(d["totalPassed"] for d in dlls)
    tot_fail = sum(d["totalFailed"] for d in dlls)
    tot_chunks = sum(d["chunksWithHotupdate"] for d in dlls)
    tot_real_pass = sum(d["realPassed"] for d in dlls)
    tot_real_fail = sum(d["realFailed"] for d in dlls)
    tot_smoke_pass = sum(d["smokePassed"] for d in dlls)
    tot_smoke_fail = sum(d["smokeFailed"] for d in dlls)
    tot_real = tot_real_pass + tot_real_fail

    return {
        "schemaVersion": SCHEMA_VERSION,
        "generatedAt": now.isoformat(),
        "staleThresholdDays": stale_days,
        "totals": {
            "dllCount": len(dlls),
            "chunksWithHotupdate": tot_chunks,
            "totalPassed": tot_pass,
            "totalFailed": tot_fail,
            "overallPassRate": round(tot_pass / (tot_pass + tot_fail) * 100, 2)
                if (tot_pass + tot_fail) else None,
            # ── real-vs-smoke split (ENG-34919) ──
            "realPassed": tot_real_pass,
            "realFailed": tot_real_fail,
            "smokePassed": tot_smoke_pass,
            "smokeFailed": tot_smoke_fail,
            "realTotal": tot_real,
            "smokeTotal": tot_smoke_pass + tot_smoke_fail,
            # Genuine pass rate = real methods only.  None when no real method
            # was exercised (all-smoke: the chunk's hotupdate proves nothing).
            "realPassRate": round(tot_real_pass / tot_real * 100, 2) if tot_real else None,
            "semanticChangedTotal": sum(d["semanticChangedCount"] for d in dlls),
            "dllsWithPatchFailures": sum(1 for d in dlls if d["patchFailedChunks"]),
            "dllsWithCrashes": sum(1 for d in dlls if d["crashChunks"]),
            "staleDlls": sum(1 for d in dlls if d["stale"]),
        },
        "dlls": dlls,
    }


def render_markdown(r: dict) -> str:
    t = r["totals"]
    real_pr = t.get("realPassRate")
    lines = [
        "# Hotupdate 报告（跨 DLL 汇总）",
        "",
        f"> 生成于 {r['generatedAt']} · stale 阈值 {r['staleThresholdDays']} 天",
        "",
        "## 总量",
        "",
        f"- DLL 有 hotupdate 数据：{t['dllCount']}",
        f"- Chunk 覆盖：{t['chunksWithHotupdate']}",
        f"- 通过/失败（名义）：{t['totalPassed']} / {t['totalFailed']}"
        f"（{t['overallPassRate']}%）",
        f"- **真实验证 through/fail**：{t['realPassed']} / {t['realFailed']}"
        f"{f'（{real_pr}%）' if real_pr is not None else '（无真实方法）'}",
        f"- UNVERIFIED（smoke 占位）：{t['smokePassed']} passed / {t['smokeFailed']} failed",
        f"- Semantic changes：{t['semanticChangedTotal']}",
        f"- Patch 失败的 DLL：{t['dllsWithPatchFailures']}",
        f"- Crash 的 DLL：{t['dllsWithCrashes']}",
        f"- 陈旧报告：{t['staleDlls']}",
        "",
        "## 逐 DLL",
        "",
        "| DLL | Chunks | 通过/失败(名义) | 真实通过 | 真实失败 | Smoke占位 | Sem变化 | Patch失败 | Crash | 年龄(天) |",
        "|-----|--------|----------------|---------|---------|----------|--------|----------|-------|---------|",
    ]
    for d in sorted(r["dlls"], key=lambda x: x["assemblyName"]):
        pf = f"{d['totalPassed']}/{d['totalFailed']}"
        rp = f"{d['realPassed']}/{d['realFailed']}"
        sp = f"{d['smokePassed']}/{d['smokeFailed']}"
        flags = []
        if d["stale"]:
            flags.append("⏰")
        if d["patchFailedChunks"]:
            flags.append("💥")
        if d["crashChunks"]:
            flags.append("🔥")
        lines.append(
            f"| {d['assemblyName']} | {d['chunksWithHotupdate']} | "
            f"{pf} | {rp} | {sp} | "
            f"{d['semanticChangedCount']} | "
            f"{d['patchFailedChunks']} | {d['crashChunks']} | "
            f"{d['ageDays'] or '—'} {' '.join(flags)} |")

    # ⚠️ Real-vs-smoke note
    t_real = t.get("realPassed", 0) + t.get("realFailed", 0)
    t_smoke = t.get("smokePassed", 0) + t.get("smokeFailed", 0)
    total_nominal = t.get("totalPassed", 0) + t.get("totalFailed", 0)
    if t_smoke > 0:
        pct = round(t_smoke / total_nominal * 100, 1) if total_nominal else 0
        lines.extend([
            "",
            "---",
            "",
            f"> ⚠️ 全部 hotupdate 结果中 **{pct}%**（{t_smoke}/{total_nominal}）",
            "来自 UNVERIFIED 占位方法（smoke），未被真实热更验证。",
            f"> 经真实验证的方法通过率为 {t.get('realPassRate', 'N/A')}%。",
        ])

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
    real_pr = t.get("realPassRate")
    print(f"hotupdate-report -> {args.output}")
    print(f"  DLLs          {t['dllCount']}")
    print(f"  chunks        {t['chunksWithHotupdate']}")
    print(f"  passed/failed  {t['totalPassed']}/{t['totalFailed']} ({t['overallPassRate']}%)")
    print(f"  real passed/fail {t['realPassed']}/{t['realFailed']} "
          f"(rate: {f'{real_pr}%' if real_pr is not None else 'n/a — no real methods'})")
    print(f"  smoke passed/fail {t['smokePassed']}/{t['smokeFailed']}")
    print(f"  stale         {t['staleDlls']}")
    return 0


if __name__ == "__main__":
    sys.exit(main())