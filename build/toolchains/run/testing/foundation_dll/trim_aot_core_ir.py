"""Trim aot-core-ir.json to keep only methods matching an entry prefix.

Unlike a simple prefix filter, this follows all instruction-level references
(call, newobj, ldftn, callvirt, ldfld, stfld, ldsfld, stsfld) from entry
methods so the emitter has ABI metadata for every opcode argument.

Usage:
    python trim_aot_core_ir.py ^
        --input verification/.../closure-sp/aot-core-ir.json ^
        --entry-prefix ConvertCharNativeEntry ^
        --output verification/.../closure-sp/aot-core-ir.json
"""

from __future__ import annotations

import argparse
import json
import sys
from pathlib import Path

# IL opcodes whose callee/target-reference identifies another method
_CALLEE_OPS = {"call", "callvirt", "newobj", "ldftn", "ldvirtftn"}


def trim_closure(ir_path: Path, entry_prefix: str, output_path: Path) -> None:
    with open(ir_path, encoding="utf-8") as f:
        data = json.load(f)

    methods = data.get("methods", [])
    if not methods:
        print("FATAL: no methods found in aot-core-ir.json", file=sys.stderr)
        sys.exit(1)

    # Build lookup by subject ID
    methods_by_sid: dict[str, dict] = {}
    for m in methods:
        sid = m.get("subjectId", "")
        if sid:
            methods_by_sid[sid] = m

    # Seed with entry-prefix methods
    retained: set[str] = set()
    for m in methods:
        sid = m.get("subjectId", "")
        if sid.startswith(entry_prefix):
            retained.add(sid)

    print(f"Entry methods matching '{entry_prefix}*': {len(retained)}")
    if not retained:
        print(f"WARNING: no methods matched prefix '{entry_prefix}'", file=sys.stderr)
        # Fall back: print available prefixes for debugging
        prefixes: set[str] = set()
        for m in methods:
            sid = m.get("subjectId", "")
            prefix = sid.split("/")[0] if "/" in sid else sid.split("::")[0]
            prefixes.add(prefix)
        sample = sorted(prefixes)[:10]
        print(f"Available assembly prefixes: {sample}", file=sys.stderr)

    # Collect every referenced method subject ID from retained methods
    # Then expand: also include any method that is the CALLEE of a retained method
    queue = list(retained)
    while queue:
        sid = queue.pop(0)
        m = methods_by_sid.get(sid)
        if m is None:
            continue
        for instr in m.get("instructions", []):
            op = instr.get("op", "")
            callee = instr.get("callee")
            tsymbol = instr.get("targetSymbol")

            # Follow method-reference opcodes
            if callee and op in _CALLEE_OPS:
                if callee in methods_by_sid and callee not in retained:
                    retained.add(callee)
                    queue.append(callee)

            # Also follow targetReference if it points to a method
            tref = instr.get("targetReference")
            if tref and tref.get("kind") == 2:  # MethodReference kind
                tref_sid = tref.get("subjectId", "")
                if tref_sid in methods_by_sid and tref_sid not in retained:
                    retained.add(tref_sid)
                    queue.append(tref_sid)

    total = len(methods)
    trimmed = [m for m in methods if m.get("subjectId", "") in retained]

    print(f"Trimmed {total} -> {len(trimmed)} methods")
    if not trimmed:
        print("FATAL: no methods retained", file=sys.stderr)
        sys.exit(1)

    data["methods"] = trimmed
    data["_trimmed"] = {
        "originalMethodCount": total,
        "trimmedMethodCount": len(trimmed),
        "entryPrefix": entry_prefix,
    }

    output_path.parent.mkdir(parents=True, exist_ok=True)
    with open(output_path, "w", encoding="utf-8") as f:
        json.dump(data, f, indent=2, ensure_ascii=False)

    print(f"Written to {output_path}")


def main() -> None:
    parser = argparse.ArgumentParser(description="Trim aot-core-ir.json to entry-prefix + referenced methods")
    parser.add_argument("--input", required=True, type=Path, help="Input aot-core-ir.json path")
    parser.add_argument("--entry-prefix", required=True, help="Subject ID prefix to retain (e.g. ConvertCharNativeEntry)")
    parser.add_argument("--output", required=True, type=Path, help="Output path")
    args = parser.parse_args()

    trim_closure(
        ir_path=args.input,
        entry_prefix=args.entry_prefix,
        output_path=args.output,
    )


if __name__ == "__main__":
    main()
