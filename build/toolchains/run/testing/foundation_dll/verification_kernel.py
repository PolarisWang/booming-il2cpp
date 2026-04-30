from __future__ import annotations

import json
from dataclasses import asdict, dataclass
from pathlib import Path
from typing import Any


def _string(value: Any) -> str:
    return str(value or "").strip()


@dataclass
class VerificationFact:
    factId: str
    familyId: str
    gateCode: str
    runId: str
    artifactPath: str
    artifactKind: str
    linkTargetType: str
    sourceKind: str
    producedAtUtc: str = ""

    def to_dict(self) -> dict[str, Any]:
        return asdict(self)


@dataclass
class VerificationClaim:
    claimId: str
    familyId: str
    gateCode: str
    required: bool
    denominator: int
    methodSubjectIds: list[str]
    ownerSubjectId: str
    scope: str = ""

    def to_dict(self) -> dict[str, Any]:
        return asdict(self)


@dataclass
class VerificationEvaluation:
    status: str
    numerator: int
    denominator: int
    progressPercent: float
    reason: str
    evidence: list[dict[str, Any]]
    runs: list[dict[str, Any]]
    methodDetails: list[dict[str, Any]]

    def to_dict(self) -> dict[str, Any]:
        return asdict(self)


def _try_read_json(path: Path) -> dict[str, Any] | None:
    if not path.is_file():
        return None
    try:
        payload = json.loads(path.read_text(encoding="utf-8"))
    except (OSError, json.JSONDecodeError):
        return None
    return payload if isinstance(payload, dict) else None


def _coverage_json_for_artifact_path(repo_root: Path, artifact_path: str) -> dict[str, Any] | None:
    if not artifact_path.endswith(".coverage.json"):
        return None
    return _try_read_json(repo_root / artifact_path)


def _extract_run_id(path: str) -> str:
    parts = str(path or "").replace("\\", "/").split("/")
    if "runs" not in parts:
        return ""
    index = parts.index("runs")
    return _string(parts[index + 1]) if index + 1 < len(parts) else ""


def build_native_proof_facts(projects: list[dict[str, Any]], *, family_id: str) -> list[VerificationFact]:
    facts: list[VerificationFact] = []
    for project in projects:
        if _string(project.get("projectCode")) != "native-proof":
            continue
        for artifact in list(project.get("artifacts") or []):
            path = _string(artifact.get("path"))
            if not path:
                continue
            fact = VerificationFact(
                factId=f"{family_id}::native-proof::{path.replace('\\', '/')}",
                familyId=family_id,
                gateCode="native-proof",
                runId=_extract_run_id(path),
                artifactPath=path,
                artifactKind=_string(artifact.get("artifactKind")),
                linkTargetType=_string(artifact.get("linkTargetType")),
                sourceKind="artifact",
            )
            facts.append(fact)
    return facts


def evaluate_native_proof(
    repo_root: Path,
    family: dict[str, Any],
    claim: dict[str, Any],
    *,
    projects: list[dict[str, Any]],
) -> VerificationEvaluation:
    claim_payload = VerificationClaim(
        claimId=_string(claim.get("claimId")),
        familyId=_string(claim.get("familyId")),
        gateCode=_string(claim.get("gateCode")),
        required=bool(claim.get("required")),
        denominator=int(claim.get("denominator") or 0),
        methodSubjectIds=[str(item) for item in list(claim.get("methodSubjectIds") or []) if str(item)],
        ownerSubjectId=_string(claim.get("ownerSubjectId")),
        scope=_string(claim.get("scope")),
    )
    facts = build_native_proof_facts(projects, family_id=claim_payload.familyId)
    status = _string(dict(family.get("verificationGates") or {}).get("native-proof")) or "pending"
    evidence = [
        {
            "label": Path(fact.artifactPath).name or fact.artifactPath,
            "path": fact.artifactPath,
            "artifactKind": fact.artifactKind,
            "linkTargetType": fact.linkTargetType,
        }
        for fact in facts
    ]
    runs_by_id: dict[str, dict[str, Any]] = {}
    coverage_by_run_id: dict[str, dict[str, Any]] = {}
    for fact in facts:
        if fact.runId:
            bucket = runs_by_id.setdefault(
                fact.runId,
                {
                    "runId": fact.runId,
                    "status": status,
                    "evidence": [],
                },
            )
            bucket["evidence"].append(
                {
                    "label": Path(fact.artifactPath).name or fact.artifactPath,
                    "path": fact.artifactPath,
                    "artifactKind": fact.artifactKind,
                    "linkTargetType": fact.linkTargetType,
                }
            )
            coverage_payload = _coverage_json_for_artifact_path(repo_root, fact.artifactPath)
            if coverage_payload is not None:
                coverage_by_run_id[fact.runId] = coverage_payload

    numerator = 0
    denominator = claim_payload.denominator
    reason = ""
    method_details: list[dict[str, Any]] = []
    if denominator > 0 and status not in {"pending", "blocked", "not-required", ""}:
        latest_run_id = max(coverage_by_run_id) if coverage_by_run_id else ""
        coverage_payload = coverage_by_run_id.get(latest_run_id)
        if claim_payload.methodSubjectIds and coverage_payload is not None:
            uncovered_ids = {str(item) for item in list(coverage_payload.get("uncoveredMethodSubjectIds") or [])}
            method_details = [
                {
                    "subjectId": subject_id,
                    "covered": subject_id not in uncovered_ids,
                }
                for subject_id in claim_payload.methodSubjectIds
            ]
            numerator = sum(1 for subject_id in claim_payload.methodSubjectIds if subject_id not in uncovered_ids)
        elif claim_payload.methodSubjectIds and status == "passed":
            # Gate says "passed" but no coverage evidence — don't fabricate coverage
            method_details = [
                {
                    "subjectId": subject_id,
                    "covered": False,
                }
                for subject_id in claim_payload.methodSubjectIds
            ]
            numerator = 0
            reason = "no-coverage-evidence"
        elif status == "passed":
            numerator = 0
            reason = "no-coverage-evidence"
        else:
            reason = "coverage-source-missing"
    progress_percent = round((numerator / denominator) * 100, 2) if denominator > 0 else 0.0
    return VerificationEvaluation(
        status=status,
        numerator=numerator,
        denominator=denominator,
        progressPercent=progress_percent,
        reason=reason,
        evidence=evidence,
        runs=list(runs_by_id.values()),
        methodDetails=method_details,
    )


