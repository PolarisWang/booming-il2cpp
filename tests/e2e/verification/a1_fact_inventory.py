"""A1 定盘点法 — 盘点所有预编译 AOT entry 的 fact 缺口方法。

跑每个 entry.exe --fact-json, 提取每个方法 (si, methodIndex, value, passed)。
识别「返回 0 但 passed=false」(缺口方法), 汇聚成跨 assembly 清单。

用法: python a1_inventory.py [--entry-dir <root>]
"""
from __future__ import annotations

import json
import re
import subprocess
import sys
import time
from pathlib import Path

ENTRY_ROOT = Path(__file__).resolve().parent.parent.parent / "e2e" / "translation"


def find_entries(root: Path):
    """Yield all entry.exe paths under root."""
    return sorted(root.glob("*/chunks/*/native/entry.exe"))


def run_fact(entry: Path, timeout=120):
    """Run entry --fact-json; return parsed factResults list or None on failure."""
    try:
        r = subprocess.run(
            [str(entry), "--fact-json"],
            stdout=subprocess.PIPE, stderr=subprocess.PIPE,
            text=True, timeout=timeout, errors="replace",
            env={**__import__("os").environ, "CHAOS_IL2CPP_LOG_LEVEL": "1"},
        )
    except subprocess.TimeoutExpired:
        return None
    m = re.search(r"\{.*\"factResults\"\s*:\s*\[\s*\{(?:.*?)\}\s*\]\s*\}", r.stdout, re.S)
    if not m:
        return None
    try:
        obj = json.loads(m.group(0))
    except json.JSONDecodeError:
        return None
    return obj.get("factResults", [])


def main():
    root = ENTRY_ROOT
    if not root.exists():
        print(f"root not found: {root}")
        return
    entries = find_entries(root)
    print(f"found {len(entries)} entry.exe under {root}")

    total_methods = 0
    total_failed = 0
    total_total = 0
    gap_methods = []  # (entry, methodIndex, value, subject)
    all_rows = []

    for i, entry in enumerate(entries):
        res = run_fact(entry)
        if res is None:
            print(f"[{i+1}/{len(entries)}] {entry.relative_to(root)}: NO fact output")
            continue
        # map methodIndex -> subjectId for gap identification
        # subjectId needs the methods.json; read it if present
        methods_json = entry.parent / "codegen" / "generated" / "native-aot.methods.json"
        idx2subject = {}
        if methods_json.exists():
            try:
                d = json.load(open(methods_json, encoding="utf-8-sig"))
                idx2subject = {m.get("index"): m.get("subjectId", "") for m in d.get("methods", [])}
            except Exception:
                pass

        n = len(res)
        n_fail = sum(1 for m in res if not m.get("passed", True))
        n_zero = sum(1 for m in res if m.get("value", 1) == 0)
        total_methods += n
        total_failed += n_fail
        total_total += 1

        # gap: passed=false AND value==0
        for m in res:
            if not m.get("passed", True):
                gap_methods.append({
                    "entry": str(entry.relative_to(root)),
                    "methodIndex": m.get("methodIndex"),
                    "si": m.get("si"),
                    "value": m.get("value"),
                    "subjectId": idx2subject.get(m.get("methodIndex"), ""),
                })
            all_rows.append({
                "entry": str(entry.relative_to(root)),
                "methodIndex": m.get("methodIndex"),
                "si": m.get("si"),
                "value": m.get("value"),
                "passed": m.get("passed"),
                "subjectId": idx2subject.get(m.get("methodIndex"), ""),
            })

        print(f"[{i+1}/{len(entries)}] {entry.relative_to(root)}: {n} methods, "
              f"{n_fail} failed, {n_zero} zero-valued")
        time.sleep(0.1)

    print("\n" + "=" * 60)
    print(f"TOTAL: {len(entries)} entries, {total_methods} methods, {total_failed} failed")
    print(f"\n# Gap methods (passed=false, value=0): {len(gap_methods)}")
    for g in gap_methods:
        print(f"  [{g['entry']}] idx={g['methodIndex']} si={g['si']} val={g['value']}")
        if g["subjectId"]:
            print(f"      subject: {g['subjectId']}")

    # Save raw rows for further analysis
    out = Path(__file__).resolve().parent / "a1_fact_inventory.json"
    with open(out, "w", encoding="utf-8") as f:
        json.dump({"entries_total": total_total, "methods_total": total_methods,
                   "methods_failed": total_failed, "gap_methods": gap_methods,
                   "rows": all_rows}, f, indent=2, ensure_ascii=False)
    print(f"\nraw inventory -> {out}")


if __name__ == "__main__":
    sys.exit(main())
