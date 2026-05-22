from __future__ import annotations

from datetime import datetime, timezone
from typing import Any


SCHEMA_VERSION = 1
SOURCE_SPEC_VERSION = "unified-test-framework"
DEFAULT_PRODUCER_ID = "unified-test-framework"


def _utc_timestamp() -> str:
    return datetime.now(timezone.utc).isoformat().replace("+00:00", "Z")


def _string(value: Any) -> str:
    return str(value or "")


def _string_list(values: Any) -> list[str]:
    if values in (None, ""):
        return []
    if isinstance(values, (list, tuple)):
        return [str(value) for value in values if str(value)]
    return [str(values)]


def _blockers(values: Any) -> list[dict[str, str]]:
    normalized: list[dict[str, str]] = []
    for raw in list(values or []):
        item = dict(raw or {})
        normalized.append(
            {
                "code": _string(item.get("code")),
                "lane": _string(item.get("lane")),
                "message": _string(item.get("message")),
            }
        )
    return normalized


def _source_refs(values: Any) -> list[dict[str, str]]:
    normalized: list[dict[str, str]] = []
    for raw in list(values or []):
        item = dict(raw or {})
        normalized.append(
            {
                "kind": _string(item.get("kind")),
                "path": _string(item.get("path")),
                "label": _string(item.get("label")),
                "hash": _string(item.get("hash")),
            }
        )
    return normalized


def _base_payload(
    *,
    generated_at_utc: str | None,
    producer_id: str | None,
    source_spec_version: str | None,
) -> dict[str, Any]:
    return {
        "schemaVersion": SCHEMA_VERSION,
        "generatedAtUtc": _string(generated_at_utc) or _utc_timestamp(),
        "producerId": _string(producer_id) or DEFAULT_PRODUCER_ID,
        "sourceSpecVersion": _string(source_spec_version) or SOURCE_SPEC_VERSION,
    }


def build_snapshot(
    items: list[dict[str, Any]],
    *,
    closure_id: str,
    generated_at_utc: str | None = None,
    producer_id: str | None = None,
    source_spec_version: str | None = None,
) -> dict[str, Any]:
    payload = _base_payload(
        generated_at_utc=generated_at_utc,
        producer_id=producer_id,
        source_spec_version=source_spec_version,
    )
    payload["closureId"] = _string(closure_id)
    payload["items"] = list(items or [])
    return payload


def build_master(
    items: list[dict[str, Any]],
    *,
    merge_key: str,
    generated_at_utc: str | None = None,
    producer_id: str | None = None,
    source_spec_version: str | None = None,
) -> dict[str, Any]:
    payload = _base_payload(
        generated_at_utc=generated_at_utc,
        producer_id=producer_id,
        source_spec_version=source_spec_version,
    )
    payload["mergeKey"] = _string(merge_key)
    payload["items"] = list(items or [])
    return payload


def build_closure_record(
    *,
    closure_id: str,
    closure_kind: str,
    scope_code: str,
    template_id: str,
    overlay_id: str,
    target_capabilities: list[str] | None = None,
    required_routes: list[str] | None = None,
    required_platforms: list[str] | None = None,
    required_device_profiles: list[str] | None = None,
    required_evidence_kinds: list[str] | None = None,
    mandatory_lanes: list[str] | None = None,
    verification_state: str,
    support_state: str,
    blockers: list[dict[str, Any]] | None = None,
    obligation_count: int = 0,
    passed_count: int = 0,
    failed_count: int = 0,
    blocked_count: int = 0,
    missing_count: int = 0,
    not_required_count: int = 0,
) -> dict[str, Any]:
    return {
        "closureId": _string(closure_id),
        "closureKind": _string(closure_kind),
        "scopeCode": _string(scope_code),
        "templateId": _string(template_id),
        "overlayId": _string(overlay_id),
        "targetCapabilities": _string_list(target_capabilities),
        "requiredRoutes": _string_list(required_routes),
        "requiredPlatforms": _string_list(required_platforms),
        "requiredDeviceProfiles": _string_list(required_device_profiles),
        "requiredEvidenceKinds": _string_list(required_evidence_kinds),
        "mandatoryLanes": _string_list(mandatory_lanes),
        "verificationState": _string(verification_state),
        "supportState": _string(support_state),
        "blockers": _blockers(blockers),
        "obligationCount": int(obligation_count or 0),
        "passedCount": int(passed_count or 0),
        "failedCount": int(failed_count or 0),
        "blockedCount": int(blocked_count or 0),
        "missingCount": int(missing_count or 0),
        "notRequiredCount": int(not_required_count or 0),
    }


