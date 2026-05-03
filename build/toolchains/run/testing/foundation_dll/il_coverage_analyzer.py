"""
IL Coverage Analyzer: Compares scanned IL opcodes against emitter coverage.
Produces gap reports per DLL and per capability family.

Usage:
    python il_coverage_analyzer.py --il-scan <il-scan.json> --output <gap-report.json>
    python il_coverage_analyzer.py --il-scan <il-scan.json> --family-contracts <contracts-dir> --output <family-gap-report.json>
"""

import json
import re
import sys
import time
from pathlib import Path

# Add the foundation_dll directory to path for emitter_coverage_known
_SCRIPT_DIR = Path(__file__).parent
sys.path.insert(0, str(_SCRIPT_DIR))
sys.path.insert(0, str(_SCRIPT_DIR.parent.parent))  # for testing.trace

from emitter_coverage_known import EMITTER_KNOWN_OPCODES, EMITTER_ALL_KNOWN


def load_il_scan(path: str) -> dict:
    """Load IL scan JSON output."""
    with open(path, "r", encoding="utf-8") as f:
        return json.load(f)


def analyze_opcode_gap(il_scan: dict) -> dict:
    """
    Compare scanned unique opcodes against emitter coverage.
    Returns a gap report.
    """
    scanned = set(il_scan.get("uniqueOpcodes", []))
    emitter_set = EMITTER_ALL_KNOWN

    # Normalize: the scanner uses ILOpCode.ToString() for unknown opcodes
    # which produces PascalCase names like "Conv_i". Match them to IL convention.
    normalized_scanned = set()
    for op in scanned:
        normalized = _normalize_opcode(op)
        normalized_scanned.add(normalized)

    covered = normalized_scanned & emitter_set
    missing = normalized_scanned - emitter_set

    return {
        "assemblyName": il_scan.get("assemblyName"),
        "totalMethods": il_scan.get("totalMethods"),
        "methodsWithBody": il_scan.get("methodsWithBody"),
        "scannedUniqueOpcodes": len(normalized_scanned),
        "emitterKnownOpcodes": len(emitter_set),
        "coveredOpcodes": sorted(covered),
        "coveredCount": len(covered),
        "missingOpcodes": sorted(missing),
        "missingCount": len(missing),
        "coveragePercentage": round(len(covered) / len(normalized_scanned) * 100, 1)
            if normalized_scanned else 0.0,
    }


def _normalize_opcode(op: str) -> str:
    """
    Normalize opcode names to IL convention.
    Handles cases like ILOpCode.ToString() output vs IL convention.
    """
    # Known IL opcode mappings for PascalCase -> IL convention
    pascal_to_il = {
        "Add_ovf_un": "add.ovf.un",
        "Bgt_un": "bgt.un",
        "Bgt_un_s": "bgt.un.s",
        "Ble_un": "ble.un",
        "Ble_un_s": "ble.un.s",
        "Blt_un": "blt.un",
        "Blt_un_s": "blt.un.s",
        "Conv_i": "conv.i",
        "Conv_ovf_i": "conv.ovf.i",
        "Conv_ovf_i_un": "conv.ovf.i.un",
        "Conv_ovf_i1_un": "conv.ovf.i1.un",
        "Conv_ovf_i2_un": "conv.ovf.i2.un",
        "Conv_ovf_i4_un": "conv.ovf.i4.un",
        "Conv_ovf_i8_un": "conv.ovf.i8.un",
        "Conv_ovf_u": "conv.ovf.u",
        "Conv_ovf_u_un": "conv.ovf.u.un",
        "Conv_ovf_u1_un": "conv.ovf.u1.un",
        "Conv_ovf_u2_un": "conv.ovf.u2.un",
        "Conv_ovf_u4_un": "conv.ovf.u4.un",
        "Conv_u4": "conv.u4",
        "Ldelem_i": "ldelem.i",
        "Ldind_i": "ldind.i",
        "Mul_ovf_un": "mul.ovf.un",
        "Stelem_i": "stelem.i",
        "Stind_i": "stind.i",
        "Starg_s": "starg.s",
        "Sub_ovf_un": "sub.ovf.un",
    }
    if op in pascal_to_il:
        return pascal_to_il[op]

    # If already in IL convention (lowercase, dots), return as-is
    if op[0].islower() and "." in op:
        return op

    # General PascalCase to IL: ConvI4 -> conv.i4
    result = re.sub(r"([a-z])([A-Z])", r"\1.\2", op).lower()
    return result


def analyze_family_gaps(il_scan: dict, contracts_dir: str = None) -> dict:
    """
    Analyze opcode gaps grouped by capability family.
    If contracts_dir is provided, uses actual family contract files to
    map methods to families.
    Otherwise, infers family groups from method naming patterns.
    """
    methods = il_scan.get("methods", [])
    family_methods = {}

    if contracts_dir:
        # Load actual family contracts
        contracts_path = Path(contracts_dir)
        for contract_file in sorted(contracts_path.rglob("method-capability-contracts.json")):
            family_gap = _analyze_single_family(contract_file, methods)
            if family_gap:
                family_methods[family_gap["familyId"]] = family_gap
    else:
        # Infer families from method subject ID prefixes
        for method in methods:
            msi = method.get("methodSubjectId", "")
            # Extract family-like prefix from method name
            family_key = _infer_family(msi)
            if family_key not in family_methods:
                family_methods[family_key] = {
                    "familyId": family_key,
                    "methodCount": 0,
                    "methodsWithBody": 0,
                    "uniqueOpcodes": set(),
                    "methods": [],
                }
            family_methods[family_key]["methodCount"] += 1
            if method.get("hasBody"):
                family_methods[family_key]["methodsWithBody"] += 1
                for op in method.get("uniqueOpcodes", []):
                    family_methods[family_key]["uniqueOpcodes"].add(_normalize_opcode(op))
            family_methods[family_key]["methods"].append(method)

    # Compute gaps per family
    emitter_set = EMITTER_ALL_KNOWN
    result = {}
    for fid, info in family_methods.items():
        unique = info.get("uniqueOpcodes", set())
        if isinstance(unique, set):
            missing = unique - emitter_set
            covered = unique & emitter_set
        else:
            missing = set()
            covered = set()

        result[fid] = {
            "familyId": fid,
            "methodCount": info["methodCount"],
            "methodsWithBody": info.get("methodsWithBody", 0),
            "uniqueOpcodeCount": len(unique),
            "coveredOpcodeCount": len(covered),
            "missingOpcodeCount": len(missing),
            "missingOpcodes": sorted(missing),
            "coveragePercentage": round(len(covered) / len(unique) * 100, 1) if unique else 100.0,
        }

    return result


