#!/usr/bin/env python3
"""EH P2 — EUR triage that separates TRUE EH semantics defects from ATG noise.

WHY THIS EXISTS
---------------
`classify_eur.py` splits EUR by `resultKind`, but that alone is not enough to
size the EH work.  The dominant producer of `caught && !assertFailed` records
turned out NOT to be an EH defect at all:

    AutoTestGenerator emits, for methods it maps to no native symbol, a subject
    whose entire body is `return 42L` carrying a comment:
        // [AOT smoke] <Ex> thrown by <call> (external assembly stub — skipping Throws)
    or a machine-readable marker:
        // AOT-STUB-GAP
        // [UNVERIFIED] AOT stub: ...

    Those subjects deliberately verify nothing about exceptions.  If the call
    they were probing does raise, the exception escapes the subject (it is never
    wrapped in Assert.Throws) and the runner records `caught=true`,
    `assertFailed=false` — indistinguishable, in the fact file alone, from a
    real EH failure.

So the fact record must be cross-referenced with the SUBJECT SOURCE, which is
where the intent is stated.  That is what this tool does.

CLASSIFICATION
    TRUE-EH     bodyAvailability=NativeGenerated AND the subject source has
                neither `skipping Throws` nor `AOT-STUB-GAP` → the subject was
                supposed to assert and did not.  THIS is the EH work-list.
    SMOKE-SKIP  subject source carries `skipping Throws` → ATG chose not to
                assert.  Not an EH defect (coverage question, not semantics).
    STUB-GAP    subject source carries `AOT-STUB-GAP` / `[UNVERIFIED]` or the
                fact layer reports a non-NativeGenerated body → no AOT body.
    UNKNOWN     no subject source found for the chunk (source not materialised).

USAGE
    python tools/eh/classify_eur2.py [--root artifacts/foundation-dll]
                                      [--subjects testing/foundation-dll]
                                      [--json tools/eh/eur-triage.json]
"""

from __future__ import annotations

import argparse
import glob
import json
import os
import sys
from collections import Counter, defaultdict


def iter_records(root: str):
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


def load_subject_markers(subjects_root: str) -> dict[str, dict[str, str]]:
    """Map 'Assembly/Chunk' -> {method_basename: marker}.

    The marker comment is emitted INSIDE the method body, immediately after the
    opening brace — NOT above the signature:

        public long AppendChild_6_XmlNode_0() {
            // [AOT smoke] <Ex> thrown by <call> (external assembly stub — skipping Throws)
            return 42L;
        }

    So the scan must track the CURRENT method name (from the `public` signature)
    and, while inside it, pick up any marker comment until the closing brace.
    """
    markers: dict[str, dict[str, str]] = {}
    pattern = os.path.join(subjects_root, "*", "chunks", "*", "managed",
                           "combined", "CombinedSubjects.cs")
    for path in glob.glob(pattern):
        parts = os.path.normpath(path).split(os.sep)
        try:
            i = parts.index("chunks")
            assembly, chunk = parts[i - 1], parts[i + 1]
        except (ValueError, IndexError):
            continue
        key = f"{assembly}/{chunk}"
        per = markers.setdefault(key, {})
        try:
            with open(path, encoding="utf-8", errors="replace") as fh:
                cur_method = ""
                marker = ""
                for line in fh:
                    stripped = line.strip()
                    # New method signature ends the previous method's scope.
                    if stripped.startswith("public ") and "(" in stripped and stripped.endswith("()"):
                        name = stripped.split()[-1][:-2]          # strip ()
                        base = name.rsplit("_", 1)[0] if name.rsplit("_", 1)[-1].isdigit() else name
                        if cur_method and marker:
                            per[base] = marker
                            per[cur_method] = marker
                        cur_method = name
                        marker = ""
                        continue
                    # Closing brace: method scope ends.
                    if stripped == "}":
                        if cur_method and marker:
                            base = cur_method.rsplit("_", 1)[0] if cur_method.rsplit("_", 1)[-1].isdigit() else cur_method
                            per[base] = marker
                            per[cur_method] = marker
                        cur_method = ""
                        marker = ""
                        continue
                    if not cur_method:
                        continue
                    if (stripped.startswith("// [AOT smoke]")
                            or stripped.startswith("// [smoke]")):
                        marker = marker or "SMOKE-SKIP"
                        continue
                    if (stripped.startswith("// AOT-STUB-GAP")
                            or stripped.startswith("// [UNVERIFIED]")):
                        marker = marker or "STUB-GAP"
                        continue
                    if stripped.startswith("// AOT-THROWS-ASSERT"):
                        marker = marker or "THROWS-ASSERT"
                        continue
        except OSError:
            continue
    return markers


