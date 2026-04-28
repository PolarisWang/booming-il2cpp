from __future__ import annotations

import json
from pathlib import Path
from typing import Any


def _string(value: Any) -> str:
    return str(value or "").strip()


def _slug_from_family_id(family_id: str) -> str:
    value = _string(family_id)
    if not value:
        return ""
    parts = value.split("/")
    if len(parts) >= 4:
        return "-".join(part.replace("_", "-") for part in parts[2:])
    return parts[-1].replace("_", "-")


def _write_json(path: Path, payload: dict[str, Any]) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(payload, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")


def generate_truth_skeleton(
    repo_root: Path,
    *,
    assembly_name: str,
    owner_subject_id: str,
    family: dict[str, Any],
    method_subject_ids: list[str],
) -> dict[str, Any]:
    family_id = _string(family.get("familyId"))
    family_slug = _slug_from_family_id(family_id)
    output_root = repo_root / "verification" / "foundation-dll" / assembly_name / family_slug

    dll_manifest = {
        "schemaVersion": 1,
        "assemblyName": assembly_name,
        "ownerSubjectId": owner_subject_id,
        "capabilityFamilies": [family_id],
        "capabilityFamilyCount": 1,
        "methodUniverseCount": len(method_subject_ids),
    }
    family_contract = {
        "schemaVersion": 1,
        "assemblyName": assembly_name,
        "ownerSubjectId": owner_subject_id,
        "familyId": family_id,
        "displayName": _string(family.get("displayName")),
        "methodSubjectIds": list(method_subject_ids),
        "methodSubjectCount": len(method_subject_ids),
        "functionalObligation": {"required": True},
        "performanceObligation": {"required": False},
        "hotupdateFunctionalObligation": {"required": False},
        "hotupdatePerformanceObligation": {"required": False},
        "reviewBundleRequired": True,
    }
    method_contracts = {
        "schemaVersion": 1,
        "assemblyName": assembly_name,
        "ownerSubjectId": owner_subject_id,
        "familyId": family_id,
        "methods": [
            {
                "methodSubjectId": subject_id,
                "functionalRequired": True,
                "benchmarkRequired": False,
                "hotupdateFunctionalRequired": False,
                "hotupdateBenchmarkRequired": False,
                "routeCodes": ["managed", "native"],
                "benchmarkProfiles": [],
                "hotupdateProfiles": [],
                "reviewRequired": True,
            }
            for subject_id in method_subject_ids
        ],
    }

    manifest_path = output_root / "dll-capability-manifest.json"
    family_contract_path = output_root / "capability-family-contract.json"
    method_contracts_path = output_root / "method-capability-contracts.json"

    _write_json(manifest_path, dll_manifest)
    _write_json(family_contract_path, family_contract)
    _write_json(method_contracts_path, method_contracts)

    return {
        "assemblyName": assembly_name,
        "familyId": family_id,
        "artifacts": [
            manifest_path.relative_to(repo_root).as_posix(),
            family_contract_path.relative_to(repo_root).as_posix(),
            method_contracts_path.relative_to(repo_root).as_posix(),
        ],
    }


def generate_truth_skeleton_for_dll(
    repo_root: Path,
    *,
    assembly_name: str,
    owner_subject_id: str,
    families: list[dict[str, Any]],
    method_subject_ids_by_family: dict[str, list[str]],
) -> dict[str, Any]:
    artifacts: list[str] = []
    for family in families:
        family_id = _string(family.get("familyId"))
        generation = generate_truth_skeleton(
            repo_root,
            assembly_name=assembly_name,
            owner_subject_id=owner_subject_id,
            family=family,
            method_subject_ids=list(method_subject_ids_by_family.get(family_id, [])),
        )
        artifacts.extend(generation["artifacts"])
    return {
        "assemblyName": assembly_name,
        "familyCount": len(families),
        "artifacts": artifacts,
    }
