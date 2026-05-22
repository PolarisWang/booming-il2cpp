from __future__ import annotations

from datetime import datetime, timezone
from pathlib import Path
from typing import Any
import sys

try:
    from ..core.common import write_json
    from . import verification_layout as verification_layout_module
except ImportError:
    root = Path(__file__).resolve().parents[1]
    sys.path.insert(0, str(root))
    from core.common import write_json
    from testing import verification_layout as verification_layout_module


def _utc_timestamp() -> str:
    return datetime.now(timezone.utc).isoformat().replace("+00:00", "Z")


def _string(value: Any) -> str:
    return str(value or "")


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


def codegen_stub_relative_dir(
    *,
    owner_subject_id: str,
    capability_id: str,
    route_code: str,
    profile_code: str | None = None,
) -> Path:
    route_segment = _string(route_code)
    if _string(profile_code):
        route_segment = f"{route_segment}-{_string(profile_code)}"
    return Path(
        verification_layout_module.VERIFICATION_ROOT_NAME,
        "evidence",
        "owners",
        _string(owner_subject_id),
        "codegen-stubs",
        _string(capability_id),
        route_segment,
    )


def build_stub_index(
    *,
    owner_subject_id: str,
    capability_id: str,
    route_code: str,
    profile_code: str | None = None,
    managed_source_refs: list[dict[str, Any]] | None = None,
    stub_refs: list[dict[str, Any]] | None = None,
    generated_from_evidence_ids: list[str] | None = None,
    producer_id: str = "unified-test-framework",
    source_spec_version: str = "unified-test-framework",
) -> dict[str, Any]:
    merge_key_parts = [_string(owner_subject_id), _string(capability_id), _string(route_code)]
    if _string(profile_code):
        merge_key_parts.append(_string(profile_code))
    return {
        "schemaVersion": 1,
        "generatedAtUtc": _utc_timestamp(),
        "producerId": _string(producer_id),
        "sourceSpecVersion": _string(source_spec_version),
        "ownerSubjectId": _string(owner_subject_id),
        "capabilityId": _string(capability_id),
        "routeCode": _string(route_code),
        "profileCode": _string(profile_code),
        "mergeKey": "|".join(merge_key_parts),
        "managedSourceRefs": _source_refs(managed_source_refs),
        "stubRefs": _source_refs(stub_refs),
        "generatedFromEvidenceIds": [str(value) for value in list(generated_from_evidence_ids or []) if str(value)],
    }


def write_stub_index(repo_root: Path, stub_index: dict[str, Any]) -> Path:
    relative_dir = codegen_stub_relative_dir(
        owner_subject_id=str(stub_index.get("ownerSubjectId") or ""),
        capability_id=str(stub_index.get("capabilityId") or ""),
        route_code=str(stub_index.get("routeCode") or ""),
        profile_code=str(stub_index.get("profileCode") or ""),
    )
    index_path = repo_root / relative_dir / "stub-index.json"
    write_json(index_path, stub_index)
    return index_path