def build_obligation_claim_record(
    *,
    obligation_claim_id: str,
    closure_id: str,
    capability_id: str,
    owner_subject_id: str,
    required_evidence_kinds: list[str] | None = None,
    required_routes: list[str] | None = None,
    required_platforms: list[str] | None = None,
    required_device_profiles: list[str] | None = None,
    mandatory_lanes: list[str] | None = None,
    support_state: str,
    verification_state: str,
    blockers: list[dict[str, Any]] | None = None,
    evidence_claim_ids: list[str] | None = None,
    latest_stage_id: str | None = None,
    capability_family: str | None = None,
    projection_meta: dict[str, Any] | None = None,
) -> dict[str, Any]:
    return {
        "obligationClaimId": _string(obligation_claim_id),
        "closureId": _string(closure_id),
        "capabilityId": _string(capability_id),
        "ownerSubjectId": _string(owner_subject_id),
        "capabilityFamily": _string(capability_family),
        "requiredEvidenceKinds": _string_list(required_evidence_kinds),
        "requiredRoutes": _string_list(required_routes),
        "requiredPlatforms": _string_list(required_platforms),
        "requiredDeviceProfiles": _string_list(required_device_profiles),
        "mandatoryLanes": _string_list(mandatory_lanes),
        "supportState": _string(support_state),
        "verificationState": _string(verification_state),
        "blockers": _blockers(blockers),
        "evidenceClaimIds": _string_list(evidence_claim_ids),
        "latestStageId": _string(latest_stage_id),
        "projectionMeta": dict(projection_meta or {}),
    }


def build_evidence_claim_record(
    *,
    evidence_claim_id: str,
    obligation_claim_id: str,
    scenario_id: str,
    scenario_kind: str,
    route_code: str,
    platform_code: str,
    device_profile_code: str,
    evidence_kind: str,
    execution_contract_id: str,
    verification_state: str,
    support_state: str,
    blockers: list[dict[str, Any]] | None = None,
    latest_evidence_id: str | None = None,
    stage_kind: str | None = None,
    projection_meta: dict[str, Any] | None = None,
) -> dict[str, Any]:
    return {
        "evidenceClaimId": _string(evidence_claim_id),
        "obligationClaimId": _string(obligation_claim_id),
        "scenarioId": _string(scenario_id),
        "scenarioKind": _string(scenario_kind),
        "routeCode": _string(route_code),
        "platformCode": _string(platform_code),
        "deviceProfileCode": _string(device_profile_code),
        "evidenceKind": _string(evidence_kind),
        "executionContractId": _string(execution_contract_id),
        "verificationState": _string(verification_state),
        "supportState": _string(support_state),
        "blockers": _blockers(blockers),
        "latestEvidenceId": _string(latest_evidence_id),
        "stageKind": _string(stage_kind),
        "projectionMeta": dict(projection_meta or {}),
    }


