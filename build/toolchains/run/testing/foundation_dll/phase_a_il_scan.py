"""
Phase A — Full IL opcode scanner for CoreLib families.

Iterates all families, collects unique opcodes from typed-il-ir.json,
compares against known emitter coverage, and produces a gap report.

Usage:
    python phase_a_il_scan.py --output gap-report.json
    python phase_a_il_scan.py --output gap-report.json --verbose
"""

from __future__ import annotations

import json
import sys
import time
from pathlib import Path
from typing import Any

_HERE = Path(__file__).resolve().parent
_REPO_ROOT = _HERE.parents[4]
_VERIFICATION = _REPO_ROOT / "verification" / "foundation-dll" / "System.Private.CoreLib"
sys.path.insert(0, str(_HERE))

from emitter_coverage_known import EMITTER_KNOWN_OPCODES, LOADER_HANDLED_OPCODES, EMITTER_ALL_KNOWN


def _normalize_opcode_name(op: str) -> str:
    """Normalize opcode names to IL convention.

    The emitter switch uses IL convention (e.g. "ldc.i4", "call").
    typed-il-ir.json may use the same convention or PascalCase.
    """
    # Already in IL convention (most common case)
    if "." in op or "_" in op:
        return op.lower()
    # PascalCase -> IL convention
    pascal_map = {
        "add_ovf_un": "add.ovf.un", "add_ovf": "add.ovf",
        "sub_ovf": "sub.ovf", "mul_ovf": "mul.ovf",
        "conv_i": "conv.i", "conv_i1": "conv.i1", "conv_i2": "conv.i2",
        "conv_i4": "conv.i4", "conv_i8": "conv.i8",
        "conv_u": "conv.u", "conv_u1": "conv.u1", "conv_u2": "conv.u2",
        "conv_u4": "conv.u4", "conv_u8": "conv.u8",
        "conv_r4": "conv.r4", "conv_r8": "conv.r8",
        "conv_ovf_i": "conv.ovf.i", "conv_ovf_i1": "conv.ovf.i1",
        "conv_ovf_i2": "conv.ovf.i2", "conv_ovf_i4": "conv.ovf.i4",
        "conv_ovf_i8": "conv.ovf.i8",
        "conv_ovf_u": "conv.ovf.u", "conv_ovf_u1": "conv.ovf.u1",
        "conv_ovf_u2": "conv.ovf.u2", "conv_ovf_u4": "conv.ovf.u4",
        "conv_ovf_u8": "conv.ovf.u8",
        "conv_ovf_i_un": "conv.ovf.i.un", "conv_ovf_u_un": "conv.ovf.u.un",
        "conv_ovf_i1_un": "conv.ovf.i1.un", "conv_ovf_i2_un": "conv.ovf.i2.un",
        "conv_ovf_i4_un": "conv.ovf.i4.un", "conv_ovf_i8_un": "conv.ovf.i8.un",
        "conv_ovf_u1_un": "conv.ovf.u1.un", "conv_ovf_u2_un": "conv.ovf.u2.un",
        "conv_ovf_u4_un": "conv.ovf.u4.un",
        "ldind_i": "ldind.i", "ldind_i1": "ldind.i1", "ldind_i2": "ldind.i2",
        "ldind_i4": "ldind.i4", "ldind_i8": "ldind.i8",
        "ldind_u1": "ldind.u1", "ldind_u2": "ldind.u2", "ldind_u4": "ldind.u4",
        "ldind_r4": "ldind.r4", "ldind_r8": "ldind.r8",
        "ldind_ref": "ldind.ref",
        "stind_i": "stind.i", "stind_i1": "stind.i1", "stind_i2": "stind.i2",
        "stind_i4": "stind.i4", "stind_i8": "stind.i8",
        "stind_r4": "stind.r4", "stind_r8": "stind.r8",
        "stind_ref": "stind.ref",
        "cgt_un": "cgt.un", "clt_un": "clt.un",
        "bgt_un": "bgt.un", "blt_un": "blt.un", "bge_un": "bge.un",
        "ble_un": "ble.un", "bne_un": "bne.un",
        "add_ovf_un": "add.ovf.un", "sub_ovf_un": "sub.ovf.un", "mul_ovf_un": "mul.ovf.un",
        "conv_r_un": "conv.r.un",
        "ldelem_ref": "ldelem.ref", "stelem_ref": "stelem.ref",
    }
    return pascal_map.get(op, op.lower())


