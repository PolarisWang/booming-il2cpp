from __future__ import annotations

import json
from pathlib import Path
from typing import Any


def _string(value: Any) -> str:
    return str(value or "").strip()


def _list(values: Any) -> list[Any]:
    if isinstance(values, list):
        return list(values)
    if isinstance(values, tuple):
        return list(values)
    if values in (None, ""):
        return []
    return [values]


def _family_slug(family_id: str) -> str:
    parts = [part for part in _string(family_id).split("/") if part]
    if len(parts) >= 4:
        return "-".join(parts[2:])
    return _string(family_id).replace("/", "-")


def _read_json(path: Path) -> dict[str, Any] | None:
    if not path.is_file():
        return None
    try:
        payload = json.loads(path.read_text(encoding="utf-8"))
    except (OSError, json.JSONDecodeError):
        return None
    return payload if isinstance(payload, dict) else None


def _family_truth_root(repo_root: Path, assembly_name: str, family_id: str) -> Path:
    return repo_root / "testing" / "foundation-dll" / assembly_name / _family_slug(family_id)


def _default_method_contracts(family: dict[str, Any], method_subject_ids: list[str]) -> list[dict[str, Any]]:
    gates = dict(family.get("verificationGates") or {})
    functional_required = _string(gates.get("native-proof")) != "not-required"
    benchmark_required = _string(gates.get("benchmark")) != "not-required"
    hotupdate_required = _string(gates.get("hotupdate-proof")) != "not-required"
    route_codes = ["managed", "native"] if functional_required else []
    benchmark_profiles = ["default"] if benchmark_required else []
    hotupdate_profiles = ["host-to-patch"] if hotupdate_required else []
    return [
        {
            "methodSubjectId": subject_id,
            "functionalRequired": functional_required,
            "benchmarkRequired": benchmark_required,
            "hotupdateFunctionalRequired": hotupdate_required,
            "hotupdateBenchmarkRequired": False,
            "routeCodes": route_codes,
            "benchmarkProfiles": benchmark_profiles,
            "hotupdateProfiles": hotupdate_profiles,
            "reviewRequired": True,
        }
        for subject_id in method_subject_ids
    ]


def load_method_contracts_for_family(
    repo_root: Path,
    *,
    assembly_name: str,
    family: dict[str, Any],
) -> list[dict[str, Any]]:
    family_id = _string(family.get("familyId"))
    payload = _read_json(_family_truth_root(repo_root, assembly_name, family_id) / "method-capability-contracts.json")
    if payload is not None:
        methods = [dict(item) for item in list(payload.get("methods") or [])]
        if methods:
            return methods
    method_subject_ids = [str(item) for item in list(family.get("methodSubjectIds") or []) if str(item)]
    return _default_method_contracts(family, method_subject_ids)


def build_truth_contracts_snapshot(
    *,
    assembly_name: str,
    owner_subject_id: str,
    families: list[dict[str, Any]],
    method_contracts_by_family: dict[str, list[dict[str, Any]]],
) -> dict[str, Any]:
    capability_families = [_string(item.get("familyId")) for item in families if _string(item.get("familyId"))]
    method_items: list[dict[str, Any]] = []
    family_contracts: list[dict[str, Any]] = []

    for family in families:
        family_id = _string(family.get("familyId"))
        method_contracts = [dict(item) for item in method_contracts_by_family.get(family_id, [])]
        method_subject_ids = [_string(item.get("methodSubjectId")) for item in method_contracts if _string(item.get("methodSubjectId"))]
        method_items.extend(
            {
                "assemblyName": assembly_name,
                "ownerSubjectId": owner_subject_id,
                "familyId": family_id,
                "methodSubjectId": _string(item.get("methodSubjectId")),
                "functionalRequired": bool(item.get("functionalRequired")),
                "benchmarkRequired": bool(item.get("benchmarkRequired")),
                "hotupdateFunctionalRequired": bool(item.get("hotupdateFunctionalRequired")),
                "hotupdateBenchmarkRequired": bool(item.get("hotupdateBenchmarkRequired")),
                "routeCodes": [str(route) for route in _list(item.get("routeCodes")) if str(route)],
                "benchmarkProfiles": [str(profile) for profile in _list(item.get("benchmarkProfiles")) if str(profile)],
                "hotupdateProfiles": [str(profile) for profile in _list(item.get("hotupdateProfiles")) if str(profile)],
                "reviewRequired": bool(item.get("reviewRequired")),
            }
            for item in method_contracts
        )
        family_contracts.append(
            {
                "assemblyName": assembly_name,
                "ownerSubjectId": owner_subject_id,
                "familyId": family_id,
                "displayName": _string(family.get("displayName")),
                "methodSubjectIds": method_subject_ids,
                "methodSubjectCount": len(method_subject_ids),
                "functionalObligation": {
                    "required": any(bool(item.get("functionalRequired")) for item in method_contracts),
                },
                "performanceObligation": {
                    "required": any(bool(item.get("benchmarkRequired")) for item in method_contracts),
                },
                "hotupdateFunctionalObligation": {
                    "required": any(bool(item.get("hotupdateFunctionalRequired")) for item in method_contracts),
                },
                "hotupdatePerformanceObligation": {
                    "required": any(bool(item.get("hotupdateBenchmarkRequired")) for item in method_contracts),
                },
                "reviewBundleRequired": any(bool(item.get("reviewRequired")) for item in method_contracts),
            }
        )

    return {
        "schemaVersion": 1,
        "dllCapabilityManifest": {
            "assemblyName": assembly_name,
            "ownerSubjectId": owner_subject_id,
            "capabilityFamilies": capability_families,
            "capabilityFamilyCount": len(capability_families),
            "methodUniverseCount": len(method_items),
        },
        "capabilityFamilyVerificationContracts": family_contracts,
        "methodCapabilityContracts": method_items,
    }


def load_truth_contracts_snapshot(
    repo_root: Path,
    *,
    assembly_name: str,
    owner_subject_id: str,
    families: list[dict[str, Any]],
) -> dict[str, Any]:
    return build_truth_contracts_snapshot(
        assembly_name=assembly_name,
        owner_subject_id=owner_subject_id,
        families=families,
        method_contracts_by_family={
            _string(family.get("familyId")): load_method_contracts_for_family(
                repo_root,
                assembly_name=assembly_name,
                family=family,
            )
            for family in families
        },
    )
