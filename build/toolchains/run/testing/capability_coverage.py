from __future__ import annotations

from enum import IntEnum
import json
from pathlib import Path
from typing import Any
import sys

try:
    from . import declared_metadata_labels as declared_metadata_labels_module
except ImportError:
    root = Path(__file__).resolve().parents[1]
    sys.path.insert(0, str(root))
    from testing import declared_metadata_labels as declared_metadata_labels_module


class BodyAvailabilityCode(IntEnum):
    NONE = 0
    NATIVE_GENERATED = 1
    INTERPRETER_READY = 2
    BRIDGE_DISPATCH = 3
    METADATA_ONLY = 4
    EXTERNAL_RUNTIME = 5
    UNSUPPORTED = 6


_SUPPORTED_SCHEMA_VERSION = 1
_DEFAULT_REPO_ROOT = Path(__file__).resolve().parents[4]
_FEATURE_CONTRACT_CACHE: dict[str, dict[int, dict[str, Any]]] = {}


def _int_value(value: Any) -> int:
    try:
        return int(value or 0)
    except (TypeError, ValueError):
        return 0


def _bool_value(value: Any, *, default: bool) -> bool:
    if value is None:
        return default
    if isinstance(value, bool):
        return value
    if isinstance(value, (int, float)):
        return bool(value)
    if isinstance(value, str):
        normalized = value.strip().lower()
        if normalized in {"1", "true", "yes"}:
            return True
        if normalized in {"0", "false", "no", ""}:
            return False
    return default


def _normalize_repo_root(repo_root: Path | str | None) -> Path:
    if repo_root is None:
        return _DEFAULT_REPO_ROOT
    if isinstance(repo_root, Path):
        return repo_root
    return Path(repo_root)


def _subject_features_root(repo_root: Path) -> Path:
    return repo_root / "subjects"


def _cache_key(repo_root: Path) -> str:
    return str(repo_root.resolve())


def _normalize_feature_entry(*, subject_id: str, raw_entry: Any) -> dict[str, Any] | None:
    if not isinstance(raw_entry, dict):
        return None

    capability_item = _int_value(raw_entry.get("capabilityItem"))
    if capability_item <= 0:
        return None

    support_states = [
        _int_value(state)
        for state in raw_entry.get("supportStates", [])
        if _int_value(state) >= 0
    ]

    return {
        "capabilityFamily": _int_value(raw_entry.get("capabilityFamily")),
        "capabilityItem": capability_item,
        "ownerSubjectId": subject_id,
        "supportStates": support_states,
        "proofRequired": _bool_value(raw_entry.get("proofRequired"), default=True),
        "benchmarkRequired": _bool_value(raw_entry.get("benchmarkRequired"), default=False),
    }


def _load_feature_contracts(repo_root: Path) -> dict[int, dict[str, Any]]:
    cache_key = _cache_key(repo_root)
    cached_contracts = _FEATURE_CONTRACT_CACHE.get(cache_key)
    if cached_contracts is not None:
        return cached_contracts

    contracts_by_item: dict[int, dict[str, Any]] = {}
    subjects_root = _subject_features_root(repo_root)
    if not subjects_root.is_dir():
        _FEATURE_CONTRACT_CACHE[cache_key] = contracts_by_item
        return contracts_by_item

    for feature_path in sorted(subjects_root.glob("*/subject.features.json")):
        payload = json.loads(feature_path.read_text(encoding="utf-8"))
        schema_version = _int_value(payload.get("schemaVersion"))
        if schema_version != _SUPPORTED_SCHEMA_VERSION:
            raise ValueError(f"unsupported subject.features.json schemaVersion: {feature_path}")

        subject_id = str(payload.get("subjectId") or feature_path.parent.name).strip()
        if not subject_id:
            raise ValueError(f"subject.features.json missing subjectId: {feature_path}")

        for raw_entry in payload.get("features", []):
            normalized_entry = _normalize_feature_entry(subject_id=subject_id, raw_entry=raw_entry)
            if normalized_entry is None:
                continue

            capability_item = int(normalized_entry["capabilityItem"])
            existing_entry = contracts_by_item.get(capability_item)
            if existing_entry is not None and existing_entry["ownerSubjectId"] != subject_id:
                raise ValueError(
                    "duplicate capability ownership in subject.features.json: "
                    f"item={capability_item}, owners={existing_entry['ownerSubjectId']} and {subject_id}"
                )
            contracts_by_item[capability_item] = normalized_entry

    _FEATURE_CONTRACT_CACHE[cache_key] = contracts_by_item
    return contracts_by_item


def resolve_capability_contract(
    *,
    capability_family: Any,
    capability_item: Any,
    repo_root: Path | str | None = None,
) -> dict[str, Any]:
    capability_item_value = _int_value(capability_item)
    declared_family_value = _int_value(capability_family)
    contracts_by_item = _load_feature_contracts(_normalize_repo_root(repo_root))
    feature_contract = contracts_by_item.get(capability_item_value)

    resolved_family_value = declared_family_value
    support_states: list[int] = []
    owner_subject_id = ""
    proof_required = True
    benchmark_required = False

    if feature_contract is not None:
        resolved_family_value = _int_value(feature_contract.get("capabilityFamily")) or declared_family_value
        support_states = [int(state) for state in feature_contract.get("supportStates", [])]
        owner_subject_id = str(feature_contract.get("ownerSubjectId") or "")
        proof_required = bool(feature_contract.get("proofRequired"))
        benchmark_required = bool(feature_contract.get("benchmarkRequired"))

    return {
        "capabilityFamily": resolved_family_value,
        "capabilityFamilyLabel": declared_metadata_labels_module.capability_family_label(resolved_family_value),
        "capabilityItem": capability_item_value,
        "capabilityItemLabel": declared_metadata_labels_module.capability_item_label(capability_item_value),
        "ownerSubjectId": owner_subject_id,
        "supportStates": support_states,
        "supportStateLabels": declared_metadata_labels_module.body_availability_labels(support_states),
        "proofRequired": proof_required,
        "benchmarkRequired": benchmark_required,
    }
