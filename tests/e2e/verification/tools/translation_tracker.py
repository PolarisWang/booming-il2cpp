#!/usr/bin/env python3
"""Translation-tracking scanner — report per-method AOT translation state across families.

Reads the per-chunk subjects.metadata.json (which now carries a bodyAvailability +
capabilitySummary stamped by the build stage after TPG codegen) and, where present,
the per-method fact-results.json, then prints a family-wide table of:

  * declared methods
  * annotated fact wrappers (non-aot-coverage)
  * NativeGenerated (real AOT C++) vs NoCanonicalBody/fallback
  * pass rate broken out by translation category

This is the offline, code-driven half of "翻译追踪".  It needs a REAL codegen + fact
pipeline run to populate the artifacts (the BodyAvailability field only appears once
the codegen emitter is rebuilt); until then it reports the raw metadata capability
summary that the build stage stamps even on current artifacts.

Usage:
    python -m verification.tools.translation_tracker \
        [--root artifacts/foundation-dll] [--family System.Collections.Immutable]

Backward-compatible: any chunk whose metadata lacks capabilitySummary is reported as
"no data" rather than failing, so the scanner is safe to run over older artifact trees.
"""

from __future__ import annotations

import argparse
import json
import sys
from pathlib import Path
from collections import Counter, defaultdict


def _scan_family(root: Path, family: str) -> dict:
    """Scan every chunk of one family; returns a summarized view."""
    fam_dirs = sorted((root / family).glob("chunks/*"))
    view: dict[str, object] = {
        "family": family,
        "chunks_with_data": 0,
        "declaredMethods": 0,
        "annotatedWrappers": 0,
        "nativeGenerated": 0,
        "noCanonicalBody": 0,
        "methodKindHistogram": Counter(),
        "failedByAvailability": Counter(),
        "byChunk": {},
    }
    per_fam_failed_ng: Counter = Counter()

    for chunk_dir in fam_dirs:
        if not chunk_dir.is_dir():
            continue
        chunk = chunk_dir.name
        meta_path = chunk_dir / "managed" / "subjects" / "subjects.metadata.json"
        if not meta_path.exists():
            continue
        try:
            meta = json.loads(meta_path.read_text(encoding="utf-8"))
        except (json.JSONDecodeError, OSError):
            continue
        methods = meta.get("methods") or []
        cs = meta.get("capabilitySummary") or {}
        annotated = cs.get("annotated", 0)

        entry = {
            "declared": len(methods),
            "annotatedWrappers": annotated,
            "nativeGeneratedDeclared": cs.get("nativeGenerated"),
            "capabilityStatus": "data" if cs else "no_capabilitySummary",
        }
        if not annotated:
            entry["capabilityStatus"] = "no_capabilitySummary"
        view["declaredMethods"] += entry["declared"]
        view["annotatedWrappers"] += annotated
        view["nativeGenerated"] += cs.get("nativeGenerated", 0)
        no_canon = max(0, annotated - cs.get("nativeGenerated", 0))
        view["noCanonicalBody"] += no_canon
        if annotated:
            view["chunks_with_data"] += 1

        # Per-method kind + any failure visibility in fact-results.json if annotated.
        for m in methods:
            view["methodKindHistogram"][m.get("kind", "?")] += 1
        fr_path = chunk_dir / "results" / "fact-results.json"
        if fr_path.exists():
            try:
                fr = json.loads(fr_path.read_text(encoding="utf-8"))
                for rec in fr.get("aot") or fr.get("jit") or []:
                    ba = rec.get("bodyAvailability")
                    if ba and not rec.get("passed", True):
                        view["failedByAvailability"][ba] += 1
            except (json.JSONDecodeError, OSError):
                pass
        view["byChunk"][chunk] = entry

    return view


def _print_view(view: dict) -> None:
    fam = view["family"]
    d = view["declaredMethods"]
    a = view["annotatedWrappers"]
    ng = view["nativeGenerated"]
    ncb = view["noCanonicalBody"]
    print(f"\nFamily: {fam}")
    print(f"  declared methods      : {d}")
    print(f"  annotated wrappers(fact): {a}")
    print(f"  NativeGenerated        : {ng}")
    print(f"  NoCanonicalBody/fallback: {ncb}")
    if a:
        print(f"  native share of annotated: {ng / a * 100:.1f}%")
    print(f"  method-kind histogram    : {dict(view['methodKindHistogram'])}")
    if view["failedByAvailability"]:
        print(f"  failures by availability : {dict(view['failedByAvailability'])}")
    print("  chunks with tracking data:", view["chunks_with_data"],
          f"/ need capabilitySummary stamped by a real codegen+fact run.")


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--root", default="artifacts/foundation-dll",
                    help="artifact root containing family dirs")
    ap.add_argument("--family", default=None,
                    help="limit to one family (default: all)")
    args = ap.parse_args()

    root = Path(args.root)
    if not root.is_dir():
        print(f"ERROR: root not found: {root}", file=sys.stderr)
        return 1

    families = [args.family] if args.family else \
        sorted(p.name for p in root.iterdir() if p.is_dir())
    for fam in families:
        _print_view(_scan_family(root, fam))
    return 0


if __name__ == "__main__":
    sys.exit(main())
