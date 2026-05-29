from __future__ import annotations

import json
from pathlib import Path
from typing import Any

try:
    from . import candidate_extractor as candidate_extractor_module
    from . import family_matching as family_matching_module
    from . import family_synthesis as family_synthesis_module
except ImportError:
    import sys

    sys.path.insert(0, str(Path(__file__).resolve().parent))
    import candidate_extractor as candidate_extractor_module
    import family_matching as family_matching_module
    import family_synthesis as family_synthesis_module


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
    assembly_payload = dict(surface_assemblies.get(assembly_name) or {})
    features = candidate_extractor_module.extract_candidate_features(
        assembly_name,
        assembly_payload,
        semantic_family_count=semantic_family_count if assembly_name == "System.Private.CoreLib" else 0,
    )
    return {
        "surfacePublicTypeCount": features["publicTypeCount"],
        "surfacePublicMemberCount": features["publicMemberCount"],
        "semanticFamilyCount": features["semanticFamilyCount"],
        "namespaceBucketCount": len(features["namespaceBuckets"]),
        "surfaceTypeTokenCount": len(features["typeNameTokens"]),
    }


def _demote_family(family: dict[str, Any]) -> dict[str, Any]:
    demoted = dict(family)
    demoted["denominatorStatus"] = "candidate-derived"
    return demoted


def _synthesize_candidate_families(
    assembly_name: str,
    *,
    existing_families: list[dict[str, Any]],
    surface_assemblies: dict[str, dict[str, Any]],
    project_policies: dict[str, str] | None = None,
) -> list[dict[str, Any]]:
    assembly_payload = dict(surface_assemblies.get(assembly_name) or {})
    surface_types = dict(assembly_payload.get("publicTypes") or {})
    synthesized = family_synthesis_module.synthesize_families_for_assembly(assembly_name, surface_types)
    if not synthesized:
        # No new synthesis — reconcile existing family gates against assembly policy.
        policies = dict(project_policies or {})
        next_families: list[dict[str, Any]] = []
        for family in existing_families:
            demoted = _demote_family(family)
            demoted["synthesisStatus"] = "orphan-confirmed"
            gates = dict(demoted.get("verificationGates") or {})
            for gate in ("hotupdate-proof", "benchmark"):
                if gates.get(gate) == "not-required" and policies.get(gate) in ("required", "conditional"):
                    gates[gate] = "pending"
            demoted["verificationGates"] = gates
            next_families.append(demoted)
        return next_families

    matches = family_matching_module.match_family_candidates(synthesized, existing_families)
    existing_by_id = {str(family.get("familyId") or ""): family for family in existing_families}
    synthesized_by_id = {str(family.get("familyId") or ""): family for family in synthesized}

    # Resolve effective gate policy for this assembly.
    # Program manifest defines per-assembly projectPolicies that override hardcoded defaults.
    # Treat any non-"not-required" policy as "pending" (the initial active state).
    policies = dict(project_policies or {})
    def _initial_gate_state(gate: str) -> str:
        policy = policies.get(gate)
        if policy in ("required", "conditional"):
            return "pending"
        return "not-required"

    next_families: list[dict[str, Any]] = []
    for family_id in matches["matchedFamilyIds"]:
        demoted = _demote_family(existing_by_id[family_id])
        demoted["matchedTypes"] = list(synthesized_by_id[family_id].get("matchedTypes") or [])
        demoted["synthesisStatus"] = "matched-existing"
        # Reconcile existing gate states with assembly-level policy —
        # if the policy says a gate is required/conditional but the family
        # has it as "not-required", promote to "pending".
        gates = dict(demoted.get("verificationGates") or {})
        for gate in ("hotupdate-proof", "benchmark"):
            if gates.get(gate) == "not-required" and policies.get(gate) in ("required", "conditional"):
                gates[gate] = "pending"
        demoted["verificationGates"] = gates
        next_families.append(demoted)

    for family_id in matches["newCandidateFamilyIds"]:
        synthesized_family = dict(synthesized_by_id[family_id])
        synthesized_family["denominatorStatus"] = "candidate-derived"
        synthesized_family["closureStatus"] = "in-progress"
        synthesized_family["verificationGates"] = {
            "audit-input-and-ledger": "pending",
            "managed-proof": "pending",
            "native-proof": "pending",
            "hotupdate-proof": _initial_gate_state("hotupdate-proof"),
            "benchmark": _initial_gate_state("benchmark"),
            "codegen-review": "pending",
        }
        synthesized_family["methodCount"] = 0
        synthesized_family["implementationFamilies"] = []
        synthesized_family["sourceGroups"] = []
        synthesized_family["synthesisStatus"] = "new-candidate"
        next_families.append(synthesized_family)

    for family_id in matches["orphanConfirmedFamilyIds"]:
        demoted = _demote_family(existing_by_id[family_id])
        demoted["synthesisStatus"] = "orphan-confirmed"
        # Reconcile existing gate states with assembly-level policy.
        gates = dict(demoted.get("verificationGates") or {})
        for gate in ("hotupdate-proof", "benchmark"):
            if gates.get(gate) == "not-required" and policies.get(gate) in ("required", "conditional"):
                gates[gate] = "pending"
        demoted["verificationGates"] = gates
        next_families.append(demoted)

    return next_families


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

    program_manifest = _read_json(program_manifest_path)
    assemblies_config = {str(a.get("assemblyName")): dict(a) for a in list(program_manifest.get("assemblies") or [])}

    derived_dlls: list[dict[str, Any]] = []
    for dll in list(payload.get("dlls") or []):
        assembly_name = str(dll.get("assemblyName") or "")
        if scope != "all" and assembly_name != scope:
            continue
        next_dll = dict(dll)
        next_dll["denominatorStatus"] = "candidate-derived"
        next_dll["authorityInputs"] = _derive_dll_inputs(assembly_name, surface_assemblies, semantic_family_count)
        assembly_config = assemblies_config.get(assembly_name, {})
        project_policies = dict(assembly_config.get("projectPolicies") or {})
        next_dll["families"] = _synthesize_candidate_families(
            assembly_name,
            existing_families=list(dll.get("families") or []),
            surface_assemblies=surface_assemblies,
            project_policies=project_policies,
        )
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
