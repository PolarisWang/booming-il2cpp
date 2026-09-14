#!/usr/bin/env python3
"""Export the codegen capability manifest into an ATG-consumable form.

The codegen emits ``aot-capability-manifest.json`` next to its generated C++
(one per chunk).  That file describes which managed methods the codegen can
dispatch natively:

  * ``exact``           — a concrete SimpleForward / InlineBody shape
  * ``generic-pattern`` — a prefix+methodName resolver (matches many overloads)
  * ``inline-pattern``  — a call-site expression expander

This tool folds those three kinds into a single lookup table keyed by
``Type::Method`` so the ATG value generator can ask, per method, "does the
codegen have an implementation?" and only inject a semantic (valid) input when
the answer is yes.

Injecting a valid input for an API with no implementation would trade one
unfixable failure for another; injecting for one *with* an implementation
turns a null-input smoke test into a real semantic assertion.

Usage::

    python tests/e2e/verification/tools/export_capability_table.py \\
        --manifest artifacts/.../aot-capability-manifest.json \\
        --out tests/e2e/translation/capability-table.json
"""
from __future__ import annotations

import argparse
import json
from pathlib import Path


def build_table(manifest: dict) -> dict:
    """Fold exact / generic / inline entries into one Type::Method -> status map."""
    real: dict[str, dict] = {}

    for entry in manifest.get("entries", []) or []:
        kind = entry.get("kind")

        if kind == "exact":
            type_name = entry.get("typeDisplayName")
            method_name = entry.get("methodName")
            if not type_name or not method_name:
                continue
            real[f"{type_name}::{method_name}"] = {
                "status": "real",
                "source": "exact",
                "nativeSymbol": entry.get("nativeSymbol"),
                "shapeKind": entry.get("shapeKind"),
            }

        elif kind in ("generic-pattern", "inline-pattern"):
            # Patterns are prefix-matched (e.g. "System.Type" + "GetProperty"),
            # so they cover every overload of that method on that type.  Record
            # them with a wildcard sentinel; the gate below matches by prefix.
            prefix = entry.get("typeDisplayNamePrefix")
            method_name = entry.get("methodName")
            if not prefix or not method_name:
                continue
            key = f"{prefix}::{method_name}"
            # An exact entry is strictly more informative — never downgrade it.
            real.setdefault(key, {
                "status": "real",
                "source": kind,
                "nativeSymbol": None,
                "shapeKind": None,
            })

    return {
        "schemaVersion": 1,
        "generatedFrom": manifest.get("generatedAt"),
        "counts": {
            "exact": manifest.get("totalExactShapes", 0),
            "genericPatterns": manifest.get("totalGenericPatterns", 0),
            "inlinePatterns": manifest.get("totalInlinePatterns", 0),
            "realKeys": len(real),
        },
        "real": real,
    }


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--manifest", required=True, type=Path,
                    help="aot-capability-manifest.json emitted by the codegen")
    ap.add_argument("--out", required=True, type=Path,
                    help="Destination capability-table.json")
    args = ap.parse_args()

    raw = args.manifest.read_text(encoding="utf-8-sig")  # tolerate BOM
    manifest = json.loads(raw)

    table = build_table(manifest)

    args.out.parent.mkdir(parents=True, exist_ok=True)
    args.out.write_text(json.dumps(table, indent=2, ensure_ascii=False),
                        encoding="utf-8")

    c = table["counts"]
    print(f"[capability-table] exact={c['exact']} generic={c['genericPatterns']} "
          f"inline={c['inlinePatterns']} -> {c['realKeys']} real keys")
    print(f"[capability-table] written to {args.out}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
