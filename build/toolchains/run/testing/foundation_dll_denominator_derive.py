from __future__ import annotations

import json
from pathlib import Path
from typing import Any


CANDIDATE_LEDGER_PATH = ("verification", "projections", "foundation-dll-audit", "capability-family-ledger.candidate.json")
LEDGER_PATH = ("verification", "projections", "foundation-dll-audit", "capability-family-ledger.json")
PROGRAM_MANIFEST_PATH = ("verification", "catalog", "programs", "foundation-dll-translation-audit.program.json")
SURFACE_MANIFEST_PATH = (
    "docs",
    "dev",
    "completed",
    "20260418-03-phase-1-contract-facade-and-shim-certification-lane",
    "public-surface-manifest-v1-01.json",
)
SEMANTIC_FAMILY_PLAN_PATH = (
    "docs",
    "dev",
    "completed",
    "20260418-05-phase-3-system-private-corelib-semantic-family-nativeization",
    "semantic-family-plan-v1-01.json",
)


def _read_json(path: Path) -> dict[str, Any]:
    return json.loads(path.read_text(encoding="utf-8"))


def _relative(repo_root: Path, path: Path) -> str:
    return path.resolve().relative_to(repo_root.resolve()).as_posix()


def _surface_assembly_index(surface_manifest: dict[str, Any]) -> dict[str, dict[str, Any]]:
    target_frameworks = dict(surface_manifest.get("targetFrameworks") or {})
    net10 = dict(target_frameworks.get("net10.0") or {})
    assemblies = dict(net10.get("assemblies") or {})
    return assemblies


def _semantic_family_count(semantic_family_plan: dict[str, Any]) -> int:
    target_frameworks = dict(semantic_family_plan.get("targetFrameworks") or {})
    net10 = dict(target_frameworks.get("net10.0") or {})
    return int(net10.get("familyCount") or 0)


def _build_authority_inputs(
    repo_root: Path,
    *,
    program_manifest_path: Path,
    surface_manifest_path: Path,
    semantic_family_plan_path: Path,
) -> dict[str, Any]:
    return {
        "programManifest": _relative(repo_root, program_manifest_path),
        "surfaceManifest": _relative(repo_root, surface_manifest_path),
        "semanticFamilyPlan": _relative(repo_root, semantic_family_plan_path),
    }


def _derive_dll_inputs(
    assembly_name: str,
    surface_assemblies: dict[str, dict[str, Any]],
    semantic_family_count: int,
) -> dict[str, Any]:
    payload: dict[str, Any] = {}
    if assembly_name in surface_assemblies:
        assembly_payload = dict(surface_assemblies[assembly_name] or {})
        payload["surfacePublicTypeCount"] = int(assembly_payload.get("publicTypeDefinitionCount") or 0)
        payload["surfacePublicMemberCount"] = int(assembly_payload.get("publicMemberCount") or 0)
    if assembly_name == "System.Private.CoreLib":
        payload["semanticFamilyCount"] = semantic_family_count
    return payload


def _demote_family(family: dict[str, Any]) -> dict[str, Any]:
    demoted = dict(family)
    demoted["denominatorStatus"] = "candidate-derived"
    return demoted


def _build_candidate_payload(
    repo_root: Path,
    *,
    formal_ledger: dict[str, Any],
    program_manifest_path: Path,
    surface_manifest_path: Path,
    semantic_family_plan_path: Path,
    scope: str,
) -> dict[str, Any]:
    authority_inputs = _build_authority_inputs(
        repo_root,
        program_manifest_path=program_manifest_path,
        surface_manifest_path=surface_manifest_path,
        semantic_family_plan_path=semantic_family_plan_path,
    )
    surface_assemblies = _surface_assembly_index(_read_json(surface_manifest_path))
    semantic_family_count = _semantic_family_count(_read_json(semantic_family_plan_path))

    payload = dict(formal_ledger)
    payload["snapshotId"] = "snap-20260426-candidate-refresh-v1"
    payload["denominatorStatus"] = "candidate-derived"
    payload["authorityInputs"] = authority_inputs

    derived_dlls: list[dict[str, Any]] = []
    for dll in list(payload.get("dlls") or []):
        assembly_name = str(dll.get("assemblyName") or "")
        if scope != "all" and assembly_name != scope:
            continue
        next_dll = dict(dll)
        next_dll["denominatorStatus"] = "candidate-derived"
        next_dll["authorityInputs"] = _derive_dll_inputs(assembly_name, surface_assemblies, semantic_family_count)
        next_dll["families"] = [_demote_family(family) for family in list(dll.get("families") or [])]
        derived_dlls.append(next_dll)

    if scope == "all":
        payload["dlls"] = derived_dlls
    else:
        existing = [dict(dll) for dll in list(payload.get("dlls") or []) if str(dll.get("assemblyName") or "") != scope]
        payload["dlls"] = existing + derived_dlls
    payload["totalFamilies"] = sum(len(dll.get("families", [])) for dll in payload.get("dlls", []))
    return payload


def generate_candidate_ledger(repo_root: Path, *, scope: str = "all") -> dict[str, Any]:
    formal_ledger_path = repo_root.joinpath(*LEDGER_PATH)
    program_manifest_path = repo_root.joinpath(*PROGRAM_MANIFEST_PATH)
    surface_manifest_path = repo_root.joinpath(*SURFACE_MANIFEST_PATH)
    semantic_family_plan_path = repo_root.joinpath(*SEMANTIC_FAMILY_PLAN_PATH)

    payload = _build_candidate_payload(
        repo_root,
        formal_ledger=_read_json(formal_ledger_path),
        program_manifest_path=program_manifest_path,
        surface_manifest_path=surface_manifest_path,
        semantic_family_plan_path=semantic_family_plan_path,
        scope=scope,
    )

    candidate_path = repo_root.joinpath(*CANDIDATE_LEDGER_PATH)
    candidate_path.parent.mkdir(parents=True, exist_ok=True)
    candidate_path.write_text(json.dumps(payload, indent=2, ensure_ascii=False) + "\n", encoding="utf-8")

    family_count = sum(len(dll.get("families", [])) for dll in payload.get("dlls", []))
    return {
        "candidateLedgerPath": candidate_path.relative_to(repo_root).as_posix(),
        "familyCount": family_count,
        "dllCount": len(payload.get("dlls", [])),
        "scope": scope,
        "authorityInputs": dict(payload.get("authorityInputs") or {}),
    }
