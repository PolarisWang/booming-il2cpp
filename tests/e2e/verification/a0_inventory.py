"""A0 补跑 — 对 A1 超时的 8 个 System.Private.CoreLib 大 chunk 跑 fact 盘点缺口。

用法: python a0_inventory.py [--timeout 300]
"""
from __future__ import annotations

import json
import re
import subprocess
import sys
from pathlib import Path

CHUNK_ROOT = Path(__file__).resolve().parent.parent.parent \
    / "e2e" / "translation" / "System.Private.CoreLib" / "chunks"

TIMEOUT = int(sys.argv[sys.argv.index("--timeout") + 1]) if "--timeout" in sys.argv else 300

TARGET_CHUNKS = [
    "runtime-intrinsics", "runtime-intrinsics-2", "runtime-intrinsics-3", "runtime-intrinsics-4",
    "system", "system-11", "system-2", "system-8",
]


def run_fact(entry: Path, timeout: int):
    try:
        r = subprocess.run([str(entry), "--fact-json"],
                           stdout=subprocess.PIPE, stderr=subprocess.PIPE,
                           text=True, timeout=timeout, errors="replace",
                           env={**__import__("os").environ, "CHAOS_IL2CPP_LOG_LEVEL": "1"})
    except subprocess.TimeoutExpired as e:
        return None, f"TIMEOUT({timeout}s)"
    stdout = r.stdout or ""
    stderr_tail = (r.stderr or "")[-200:]
    if not stdout.strip():
        return None, f"EMPTY_STDOUT rc={r.returncode} stderr={stderr_tail!r}"
    m = re.search(r"\{.*\"factResults\"\s*:\s*\[\s*\{(?:.*?)\}\s*\]\s*\}", stdout, re.S)
    if not m:
        return None, f"NO_PARSE rc={r.returncode} stdout_len={len(stdout)} tail={stdout[-120:]!r}"
    try:
        return json.loads(m.group(0)).get("factResults", []), "OK"
    except json.JSONDecodeError as e:
        return None, f"BAD_JSON({e})"


def main():
    total_methods = 0
    total_failed = 0
    all_gaps = []
    for c in TARGET_CHUNKS:
        entry = CHUNK_ROOT / c / "native" / "entry.exe"
        if not entry.exists():
            print(f"[{c}] NO entry.exe")
            continue
        res, err = run_fact(entry, TIMEOUT)
        if res is None:
            print(f"[{c}] {err} (timeout={TIMEOUT}s)")
            continue
        methods_json = entry.parent / "codegen" / "generated" / "native-aot.methods.json"
        idx2sub = {}
        if methods_json.exists():
            try:
                d = json.load(open(methods_json, encoding="utf-8-sig"))
                idx2sub = {m.get("index"): m.get("subjectId", "") for m in d.get("methods", [])}
            except Exception:
                pass
        n = len(res)
        n_fail = sum(1 for m in res if not m.get("passed", True))
        total_methods += n
        total_failed += n_fail
        print(f"[{c}] {n} methods, {n_fail} failed")
        for m in res:
            if not m.get("passed", True):
                all_gaps.append({
                    "chunk": c, "methodIndex": m.get("methodIndex"),
                    "si": m.get("si"), "value": m.get("value"),
                    "subjectId": idx2sub.get(m.get("methodIndex"), ""),
                })
    print(f"\nTOTAL: {total_methods} methods, {total_failed} failed gaps (A0 补跑)")
    for g in all_gaps:
        print(f"  [{g['chunk']}] idx={g['methodIndex']} val={g['value']} | {g['subjectId']}")
    out = Path(__file__).resolve().parent / "a0_inventory.json"
    with open(out, "w", encoding="utf-8") as f:
        json.dump({"total_methods": total_methods, "total_failed": total_failed, "gaps": all_gaps},
                  f, indent=2, ensure_ascii=False)
    print(f"raw -> {out}")


if __name__ == "__main__":
    sys.exit(main())