def subject_key(subject_id: str) -> str:
    """CombinedSubjects/AutoGenerated.<...>.<Type>::<Method>__N:Ret() -> <Method>-base."""
    if not subject_id:
        return ""
    tail = subject_id.split("::")[-1]
    tail = tail.split(":")[0]
    return tail


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("--root", default="artifacts/foundation-dll")
    ap.add_argument("--subjects", default="testing/foundation-dll")
    ap.add_argument("--json", default="")
    ap.add_argument("--show", type=int, default=10)
    args = ap.parse_args()

    markers = load_subject_markers(args.subjects)
    print(f"subject sources scanned: {len(markers)} chunk(s)")

    buckets: Counter[str] = Counter()
    detail: dict[str, list[dict]] = defaultdict(list)
    by_chunk: Counter[tuple[str, str]] = Counter()

    for assembly, chunk, mode, rec in iter_records(args.root):
        if not (rec.get("caught") and not rec.get("assertFailed")):
            continue
        subj = subject_key(rec.get("methodSubjectId") or "")
        mk = markers.get(f"{assembly}/{chunk}", {})
        base = subj.rsplit("_", 1)[0]
        marker = mk.get(subj) or mk.get(base, "")

        if marker == "SMOKE-SKIP":
            bucket = "SMOKE-SKIP"
        elif marker == "STUB-GAP":
            bucket = "STUB-GAP"
        elif rec.get("bodyAvailability") != "NativeGenerated":
            bucket = "STUB-GAP"
        elif f"{assembly}/{chunk}" not in markers:
            bucket = "UNKNOWN"
        else:
            bucket = "TRUE-EH"

        buckets[bucket] += 1
        by_chunk[(f"{assembly}/{chunk}", bucket)] += 1
        if len(detail[bucket]) < args.show:
            detail[bucket].append({
                "chunk": f"{assembly}/{chunk}",
                "mode": mode,
                "subject": rec.get("methodSubjectId"),
                "resultKind": rec.get("resultKind"),
                "marker": marker,
            })

    print()
    print("EUR triage:")
    for b, n in buckets.most_common():
        print(f"  {n:6d}  {b}")
    print()
    print("TRUE-EH by chunk:")
    for (chunk, b), n in sorted(by_chunk.items(), key=lambda x: -x[1]):
        if b == "TRUE-EH":
            print(f"  {n:6d}  {chunk}")
    print()
    print("--- TRUE-EH samples ---")
    for it in detail.get("TRUE-EH", []):
        print(f"  {it['chunk']} [{it['mode']}] {it['resultKind']}: {it['subject']}")

    if args.json:
        out_dir = os.path.dirname(args.json) or "."
        os.makedirs(out_dir, exist_ok=True)
        with open(args.json, "w", encoding="utf-8") as fh:
            json.dump({
                "buckets": dict(buckets),
                "true_eh_by_chunk": {k[0]: v for k, v in by_chunk.items() if k[1] == "TRUE-EH"},
                "samples": dict(detail),
            }, fh, ensure_ascii=False, indent=2)
        print(f"\nwrote {args.json}")
    return 0


if __name__ == "__main__":
    sys.exit(main())