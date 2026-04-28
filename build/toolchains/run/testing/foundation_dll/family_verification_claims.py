from __future__ import annotations

import json
import re
from pathlib import Path
from typing import Any


def _string(value: Any) -> str:
    return str(value or "").strip()


def _normalized(text: str) -> str:
    return str(text or "").replace("\\", "/")


def _extract_run_id(path: str) -> str:
    match = re.search(r"runs/([^/]+)/", path.replace("\\", "/"))
    if match is None:
        return ""
    return _string(match.group(1))


def _try_read_json(path: Path) -> dict[str, Any] | None:
    if not path.is_file():
        return None
    try:
        payload = json.loads(path.read_text(encoding="utf-8"))
    except (OSError, json.JSONDecodeError):
        return None
    return payload if isinstance(payload, dict) else None


def _native_reference_plan_candidates(repo_root: Path, artifact_path: str) -> list[Path]:
    normalized = _normalized(artifact_path)
    path = repo_root / normalized
    candidates: list[Path] = []
    if normalized.endswith("native-reference.plan.json"):
        candidates.append(path)
    if normalized.endswith("native-reference.runtime-skeleton.coverage.json"):
        candidates.append(path.parents[2] / "native-reference.plan.json")
    if normalized.endswith("/generated/runtime/") or normalized.endswith("\\generated\\runtime\\"):
        candidates.append(path.parent.parent / "native-reference.plan.json")
    return candidates


def _load_latest_method_universe(repo_root: Path, projects: list[dict[str, Any]]) -> tuple[list[str], list[str]]:
    run_candidates: dict[str, list[Path]] = {}
    for project in projects:
        if _string(project.get("projectCode")) != "native-proof":
            continue
        for artifact in list(project.get("artifacts") or []):
            path = _string(artifact.get("path"))
            if not path:
                continue
            run_id = _extract_run_id(path)
            if not run_id:
                continue
            for candidate in _native_reference_plan_candidates(repo_root, path):
                run_candidates.setdefault(run_id, []).append(candidate)

    for run_id in sorted(run_candidates.keys(), reverse=True):
        for candidate in run_candidates[run_id]:
            payload = _try_read_json(candidate)
            if payload is None:
                continue
            method_subject_ids = [str(item) for item in list(payload.get("translationUnitMethodSubjectIds") or []) if str(item)]
            if method_subject_ids:
                return method_subject_ids, [candidate.relative_to(repo_root).as_posix()]
    return [], []


def _family_slug(family_id: str) -> str:
    parts = [part for part in _string(family_id).split("/") if part]
    if len(parts) >= 4:
        return "-".join(parts[2:])
    return _string(family_id).replace("/", "-")


def _load_truth_method_subject_ids(
    repo_root: Path,
    *,
    assembly_name: str,
    family_id: str,
) -> tuple[list[str], list[str]]:
    truth_path = (
        repo_root
        / "verification"
        / "foundation-dll"
        / assembly_name
        / _family_slug(family_id)
        / "method-capability-contracts.json"
    )
    payload = _try_read_json(truth_path)
    if payload is None:
        return [], []
    methods = [dict(item) for item in list(payload.get("methods") or [])]
    subject_ids = [_string(item.get("methodSubjectId")) for item in methods if _string(item.get("methodSubjectId"))]
    if not subject_ids:
        return [], []
    return subject_ids, [truth_path.relative_to(repo_root).as_posix()]


def _append_gate_claim(
    claims: list[dict[str, Any]],
    *,
    family: dict[str, Any],
    owner_subject_id: str,
    gate_code: str,
    denominator: int,
    method_subject_ids: list[str],
    derivation_rule: str,
    authority_refs: list[str],
) -> None:
    family_id = _string(family.get("familyId"))
    claims.append(
        {
            "claimId": f"{family_id}::{gate_code}",
            "familyId": family_id,
            "gateCode": gate_code,
            "required": _string(dict(family.get("verificationGates") or {}).get(gate_code)) != "not-required",
            "denominator": denominator,
            "methodSubjectIds": method_subject_ids,
            "ownerSubjectId": owner_subject_id,
            "scope": _string(family.get("displayName")),
            "derivationRule": derivation_rule,
            "authorityRefs": authority_refs,
        }
    )


def build_family_verification_claims_snapshot(
    repo_root: Path,
    *,
    assembly_name: str,
    owner_subject_id: str,
    families: list[dict[str, Any]],
    projects: list[dict[str, Any]],
) -> dict[str, Any]:
    method_universe, source_paths = _load_latest_method_universe(repo_root, projects)
    claims: list[dict[str, Any]] = []
    for family in families:
        family_id = _string(family.get("familyId"))
        truth_method_subject_ids, truth_refs = _load_truth_method_subject_ids(
            repo_root,
            assembly_name=assembly_name,
            family_id=family_id,
        )
        inline_method_subject_ids = [str(item) for item in list(family.get("methodSubjectIds") or []) if str(item)]
        method_subject_ids = truth_method_subject_ids or inline_method_subject_ids
        derivation_rule = (
            "truth-contract:method-capability-contracts"
            if truth_method_subject_ids
            else "inline-family:methodSubjectIds"
            if inline_method_subject_ids
            else ""
        )
        denominator = len(method_subject_ids) if method_subject_ids else int(family.get("methodCount") or 0)
        _append_gate_claim(
            claims,
            family=family,
            owner_subject_id=owner_subject_id,
            gate_code="native-proof",
            denominator=denominator,
            method_subject_ids=method_subject_ids,
            derivation_rule=derivation_rule,
            authority_refs=truth_refs or source_paths,
        )
        _append_gate_claim(
            claims,
            family=family,
            owner_subject_id=owner_subject_id,
            gate_code="managed-proof",
            denominator=1,
            method_subject_ids=[],
            derivation_rule="gate-presence:managed-proof",
            authority_refs=[],
        )
        _append_gate_claim(
            claims,
            family=family,
            owner_subject_id=owner_subject_id,
            gate_code="hotupdate-proof",
            denominator=1,
            method_subject_ids=[],
            derivation_rule="gate-presence:hotupdate-proof",
            authority_refs=[],
        )
        _append_gate_claim(
            claims,
            family=family,
            owner_subject_id=owner_subject_id,
            gate_code="benchmark",
            denominator=1,
            method_subject_ids=[],
            derivation_rule="gate-presence:benchmark",
            authority_refs=[],
        )
        _append_gate_claim(
            claims,
            family=family,
            owner_subject_id=owner_subject_id,
            gate_code="test-code",
            denominator=int(family.get("methodCount") or 0),
            method_subject_ids=[],
            derivation_rule="ledger:testCode",
            authority_refs=[],
        )
    return {
        "schemaVersion": 1,
        "assemblyName": assembly_name,
        "ownerSubjectId": owner_subject_id,
        "methodUniverseArtifactPaths": source_paths,
        "claims": claims,
    }