def scan_family_opcodes(family_slug: str) -> dict:
    """Scan one family's typed-il-ir.json and return opcode data."""
    ir_path = _VERIFICATION / family_slug / "entrypoint" / "closure-sp" / "analysis" / "typed-il-ir.json"
    if not ir_path.exists():
        return {"family": family_slug, "status": "NO_IR", "opcodes": [], "method_count": 0}

    data = json.loads(ir_path.read_text(encoding="utf-8"))
    methods = data.get("methods", [])
    all_opcodes: set[str] = set()
    per_method: dict[str, set[str]] = {}

    for m in methods:
        sid = m.get("subjectId", "")
        method_opcodes: set[str] = set()
        for blk in m.get("blocks", []):
            for inst in blk.get("instructions", []):
                op = inst.get("op", "")
                if op:
                    normalized = _normalize_opcode_name(op)
                    all_opcodes.add(normalized)
                    method_opcodes.add(normalized)
        if method_opcodes:
            per_method[sid] = method_opcodes

    return {
        "family": family_slug,
        "status": "OK",
        "method_count": len(methods),
        "unique_opcodes": sorted(all_opcodes),
        "opcode_count": len(all_opcodes),
        "per_method": {k: sorted(v) for k, v in per_method.items()},
    }


def scan_all_families() -> list[dict]:
    """Scan all CoreLib families."""
    families = sorted([
        d.name for d in _VERIFICATION.iterdir()
        if d.is_dir() and (d / "capability-family-contract.json").exists()
    ])

    results = []
    for slug in families:
        result = scan_family_opcodes(slug)
        results.append(result)
        print(f"  {slug:35s} {result['opcode_count']:3d} opcodes  ({result['method_count']:3d} methods)")
    return results


def analyze_gaps(scan_results: list[dict]) -> dict:
    """Compare scanned opcodes against emitter coverage."""
    all_scanned: set[str] = set()
    per_family_gaps: dict[str, list[str]] = {}

    for r in scan_results:
        if r["status"] != "OK":
            continue
        family = r["family"]
        scanned = set(r["unique_opcodes"])
        all_scanned.update(scanned)
        missing = scanned - EMITTER_ALL_KNOWN
        per_family_gaps[family] = sorted(missing)

    # Aggregate across all families
    total_gap = all_scanned - EMITTER_ALL_KNOWN

    # Classify each gap
    gap_classification: dict[str, dict] = {}
    for op in sorted(total_gap):
        families_with_gap = []
        for family, gaps in per_family_gaps.items():
            if op in gaps:
                families_with_gap.append(family)

        # Determine classification
        if op in EMITTER_KNOWN_OPCODES:
            classification = "in-known-but-not-in-all-known"
        elif op.replace(".s", "") in EMITTER_KNOWN_OPCODES or \
             op.rstrip(".0123456789") in EMITTER_KNOWN_OPCODES:
            classification = "short-form-normalizable"
        elif op in LOADER_HANDLED_OPCODES:
            classification = "loader-handled"
        else:
            # Check if it's a short-form variant
            base = op.split(".")[0]
            if base in {"ldarg", "ldloc", "stloc", "ldc"} or op.endswith(".s"):
                classification = "short-form-normalizable"
            elif op.startswith("ldelem.") or op.startswith("stelem."):
                classification = "typed-array-element-loader-resolved"
            elif op.startswith("ldind.") or op.startswith("stind."):
                classification = "indirect-emitter-gap"
            elif op.startswith("conv.") or op.startswith("conv_ovf"):
                classification = "conversion-emitter-gap"
            elif op.startswith("add") or op.startswith("sub") or op.startswith("mul") or op.startswith("div") or op.startswith("rem"):
                classification = "arithmetic-emitter-gap"
            else:
                classification = "emitter-switch-gap"

        families_with_gap_sorted = sorted(set(families_with_gap))
        gap_classification[op] = {
            "opcode": op,
            "classification": classification,
            "families": families_with_gap_sorted,
            "family_count": len(families_with_gap_sorted),
        }

    return {
        "total_families": len([r for r in scan_results if r["status"] == "OK"]),
        "all_scanned_opcodes": sorted(all_scanned),
        "scanned_count": len(all_scanned),
        "emitter_known_count": len(EMITTER_ALL_KNOWN),
        "covered_count": len(all_scanned & EMITTER_ALL_KNOWN),
        "gap_count": len(total_gap),
        "gaps": gap_classification,
    }


