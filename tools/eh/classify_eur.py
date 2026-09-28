#!/usr/bin/env python3
"""EH P2 — Exception-Uncaught-Raise (EUR) classifier.

WHAT THIS MEASURES
------------------
A fact record with `caught == true && assertFailed == false` means: the subject
raised a managed exception, the runner caught it, and NO assertion had recorded
a failure.  That combination is the signature of an exception escaping before
the assertion the subject was supposed to run — the metric the EH roadmap calls
`preAssertionRaise` (~1614 records at the time of writing).

WHY A DEDICATED TOOL
--------------------
The raw count alone cannot drive work: the same signature is produced by
several unrelated causes, and only some of them are EH-semantics defects.  This
tool splits the population by `resultKind` / `bodyAvailability` so the true EH
set can be separated from stub gaps and test-harness noise before any fix is
attempted.

USAGE
    python tools/eh/classify_eur.py [--root artifacts/foundation-dll] [--json out.json]
"""

from __future__ import annotations

import argparse
import glob
import json
import os
import sys
from collections import Counter, defaultdict


def iter_records(root: str):
    """Yield (assembly, chunk, mode, record) for every fact-results.json."""
    pattern = os.path.join(root, "*", "chunks", "*", "results", "fact-results.json")
    for path in glob.glob(pattern):
        parts = os.path.normpath(path).split(os.sep)
        try:
            i = parts.index("chunks")
            assembly, chunk = parts[i - 1], parts[i + 1]
        except (ValueError, IndexError):
            continue
        try:
            with open(path, encoding="utf-8") as fh:
                data = json.load(fh)
        except (OSError, json.JSONDecodeError):
            continue
        for mode in ("aot", "jit"):
            for rec in data.get(mode, []) or []:
                if isinstance(rec, dict):
                    yield assembly, chunk, mode, rec


def is_eur(rec: dict) -> bool:
    return bool(rec.get("caught")) and not rec.get("assertFailed")


def classify(rec: dict) -> str:
    """Bucket an EUR record by its declared cause.

    The buckets are deliberately conservative: anything that is not clearly an
    EH-semantics problem is kept in its own bucket so it can be excluded from
    the EH work-list rather than silently inflating it.
    """
    kind = rec.get("resultKind")
    return {
        "realDefect": "eh-candidate",   # behaviour differs AOT vs managed — may be EH
        "failed": "unclassified",       # needs investigation; may or may not be EH
        "factoryGap": "non-eh",         # factory helper missing — not an EH path
        "nullArg": "non-eh",            # argument-validation path
        "stubGap": "non-eh",
        "unimplemented": "non-eh",
        "smoke": "non-eh",
        "envSensitive": "non-eh",
    }.get(kind, "other")


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("--root", default="artifacts/foundation-dll")
    ap.add_argument("--json", default="")
    ap.add_argument("--show", type=int, default=0,
                    help="print this many sample records per bucket")
    args = ap.parse_args()

    by_bucket = Counter()
    by_kind = Counter()
    by_chunk_kind = Counter()
    samples = defaultdict(list)
    total = 0

    for assembly, chunk, mode, rec in iter_records(args.root):
        if not is_eur(rec):
            continue
        total += 1
        bucket = classify(rec)
        kind = rec.get("resultKind")
        by_bucket[bucket] += 1
        by_kind[kind] += 1
        by_chunk_kind[(f"{assembly}/{chunk}", kind)] += 1
        if len(samples[bucket]) < args.show:
            samples[bucket].append({
                "assembly": assembly,
                "chunk": chunk,
                "mode": mode,
                "subject": rec.get("methodSubjectId"),
                "resultKind": kind,
                "bodyAvailability": rec.get("bodyAvailability"),
                "returnType": rec.get("returnType"),
            })

    print(f"EUR total: {total}")
    print()
    print("by bucket:")
    for b, n in by_bucket.most_common():
        print(f"  {n:6d}  {b}")
    print()
    print("by resultKind:")
    for k, n in by_kind.most_common():
        print(f"  {n:6d}  {k}")
    print()
    print("by chunk x resultKind:")
    for (chunk, kind), n in sorted(by_chunk_kind.items(), key=lambda x: -x[1]):
        print(f"  {n:6d}  {chunk:<46} {kind}")

    for b, items in samples.items():
        if not items:
            continue
        print()
        print(f"--- samples: {b} ---")
        for s in items:
            print(f"  {s['assembly']}/{s['chunk']} [{s['mode']}] kind={s['resultKind']}: {s['subject']}")

    if args.json:
        with open(args.json, "w", encoding="utf-8") as fh:
            json.dump({
                "total": total,
                "by_bucket": dict(by_bucket),
                "by_kind": dict(by_kind),
                "by_chunk_kind": {f"{k[0]}|{k[1]}": v for k, v in by_chunk_kind.items()},
            }, fh, ensure_ascii=False, indent=2)
        print(f"\nwrote {args.json}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