def evaluate_generic_gate(
    family: dict[str, Any],
    *,
    claim: dict[str, Any] | None = None,
    gate_code: str,
    projects: list[dict[str, Any]],
) -> VerificationEvaluation:
    status = _string(dict(family.get("verificationGates") or {}).get(gate_code)) or "pending"
    evidence: list[dict[str, Any]] = []
    runs_by_id: dict[str, dict[str, Any]] = {}
    for project in projects:
        if _string(project.get("projectCode")) != gate_code:
            continue
        for artifact in list(project.get("artifacts") or []):
            path = _string(artifact.get("path"))
            if not path:
                continue
            item = {
                "label": Path(path).name or path,
                "path": path,
                "artifactKind": _string(artifact.get("artifactKind")),
                "linkTargetType": _string(artifact.get("linkTargetType")),
            }
            evidence.append(item)
            run_id = _extract_run_id(path)
            if run_id:
                runs_by_id.setdefault(run_id, {"runId": run_id, "status": status, "evidence": []})["evidence"].append(dict(item))
    claim_method_ids = list((claim or {}).get("methodSubjectIds") or [])
    if claim_method_ids:
        denominator = len(claim_method_ids)
    else:
        denominator = int((claim or {}).get("denominator") or (1 if status != "not-required" else 0))
    numerator = denominator if status == "passed" and claim_method_ids and denominator else 0
    progress_percent = 100.0 if numerator and denominator else 0.0
    return VerificationEvaluation(
        status=status,
        numerator=numerator,
        denominator=denominator,
        progressPercent=progress_percent,
        reason="",
        evidence=evidence,
        runs=list(runs_by_id.values()),
        methodDetails=[],
    )


def evaluate_test_code(family: dict[str, Any], *, claim: dict[str, Any] | None = None) -> VerificationEvaluation:
    test_code = dict(family.get("testCode") or {})
    status = _string(test_code.get("testCodeStatus")) or "pending"
    denominator = int((claim or {}).get("denominator") or test_code.get("requestedMethodCount") or family.get("methodCount") or 0)
    numerator = denominator if status == "present" and denominator > 0 else 0
    progress_percent = round((numerator / denominator) * 100, 2) if denominator > 0 else 0.0
    evidence = []
    test_project_path = _string(test_code.get("testProjectPath"))
    if test_project_path:
        evidence.append(
            {
                "label": Path(test_project_path).name or test_project_path,
                "path": test_project_path,
                "artifactKind": "test-project",
                "linkTargetType": "directory",
            }
        )
    return VerificationEvaluation(
        status=status,
        numerator=numerator,
        denominator=denominator,
        progressPercent=progress_percent,
        reason="",
        evidence=evidence,
        runs=[],
        methodDetails=[],
    )


def build_family_verification_snapshot(
    repo_root: Path,
    *,
    assembly_name: str,
    owner_subject_id: str,
    families: list[dict[str, Any]],
    native_proof_claims: dict[str, dict[str, Any]],
    projects: list[dict[str, Any]],
) -> dict[str, Any]:
    snapshot_families: list[dict[str, Any]] = []
    for family in families:
        snapshot_family = dict(family)
        family_id = _string(family.get("familyId"))
        snapshot_family["nativeProof"] = evaluate_native_proof(
            repo_root,
            family,
            claim=native_proof_claims.get(f"{family_id}::native-proof", {}),
            projects=projects,
        ).to_dict()
        snapshot_family["managedProof"] = evaluate_generic_gate(
            family,
            claim=native_proof_claims.get(f"{family_id}::managed-proof"),
            gate_code="managed-proof",
            projects=projects,
        ).to_dict()
        snapshot_family["hotupdateProof"] = evaluate_generic_gate(
            family,
            claim=native_proof_claims.get(f"{family_id}::hotupdate-proof"),
            gate_code="hotupdate-proof",
            projects=projects,
        ).to_dict()
        snapshot_family["benchmarkProof"] = evaluate_generic_gate(
            family,
            claim=native_proof_claims.get(f"{family_id}::benchmark"),
            gate_code="benchmark",
            projects=projects,
        ).to_dict()
        snapshot_family["testCodeProof"] = evaluate_test_code(
            family,
            claim=native_proof_claims.get(f"{family_id}::test-code"),
        ).to_dict()
        snapshot_families.append(snapshot_family)
    return {
        "schemaVersion": 1,
        "assemblyName": assembly_name,
        "ownerSubjectId": owner_subject_id,
        "families": snapshot_families,
    }
