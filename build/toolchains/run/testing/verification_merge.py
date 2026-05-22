from __future__ import annotations

from datetime import datetime, timezone
from typing import Any


def _utc_timestamp() -> str:
    return datetime.now(timezone.utc).isoformat().replace("+00:00", "Z")


def _key_tuple(item: dict[str, Any], key_fields: tuple[str, ...]) -> tuple[str, ...]:
    return tuple(str(item.get(field) or "") for field in key_fields)


def merge_items(
    existing_items: list[dict[str, Any]],
    incoming_items: list[dict[str, Any]],
    *,
    key_fields: tuple[str, ...],
) -> list[dict[str, Any]]:
    merged = [dict(item or {}) for item in list(existing_items or [])]
    index_by_key = {_key_tuple(item, key_fields): index for index, item in enumerate(merged)}

    for raw in list(incoming_items or []):
        item = dict(raw or {})
        key = _key_tuple(item, key_fields)
        position = index_by_key.get(key)
        if position is None:
            index_by_key[key] = len(merged)
            merged.append(item)
            continue
        merged[position] = item

    return merged


def merge_master_payload(
    existing_payload: dict[str, Any] | None,
    incoming_payload: dict[str, Any],
    *,
    key_fields: tuple[str, ...],
    merge_key: str | None = None,
) -> dict[str, Any]:
    existing = dict(existing_payload or {})
    incoming = dict(incoming_payload or {})
    return {
        "schemaVersion": int(incoming.get("schemaVersion") or existing.get("schemaVersion") or 1),
        "generatedAtUtc": str(incoming.get("generatedAtUtc") or _utc_timestamp()),
        "producerId": str(incoming.get("producerId") or existing.get("producerId") or "unified-test-framework"),
        "sourceSpecVersion": str(
            incoming.get("sourceSpecVersion") or existing.get("sourceSpecVersion") or "unified-test-framework"
        ),
        "mergeKey": str(merge_key or incoming.get("mergeKey") or existing.get("mergeKey") or ""),
        "items": merge_items(
            list(existing.get("items") or []),
            list(incoming.get("items") or []),
            key_fields=key_fields,
        ),
    }


def merge_closure_master(existing_payload: dict[str, Any] | None, incoming_payload: dict[str, Any]) -> dict[str, Any]:
    return merge_master_payload(
        existing_payload,
        incoming_payload,
        key_fields=("closureId",),
        merge_key="closureId",
    )


def merge_capability_master(existing_payload: dict[str, Any] | None, incoming_payload: dict[str, Any]) -> dict[str, Any]:
    return merge_master_payload(
        existing_payload,
        incoming_payload,
        key_fields=("capabilityId",),
        merge_key="capabilityId",
    )


def merge_stage_master(existing_payload: dict[str, Any] | None, incoming_payload: dict[str, Any]) -> dict[str, Any]:
    return merge_master_payload(
        existing_payload,
        incoming_payload,
        key_fields=("stageId", "scopeCode"),
        merge_key="stageId+scopeCode",
    )


def merge_result_master(existing_payload: dict[str, Any] | None, incoming_payload: dict[str, Any]) -> dict[str, Any]:
    return merge_master_payload(
        existing_payload,
        incoming_payload,
        key_fields=("obligationClaimId",),
        merge_key="obligationClaimId",
    )


def merge_evidence_claim_master(
    existing_payload: dict[str, Any] | None,
    incoming_payload: dict[str, Any],
) -> dict[str, Any]:
    return merge_master_payload(
        existing_payload,
        incoming_payload,
        key_fields=("evidenceClaimId",),
        merge_key="evidenceClaimId",
    )
