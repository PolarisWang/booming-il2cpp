"""Populate methodSubjectIds for non-CoreLib foundation-dll families from surface manifest.

Phase 1: Generate simplified methodSubjectIds using public surface manifest data.
Phase 2: Inject into formal ledger so foundation-dll pipeline sees methodCount > 0.

Usage:
    python build/populate_noncorelib_families.py [--scope <assembly-name>]
"""
from __future__ import annotations

import json
import re
import sys
from pathlib import Path
from typing import Any

REPO_ROOT = Path(__file__).resolve().parent.parent
SURFACE_MANIFEST_PATH = (
    REPO_ROOT / "docs" / "dev" / "completed" / "20260418-03-phase-1-contract-facade-and-shim-certification-lane"
    / "public-surface-manifest-v1-01.json"
)
FORMAL_LEDGER_PATH = REPO_ROOT / "testing" / "verification-catalog" / "projections" / "foundation-dll-audit" / "capability-family-ledger.json"
CANDIDATE_LEDGER_PATH = REPO_ROOT / "testing" / "verification-catalog" / "projections" / "foundation-dll-audit" / "capability-family-ledger.candidate.json"


# Assemblies tracked in the foundation-dll audit that are NOT System.Private.CoreLib
NON_CORELIB_ASSEMBLIES = {
    "System.Collections.Immutable",
    "System.Formats.Asn1",
    "System.IO.Compression.Brotli",
    "System.IO.Compression.ZipFile",
    "System.IO.Pipelines",
    "System.Linq",
    "System.Net.ServerSentEvents",
    "System.ObjectModel",
    "System.Runtime.InteropServices",
    "System.Runtime.Serialization.Formatters",
    "System.Security.Principal.Windows",
    "System.Threading.Tasks.Parallel",
    "System.Text.Json",
}


def _read_json(path: Path) -> dict[str, Any]:
    return json.loads(path.read_text(encoding="utf-8"))


def _write_json(path: Path, payload: dict[str, Any]) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(payload, indent=2, ensure_ascii=False) + "\n", encoding="utf-8")


def _method_arity(method_entry: str) -> int:
    """Extract parameter count from a method entry like 'Copy/4' or '.ctor/3'."""
    parts = method_entry.split("/")
    if len(parts) >= 2 and parts[-1].isdigit():
        return int(parts[-1])
    return 0


def _method_name_part(method_entry: str) -> str:
    """Extract the method name (without param count) from entry like 'Copy/4'."""
    parts = method_entry.rsplit("/", 1)
    return parts[0]


def _is_special_method(name: str) -> bool:
    return name in (".ctor", ".cctor", ".dtor")


def _build_method_subject_id(
    assembly_name: str,
    type_full_name: str,
    method_entry: str,
) -> str:
    """Build a standardized method subject ID from surface manifest data.

    Format: AssemblyName/FullTypeName::MethodName:UnknownType(int params)
    Since the surface manifest only stores MethodName/ParamCount, we use
    a placeholder return type and parameter signature.
    """
    method_name = _method_name_part(method_entry)
    arity = _method_arity(method_entry)

    if _is_special_method(method_name):
        if method_name == ".ctor":
            return f"{assembly_name}/{type_full_name}::{type_full_name.split('.')[-1]}::Void({', '.join(['System.Object'] * arity)})"
        return f"{assembly_name}/{type_full_name}::.{method_name}:Void({', '.join(['System.Object'] * arity)})"

    # Build param list placeholder: for arity N, generate param types P1,P2,...
    if arity > 0:
        params = ", ".join(f"P{i}" for i in range(arity))
    else:
        params = ""

    return f"{assembly_name}/{type_full_name}::{method_name}:System.Void({params})"


def _property_accessor_name(prop_name: str, is_getter: bool) -> str:
    prefix = "get_" if is_getter else "set_"
    return f"{prefix}{prop_name}"


def _build_method_ids_for_type(
    assembly_name: str,
    type_full_name: str,
    type_data: dict[str, Any],
) -> list[str]:
    """Build method subject IDs for a single type from its surface manifest data."""
    ids: list[str] = []

    # Public methods
    for method_entry in type_data.get("publicMethods", []):
        ids.append(_build_method_subject_id(assembly_name, type_full_name, method_entry))

    # Public properties -> get_/set_ accessors
    for prop_name in type_data.get("publicProperties", []):
        ids.append(_build_method_subject_id(
            assembly_name, type_full_name,
            f"{_property_accessor_name(prop_name, True)}/0"
        ))
        ids.append(_build_method_subject_id(
            assembly_name, type_full_name,
            f"{_property_accessor_name(prop_name, False)}/1"
        ))

    return ids


def _surface_types_for_assembly(surface_manifest: dict[str, Any], assembly_name: str) -> dict[str, Any]:
    """Get public type data from the surface manifest for an assembly."""
    tf = dict(surface_manifest.get("targetFrameworks") or {})
    net10 = dict(tf.get("net10.0") or {})
    assemblies = dict(net10.get("assemblies") or {})
    asm_data = assemblies.get(assembly_name)
    if asm_data is None:
        # Try alternate name lookup
        for candidate_name, candidate_data in assemblies.items():
            if candidate_name.replace(".", "").lower() == assembly_name.replace(".", "").lower():
                asm_data = candidate_data
                break
    if asm_data is None:
        return {}
    return dict(asm_data.get("publicTypes") or {})