def main():
    import argparse
    parser = argparse.ArgumentParser(description="Phase A IL opcode scanner")
    parser.add_argument("--output", default="phase-a-gap-report.json",
                        help="Output gap report path")
    parser.add_argument("--verbose", action="store_true",
                        help="Show per-family opcode details")
    args = parser.parse_args()

    t0 = time.time()

    print(f"Scanning all CoreLib families...")
    print(f"{'Family':35s} {'Opcodes':>8s}  Methods")
    print("-" * 55)
    scan_results = scan_all_families()
    elapsed = time.time() - t0

    print(f"\nScan completed in {elapsed:.1f}s: {len(scan_results)} families")

    # Analyze gaps
    gap_report = analyze_gaps(scan_results)

    print(f"\nEmitter coverage: {gap_report['emitter_known_count']} known opcodes")
    print(f"Scanned total:    {gap_report['scanned_count']} unique opcodes across all families")
    print(f"Covered:          {gap_report['covered_count']}")
    print(f"Gaps:             {gap_report['gap_count']}")
    print()

    if gap_report["gaps"]:
        # Group by classification
        by_class: dict[str, list] = {}
        for op, info in gap_report["gaps"].items():
            by_class.setdefault(info["classification"], []).append(info)

        print("Gap classification:")
        for cls, items in sorted(by_class.items()):
            print(f"  {cls:s}: {len(items)} opcodes")
            for item in items[:5]:
                families_str = ", ".join(item["families"][:3])
                extra = "..." if len(item["families"]) > 3 else ""
                print(f"    {item['opcode']:20s} ({item['family_count']:2d} families: {families_str}{extra})")
            if len(items) > 5:
                print(f"    ... and {len(items) - 5} more")
        print()

    if args.verbose and gap_report["gaps"]:
        print("Full gap detail:")
        for op, info in sorted(gap_report["gaps"].items()):
            print(f"  {op:20s} [{info['classification']:35s}] {info['family_count']:2d} families")
            for f in info["families"]:
                print(f"    - {f}")

    # Write output
    output_path = Path(args.output)
    output_path = args.output if Path(args.output).is_absolute() else _HERE / args.output
    output_path = Path(args.output)

    report = {
        "schemaVersion": 1,
        "assemblyName": "System.Private.CoreLib",
        "scanTime": time.strftime("%Y-%m-%dT%H:%M:%S"),
        "scanDurationSeconds": round(elapsed, 1),
        "summary": {
            "totalFamiliesScanned": gap_report["total_families"],
            "emitterKnownOpcodes": gap_report["emitter_known_count"],
            "scannedUniqueOpcodes": gap_report["scanned_count"],
            "coveredOpcodes": gap_report["covered_count"],
            "gapOpcodes": gap_report["gap_count"],
            "coveragePercentage": round(
                gap_report["covered_count"] / gap_report["scanned_count"] * 100, 1
            ) if gap_report["scanned_count"] else 0,
        },
        "allScannedOpcodes": list(gap_report["all_scanned_opcodes"]),
        "gaps": list(gap_report["gaps"].values()),
        "perFamily": [],
    }

    # Add per-family summary
    for r in scan_results:
        fam_gaps = gap_report.get("gaps", {})
        family_gap_list = []
        for op in r.get("unique_opcodes", []):
            if op in fam_gaps:
                family_gap_list.append(op)
        report["perFamily"].append({
            "family": r["family"],
            "status": r["status"],
            "methodCount": r["method_count"],
            "opcodeCount": r["opcode_count"],
            "opcodes": r["unique_opcodes"],
            "gaps": family_gap_list,
            "gapCount": len(family_gap_list),
        })

    output_path = Path(args.output)
    output_path.parent.mkdir(parents=True, exist_ok=True)
    output_path.write_text(json.dumps(report, indent=2, ensure_ascii=False) + "\n", encoding="utf-8")
    print(f"Report written to: {output_path}")


if __name__ == "__main__":
    main()