def _analyze_single_family(contract_file: Path, all_methods: list) -> dict | None:
    """Analyze a single family by matching its contract methods against scanned methods."""
    import json
    try:
        contract = json.loads(contract_file.read_text(encoding="utf-8"))
    except (json.JSONDecodeError, FileNotFoundError):
        return None

    family_id = contract.get("familyId", str(contract_file.parent))
    method_ids = set(contract.get("methodSubjectIds", []))
    if not method_ids:
        return None

    # Build lookup from all_methods
    method_lookup = {m["methodSubjectId"]: m for m in all_methods}

    family_ops = set()
    matched_count = 0
    for msi in method_ids:
        if msi in method_lookup:
            matched_count += 1
            method = method_lookup[msi]
            if method.get("hasBody"):
                for op in method.get("uniqueOpcodes", []):
                    family_ops.add(_normalize_opcode(op))

    return {
        "familyId": family_id,
        "methodCount": len(method_ids),
        "matchedMethods": matched_count,
        "methodsWithBody": sum(1 for msi in method_ids
                                if msi in method_lookup and method_lookup[msi].get("hasBody")),
        "uniqueOpcodes": family_ops,
    }


def _infer_family(method_subject_id: str) -> str:
    """
    Infer capability family from method subject ID.
    Uses type namespace patterns to group methods.
    """
    # Extract type name from method subject ID
    # Format: "Assembly/TypeName::MethodName:Signature"
    type_match = re.match(r"[^/]+/([^:]+)::", method_subject_id)
    if type_match:
        type_name = type_match.group(1)
        # Use top-level namespace segment as family key
        parts = type_name.split(".")
        if len(parts) >= 2:
            return parts[0]  # e.g., "System", "Microsoft"
        return type_name
    return "_other_"


def generate_report(il_scan_path: str, output_path: str, contracts_dir: str = None,
                    trace_enabled: bool = False):
    """Generate full gap report and save to output_path."""
    if trace_enabled:
        try:
            from testing.trace import trace as _trace
        except ImportError:
            trace_enabled = False

    t0 = time.time()
    il_scan = load_il_scan(il_scan_path)

    overall = analyze_opcode_gap(il_scan)
    families = analyze_family_gaps(il_scan, contracts_dir)

    # Convert family sets to lists for JSON serialization
    family_report = {}
    for fid, info in families.items():
        family_report[fid] = {
            "familyId": info["familyId"],
            "methodCount": info["methodCount"],
            "methodsWithBody": info["methodsWithBody"],
            "uniqueOpcodeCount": info["uniqueOpcodeCount"],
            "coveredOpcodeCount": info["coveredOpcodeCount"],
            "missingOpcodeCount": info["missingOpcodeCount"],
            "missingOpcodes": info["missingOpcodes"],
            "coveragePercentage": info["coveragePercentage"],
        }

    report = {
        "reportType": "il-opcode-coverage-gap",
        "assemblyName": overall["assemblyName"],
        "overall": overall,
        "families": family_report,
    }

    with open(output_path, "w", encoding="utf-8") as f:
        json.dump(report, f, indent=2, ensure_ascii=False)

    print(f"Gap report written to: {output_path}")
    print(f"  Assembly: {overall['assemblyName']}")
    print(f"  Unique opcodes scanned: {overall['scannedUniqueOpcodes']}")
    print(f"  Covered: {overall['coveredCount']}")
    print(f"  Missing: {overall['missingCount']} -> {overall['missingOpcodes']}")
    print(f"  Coverage: {overall['coveragePercentage']}%")
    print(f"  Families analyzed: {len(family_report)}")

    if trace_enabled:
        _trace("il_coverage_analyzer", stage="il-coverage",
               assembly=overall["assemblyName"], missing=overall["missingCount"],
               coverage_pct=overall["coveragePercentage"],
               dur_ms=round((time.time() - t0) * 1000, 1))

    return report


if __name__ == "__main__":
    import argparse
    parser = argparse.ArgumentParser(description="IL Coverage Analyzer")
    parser.add_argument("--il-scan", required=True, help="Path to IL scan JSON output")
    parser.add_argument("--output", required=True, help="Path to write gap report")
    parser.add_argument("--family-contracts", help="Path to family contracts directory")
    parser.add_argument("--trace", action="store_true", help="Enable JSONL trace logging")
    args = parser.parse_args()

    trace_enabled = args.trace
    if trace_enabled:
        try:
            from testing.trace import trace_init
            trace_init(_SCRIPT_DIR.parents[4], stage="il-coverage")
            print("[trace] JSONL trace enabled")
        except ImportError:
            trace_enabled = False

    generate_report(args.il_scan, args.output, args.family_contracts, trace_enabled=trace_enabled)