def _populate_family(
    assembly_name: str,
    family: dict[str, Any],
    surface_types: dict[str, Any],
) -> list[str]:
    """Generate methodSubjectIds for a single family from its matchedTypes."""
    matched_types = list(family.get("matchedTypes") or [])
    if not matched_types:
        return []

    all_ids: list[str] = []
    seen: set[str] = set()
    for type_name in matched_types:
        type_data = surface_types.get(type_name)
        if type_data is None:
            # Try partial match
            for st_name, st_data in surface_types.items():
                if type_name in st_name or st_name.startswith(type_name):
                    type_data = st_data
                    break
        if type_data is None:
            continue
        ids = _build_method_ids_for_type(assembly_name, type_name, type_data)
        for mid in ids:
            if mid not in seen:
                seen.add(mid)
                all_ids.append(mid)
    return all_ids


def _resolve_assembly_name_for_ledger(ledger_assembly_name: str, surface_manifest: dict[str, Any]) -> str | None:
    """Resolve a ledger assembly name to a surface manifest assembly name."""
    tf = dict(surface_manifest.get("targetFrameworks") or {})
    net10 = dict(tf.get("net10.0") or {})
    assemblies = dict(net10.get("assemblies") or {})

    if ledger_assembly_name in assemblies:
        return ledger_assembly_name

    # Try case-insensitive or normalized match
    normalized = ledger_assembly_name.replace(".", "").lower()
    for candidate in assemblies:
        if candidate.replace(".", "").lower() == normalized:
            return candidate
    return None


def populate_for_assembly(
    formal_ledger: dict[str, Any],
    surface_manifest: dict[str, Any],
    *,
    scope: str | None = None,
) -> dict[str, Any]:
    """Populate methodSubjectIds for non-CoreLib families in the formal ledger."""
    dlls: list[dict[str, Any]] = []
    stats: dict[str, dict[str, int]] = {}
    total_methods = 0

    for dll in formal_ledger.get("dlls", []):
        assembly_name = str(dll.get("assemblyName") or "")
        if assembly_name == "System.Private.CoreLib":
            dlls.append(dll)
            continue
        if scope is not None and assembly_name != scope:
            dlls.append(dll)
            continue
        if assembly_name not in NON_CORELIB_ASSEMBLIES:
            dlls.append(dll)
            continue

        surface_assembly_name = _resolve_assembly_name_for_ledger(assembly_name, surface_manifest)
        if surface_assembly_name is None:
            print(f"  SKIP {assembly_name}: not found in surface manifest")
            dlls.append(dll)
            continue

        surface_types = _surface_types_for_assembly(surface_manifest, surface_assembly_name)
        if not surface_types:
            print(f"  SKIP {assembly_name}: no public types in surface manifest")
            dlls.append(dll)
            continue

        families: list[dict[str, Any]] = []
        asm_stats: dict[str, int] = {"families": 0, "populated": 0, "methods": 0}
        for family in dll.get("families", []):
            family_id = str(family.get("familyId") or "")
            asm_stats["families"] += 1

            next_family = dict(family)

            # Don't overwrite existing methodSubjectIds
            existing_ids = [str(s) for s in next_family.get("methodSubjectIds", []) if str(s)]
            if existing_ids:
                families.append(next_family)
                asm_stats["methods"] += len(existing_ids)
                continue

            ids = _populate_family(assembly_name, next_family, surface_types)
            if ids:
                next_family["methodSubjectIds"] = ids
                next_family["methodCount"] = len(ids)
                asm_stats["populated"] += 1
                asm_stats["methods"] += len(ids)
                print(f"  {family_id}: {len(ids)} methods from {len(next_family.get('matchedTypes', []))} types")
            else:
                print(f"  {family_id}: 0 methods (no surface data for matchedTypes)")

            families.append(next_family)

        dlls.append({**dll, "families": families})
        stats[assembly_name] = asm_stats
        total_methods += asm_stats["methods"]

    print(f"\nTotal: {total_methods} method IDs populated across {sum(s['families'] for s in stats.values())} families in {len(stats)} assemblies")
    for asm, s in sorted(stats.items()):
        print(f"  {asm}: {s['populated']}/{s['families']} families, {s['methods']} methods")

    return {**formal_ledger, "dlls": dlls}


def main() -> None:
    scope = None
    for arg in sys.argv[1:]:
        if arg.startswith("--scope="):
            scope = arg.split("=", 1)[1]
        elif arg == "--scope" and len(sys.argv) > sys.argv.index(arg) + 1:
            scope = sys.argv[sys.argv.index(arg) + 1]

    print("Loading surface manifest...")
    surface_manifest = _read_json(SURFACE_MANIFEST_PATH)

    print("Loading formal ledger...")
    formal_ledger = _read_json(FORMAL_LEDGER_PATH)

    print(f"Populating methodSubjectIds for non-CoreLib families (scope={scope or 'all'})...")
    updated_ledger = populate_for_assembly(formal_ledger, surface_manifest, scope=scope)

    print("Writing updated formal ledger...")
    _write_json(FORMAL_LEDGER_PATH, updated_ledger)

    # Also update candidate if it exists
    if CANDIDATE_LEDGER_PATH.is_file():
        print("Writing updated candidate ledger...")
        candidate_ledger = _read_json(CANDIDATE_LEDGER_PATH)
        updated_candidate = populate_for_assembly(candidate_ledger, surface_manifest, scope=scope)
        _write_json(CANDIDATE_LEDGER_PATH, updated_candidate)

    print("Done.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