def build_evidence_record(
    *,
    evidence_id: str,
    evidence_claim_id: str,
    obligation_claim_id: str,
    closure_id: str,
    stage_id: str,
    route_code: str,
    platform_code: str,
    device_profile_code: str,
    actual_device_id: str | None,
    verification_state: str,
    support_state: str,
    blockers: list[dict[str, Any]] | None = None,
    source_refs: list[dict[str, Any]] | None = None,
    payload_kind: str,
    payload: dict[str, Any] | None = None,
) -> dict[str, Any]:
    kernel = {
        "evidenceId": _string(evidence_id),
        "evidenceClaimId": _string(evidence_claim_id),
        "obligationClaimId": _string(obligation_claim_id),
        "closureId": _string(closure_id),
        "stageId": _string(stage_id),
        "routeCode": _string(route_code),
        "platformCode": _string(platform_code),
        "deviceProfileCode": _string(device_profile_code),
        "actualDeviceId": _string(actual_device_id),
        "verificationState": _string(verification_state),
        "supportState": _string(support_state),
        "blockers": _blockers(blockers),
        "sourceRefs": _source_refs(source_refs),
        "payloadHash": "",
        "payloadKind": _string(payload_kind),
    }
    return {
        "evidenceId": kernel["evidenceId"],
        "evidenceClaimId": kernel["evidenceClaimId"],
        "obligationClaimId": kernel["obligationClaimId"],
        "closureId": kernel["closureId"],
        "stageId": kernel["stageId"],
        "kernel": kernel,
        "payloadKind": _string(payload_kind),
        "payload": dict(payload or {}),
    }


def build_stage_report_record(
    *,
    stage_id: str,
    closure_id: str,
    scope_code: str,
    stage_kind: str,
    verification_state: str,
    support_state: str,
    covered_obligation_claim_ids: list[str] | None = None,
    success_evidence_ids: list[str] | None = None,
    failed_evidence_ids: list[str] | None = None,
    blocked_evidence_ids: list[str] | None = None,
    missing_evidence_claim_ids: list[str] | None = None,
    blockers: list[dict[str, Any]] | None = None,
    report_refs: list[dict[str, Any]] | None = None,
) -> dict[str, Any]:
    return {
        "stageId": _string(stage_id),
        "closureId": _string(closure_id),
        "scopeCode": _string(scope_code),
        "stageKind": _string(stage_kind),
        "verificationState": _string(verification_state),
        "supportState": _string(support_state),
        "coveredObligationClaimIds": _string_list(covered_obligation_claim_ids),
        "successEvidenceIds": _string_list(success_evidence_ids),
        "failedEvidenceIds": _string_list(failed_evidence_ids),
        "blockedEvidenceIds": _string_list(blocked_evidence_ids),
        "missingEvidenceClaimIds": _string_list(missing_evidence_claim_ids),
        "blockers": _blockers(blockers),
        "reportRefs": _source_refs(report_refs),
    }


def build_capability_summary_record(
    *,
    capability_id: str,
    display_name: str,
    owner_subject_id: str,
    default_routes: list[str] | None = None,
    default_platforms: list[str] | None = None,
    default_device_profiles: list[str] | None = None,
    required_evidence_kinds: list[str] | None = None,
    verification_state: str,
    support_state: str,
    blockers: list[dict[str, Any]] | None = None,
    latest_closure_id: str | None = None,
    latest_obligation_claim_ids: list[str] | None = None,
    projection_meta: dict[str, Any] | None = None,
) -> dict[str, Any]:
    return {
        "capabilityId": _string(capability_id),
        "displayName": _string(display_name),
        "ownerSubjectId": _string(owner_subject_id),
        "defaultRoutes": _string_list(default_routes),
        "defaultPlatforms": _string_list(default_platforms),
        "defaultDeviceProfiles": _string_list(default_device_profiles),
        "requiredEvidenceKinds": _string_list(required_evidence_kinds),
        "verificationState": _string(verification_state),
        "supportState": _string(support_state),
        "blockers": _blockers(blockers),
        "latestClosureId": _string(latest_closure_id),
        "latestObligationClaimIds": _string_list(latest_obligation_claim_ids),
        "projectionMeta": dict(projection_meta or {}),
    }
