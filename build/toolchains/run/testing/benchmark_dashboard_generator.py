"""Generate static benchmark dashboard data and HTML."""
from __future__ import annotations

import importlib.util
import json
import sys
from datetime import datetime, timezone
from pathlib import Path
from typing import Any

try:
    from . import capability_coverage as capability_coverage_module
    from . import declared_metadata_labels as declared_metadata_labels_module
    from . import verification_layout as verification_layout_module
    from . import workspace_declared_collection as workspace_declared_collection_module
except ImportError:
    testing_root = Path(__file__).resolve().parent
    if str(testing_root) not in sys.path:
        sys.path.insert(0, str(testing_root))
    import capability_coverage as capability_coverage_module
    import declared_metadata_labels as declared_metadata_labels_module
    import verification_layout as verification_layout_module
    import workspace_declared_collection as workspace_declared_collection_module


_MODE_ORDER = declared_metadata_labels_module.MODE_ORDER
_DEFAULT_PLATFORM = "windows-x64"
_STALE_AFTER_DAYS = 7.0
_MODE_FLAGS = declared_metadata_labels_module.MODE_FLAGS
_ALL_MODE_FLAGS = declared_metadata_labels_module.ALL_MODE_FLAGS
_PLATFORM_KEY_PREFIXES = {"windows", "linux", "macos", "android", "ios"}


def _load(name: str, path: Path):
    spec = importlib.util.spec_from_file_location(name, path)
    assert spec and spec.loader
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)  # type: ignore[union-attr]
    return module


def _testing_support_path(repo_root: Path, *relative_parts: str) -> Path:
    repo_path = repo_root / "build" / "toolchains" / "run" / "testing" / Path(*relative_parts)
    if repo_path.is_file():
        return repo_path
    return Path(__file__).resolve().parent.joinpath(*relative_parts)


def _import_verification_projection_module():
    try:
        from . import verification_projection as verification_projection_module
    except ImportError:
        testing_root = Path(__file__).resolve().parent
        if str(testing_root) not in sys.path:
            sys.path.insert(0, str(testing_root))
        import verification_projection as verification_projection_module
    return verification_projection_module


def _sort_modes(values: list[str]) -> list[str]:
    order = {mode: index for index, mode in enumerate(_MODE_ORDER)}
    return sorted({str(value) for value in values if str(value)}, key=lambda item: order.get(item, 999))


def _effective_supported_modes(configured_modes: list[str], latest_by_mode: dict[str, Any]) -> list[str]:
    return _sort_modes(list(configured_modes or []) + [mode for mode in latest_by_mode if mode in _MODE_ORDER])


def _supported_modes_from_mask(value: Any) -> list[str]:
    return declared_metadata_labels_module.supported_modes_from_mask(value)


def _labels_from_mask(value: Any, labels: dict[int, str]) -> list[str]:
    return declared_metadata_labels_module.labels_from_mask(value, labels)


def _capability_contract_payload(item: dict[str, Any]) -> dict[str, Any]:
    contract = capability_coverage_module.resolve_capability_contract(
        capability_family=item.get("capabilityFamily"),
        capability_item=item.get("capabilityItem"),
    )
    return {
        "capabilityFamily": int(contract.get("capabilityFamily") or 0),
        "capabilityFamilyLabel": str(contract.get("capabilityFamilyLabel") or ""),
        "capabilityItem": int(contract.get("capabilityItem") or 0),
        "capabilityItemLabel": str(contract.get("capabilityItemLabel") or ""),
        "ownerSubjectId": str(contract.get("ownerSubjectId") or ""),
        "supportStates": [int(value) for value in list(contract.get("supportStates") or [])],
        "supportStateLabels": [str(value) for value in list(contract.get("supportStateLabels") or [])],
        "proofRequired": bool(contract.get("proofRequired", False)),
        "benchmarkRequired": bool(contract.get("benchmarkRequired", False)),
    }


def _mode_reason_payload(*, status: str, declared_by: str) -> dict[str, str]:
    if status == "recorded":
        return {
            "reasonCode": "recorded",
            "reasonLabel": "Benchmark record captured.",
        }
    if status == "missing":
        return {
            "reasonCode": "missing-record",
            "reasonLabel": f"Declared by {declared_by}, but no benchmark record was found.",
        }
    return {
        "reasonCode": "unsupported-by-contract",
        "reasonLabel": f"This mode is not declared by the {declared_by}.",
    }


def _record_git_commit(record: dict[str, Any]) -> str:
    return str(record.get("gitCommit") or "").strip()


def _stale_record_reason(*, latest_git_commit: str, latest_recorded_at: str) -> dict[str, str]:
    pieces = ["This record was captured on an older commit than the latest selected subject record."]
    if latest_git_commit:
        pieces.append(f"Latest commit: {latest_git_commit}.")
    if latest_recorded_at:
        pieces.append(f"Latest record time: {latest_recorded_at}.")
    return {
        "staleReasonCode": "stale-record",
        "staleReasonLabel": " ".join(pieces),
    }


def _cross_commit_comparison_reason(*, current_commit: str, managed_commit: str) -> dict[str, str]:
    return {
        "reasonCode": "cross-commit-record",
        "reasonLabel": (
            "Managed baseline and comparison record were captured from different commits "
            f"({managed_commit or 'unknown'} vs {current_commit or 'unknown'}); relative comparison is hidden."
        ),
    }


def _stale_comparison_note() -> dict[str, str]:
    return {
        "staleReasonCode": "stale-record",
        "staleReasonLabel": "This comparison is not from the latest subject commit.",
    }


def _baseline_unavailable_reason(managed_entry: dict[str, Any]) -> dict[str, str]:
    baseline_reason_code = str(managed_entry.get("reasonCode") or "")
    if baseline_reason_code == "missing-record":
        return {
            "reasonCode": "baseline-missing-record",
            "reasonLabel": "Managed baseline is declared, but no benchmark record was found.",
        }
    if baseline_reason_code == "unsupported-by-contract":
        return {
            "reasonCode": "baseline-unsupported-by-contract",
            "reasonLabel": "Managed baseline is not declared by this contract.",
        }
    return {
        "reasonCode": "baseline-unavailable",
        "reasonLabel": "Managed baseline must be recorded before relative comparison is available.",
    }


def _normalize_platform_key(value: str) -> str:
    text = str(value or "").strip().lower()
    if not text:
        return ""
    parts = text.split("-")
    if len(parts) >= 2 and parts[0] in _PLATFORM_KEY_PREFIXES:
        return "-".join(parts[:2])
    return text


def _looks_like_platform_key(value: str) -> bool:
    text = str(value or "").strip().lower()
    if not text:
        return False
    parts = text.split("-")
    return len(parts) >= 2 and parts[0] in _PLATFORM_KEY_PREFIXES


def _parse_timestamp(value: str | None) -> datetime | None:
    text = str(value or "").strip()
    if not text:
        return None
    try:
        return datetime.fromisoformat(text.replace("Z", "+00:00"))
    except ValueError:
        return None


def _metric_value(metrics: dict[str, Any], metric_key: str) -> float | None:
    if metric_key == "meanDurationMs":
        candidates = ("meanDurationMs", "meanElapsedMilliseconds", "elapsedMilliseconds")
    elif metric_key == "meanOpsPerSecond":
        candidates = ("meanOpsPerSecond", "opsPerSecond")
    else:
        candidates = (metric_key,)

    for candidate in candidates:
        value = metrics.get(candidate)
        if isinstance(value, (int, float)) and not isinstance(value, bool):
            return float(value)
    return None


def _declared_source_entry(entry: dict[str, Any]) -> str:
    assembly_name = str(entry.get("assemblyName") or "")
    declaring_type = str(entry.get("declaringType") or "")
    method_signature = str(entry.get("methodSignature") or "")
    if not assembly_name or not declaring_type or not method_signature:
        return ""
    type_name = declaring_type.rsplit(".", 1)[-1]
    return f"{assembly_name}/{type_name}::{method_signature}"


def _load_declared_benchmark_cases(repo_root: Path, subject_id: str) -> dict[str, dict[str, Any]]:
    workspace_collection = workspace_declared_collection_module.load_workspace_declared_collection(
        repo_root,
        subject_id,
        host_kind="benchmark-host",
    )
    if workspace_collection is not None:
        return _declared_benchmark_cases_from_catalog(workspace_collection)

    testing_root = repo_root / "build" / "toolchains" / "run" / "testing"
    try:
        compiled_catalog_mod = _load("compiled_catalog", testing_root / "compiled_catalog.py")
        catalog = compiled_catalog_mod.build_subject_declared_test_catalog(
            repo_root=repo_root,
            subject_id=subject_id,
            force_build=False,
        )
    except Exception:
        return {}

    return _declared_benchmark_cases_from_catalog(dict(catalog))


def _declared_benchmark_cases_from_catalog(catalog: dict[str, Any]) -> dict[str, dict[str, Any]]:
    cases: dict[str, dict[str, Any]] = {}
    for payload in list(dict(catalog).get("declaredBenchmarks") or []):
        item = dict(payload or {})
        stable_id = str(item.get("stableId") or "").strip()
        if not stable_id:
            continue
        modes = int(item.get("modes") or 0)
        case_payload = {
            "stableId": stable_id,
            "alias": str(item.get("alias") or "").strip() or stable_id,
            "displayName": str(item.get("alias") or "").strip() or stable_id,
            "workloadEntry": _declared_source_entry(item),
            "assemblyName": str(item.get("assemblyName") or ""),
            "declaringType": str(item.get("declaringType") or ""),
            "methodName": str(item.get("methodName") or ""),
            "methodSignature": str(item.get("methodSignature") or ""),
            "category": int(item.get("category") or 0),
            "categoryLabel": declared_metadata_labels_module.benchmark_category_label(item.get("category")),
            "metrics": int(item.get("metrics") or 0),
            "metricLabels": _labels_from_mask(item.get("metrics"), declared_metadata_labels_module.METRIC_LABELS),
            "modes": modes,
            "supportedModes": _supported_modes_from_mask(modes),
            "requires": int(item.get("requires") or 0),
            "requirementLabels": _labels_from_mask(
                item.get("requires"),
                declared_metadata_labels_module.RUNTIME_FEATURE_LABELS,
            ),
            "archetype": int(item.get("archetype") or 0),
            "archetypeLabel": declared_metadata_labels_module.archetype_label(item.get("archetype")),
            "hotUpdateCapability": int(item.get("hotUpdateCapability") or 0),
            "hotUpdateCapabilityLabels": _labels_from_mask(
                item.get("hotUpdateCapability"),
                declared_metadata_labels_module.HOT_UPDATE_CAPABILITY_LABELS,
            ),
            "warmupCount": int(item.get("warmupCount") or 0),
            "iterationCount": int(item.get("iterationCount") or 0),
            "invocationCount": int(item.get("invocationCount") or 0),
        }
        case_payload.update(_capability_contract_payload(item))
        entry_index = item.get("entryIndex")
        if isinstance(entry_index, int) and not isinstance(entry_index, bool) and entry_index >= 0:
            case_payload["entryIndex"] = int(entry_index)
        cases[stable_id] = case_payload
    return cases


def _merge_case_meta(base: dict[str, Any], incoming: dict[str, Any]) -> dict[str, Any]:
    merged = dict(base or {})
    for key, value in incoming.items():
        if key in {"supportedModes", "metricLabels", "requirementLabels"}:
            values = [str(item) for item in list(value or []) if str(item)]
            if values:
                merged[key] = values
            elif key not in merged:
                merged[key] = []
            continue
        if value not in (None, "", [], {}):
            merged[key] = value
    return merged


def _build_declared_case_lookup(declared_cases: dict[str, dict[str, Any]]) -> dict[str, dict[str, str]]:
    lookup: dict[str, dict[str, str]] = {
        "stableId": {},
        "entryIndex": {},
        "alias": {},
        "displayName": {},
        "workloadEntry": {},
    }
    collisions: dict[str, set[str]] = {
        "entryIndex": set(),
        "alias": set(),
        "displayName": set(),
        "workloadEntry": set(),
    }

    for case_id, case_meta in declared_cases.items():
        stable_id = str(case_meta.get("stableId") or case_id).strip()
        if not stable_id:
            continue
        lookup["stableId"][stable_id] = stable_id
        entry_index = case_meta.get("entryIndex")
        if isinstance(entry_index, int) and not isinstance(entry_index, bool) and entry_index >= 0:
            entry_index_key = str(int(entry_index))
            existing_entry = lookup["entryIndex"].get(entry_index_key)
            if existing_entry and existing_entry != stable_id:
                collisions["entryIndex"].add(entry_index_key)
            else:
                lookup["entryIndex"][entry_index_key] = stable_id
        for field in ("alias", "displayName", "workloadEntry"):
            value = str(case_meta.get(field) or "").strip()
            if not value:
                continue
            existing = lookup[field].get(value)
            if existing and existing != stable_id:
                collisions[field].add(value)
                continue
            lookup[field][value] = stable_id

    for field, values in collisions.items():
        for value in values:
            lookup[field].pop(value, None)

    return lookup


def _resolve_declared_case_id(
    benchmark_case: dict[str, Any],
    *,
    declared_cases: dict[str, dict[str, Any]],
    declared_case_lookup: dict[str, dict[str, str]],
) -> str | None:
    stable_id = str(benchmark_case.get("stableId") or benchmark_case.get("alias") or "").strip()
    if not stable_id:
        return None
    if not declared_cases:
        return stable_id

    direct = declared_case_lookup["stableId"].get(stable_id)
    if direct:
        return direct

    entry_index = benchmark_case.get("entryIndex")
    if isinstance(entry_index, int) and not isinstance(entry_index, bool) and entry_index >= 0:
        resolved = declared_case_lookup["entryIndex"].get(str(int(entry_index)))
        if resolved:
            return resolved

    for field in ("alias", "displayName", "workloadEntry"):
        value = str(benchmark_case.get(field) or "").strip()
        if not value:
            continue
        resolved = declared_case_lookup[field].get(value)
        if resolved:
            return resolved
    return None


def _summary_benchmark_case_payload(case_payload: dict[str, Any]) -> dict[str, Any]:
    return {
        "caseId": str(case_payload.get("caseId") or case_payload.get("stableId") or ""),
        "stableId": str(case_payload.get("stableId") or case_payload.get("caseId") or ""),
        "alias": str(case_payload.get("alias") or case_payload.get("displayName") or ""),
        "displayName": str(case_payload.get("displayName") or case_payload.get("alias") or ""),
        "workloadEntry": str(case_payload.get("workloadEntry") or ""),
        "entryIndex": int(case_payload.get("entryIndex") or 0),
        "assemblyName": str(case_payload.get("assemblyName") or ""),
        "declaringType": str(case_payload.get("declaringType") or ""),
        "methodName": str(case_payload.get("methodName") or ""),
        "methodSignature": str(case_payload.get("methodSignature") or ""),
        "category": int(case_payload.get("category") or 0),
        "categoryLabel": str(case_payload.get("categoryLabel") or "Uncategorized"),
        "metricLabels": list(case_payload.get("metricLabels") or []),
        "requirementLabels": list(case_payload.get("requirementLabels") or []),
        "archetype": int(case_payload.get("archetype") or 0),
        "archetypeLabel": str(case_payload.get("archetypeLabel") or declared_metadata_labels_module.archetype_label(0)),
        "hotUpdateCapability": int(case_payload.get("hotUpdateCapability") or 0),
        "hotUpdateCapabilityLabels": list(case_payload.get("hotUpdateCapabilityLabels") or []),
        "capabilityFamily": int(case_payload.get("capabilityFamily") or 0),
        "capabilityFamilyLabel": str(case_payload.get("capabilityFamilyLabel") or ""),
        "capabilityItem": int(case_payload.get("capabilityItem") or 0),
        "capabilityItemLabel": str(case_payload.get("capabilityItemLabel") or ""),
        "ownerSubjectId": str(case_payload.get("ownerSubjectId") or ""),
        "supportStates": [int(value) for value in list(case_payload.get("supportStates") or [])],
        "supportStateLabels": [str(value) for value in list(case_payload.get("supportStateLabels") or [])],
        "proofRequired": bool(case_payload.get("proofRequired", False)),
        "benchmarkRequired": bool(case_payload.get("benchmarkRequired", False)),
        "supportedModes": _sort_modes(list(case_payload.get("supportedModes") or [])),
    }


def _find_summary_benchmark_case(
    workload_entry: str,
    *,
    benchmark_cases_by_device: dict[str, dict[str, Any]],
    declared_cases: dict[str, dict[str, Any]],
) -> dict[str, Any] | None:
    if not workload_entry:
        return None

    for cases in benchmark_cases_by_device.values():
        for case_payload in cases.values():
            if str(case_payload.get("workloadEntry") or "") == workload_entry:
                return _summary_benchmark_case_payload(case_payload)

    for declared_case in declared_cases.values():
        if str(declared_case.get("workloadEntry") or "") == workload_entry:
            return _summary_benchmark_case_payload(declared_case)

    return None


def _resolve_summary_workload_entry(
    manifest: dict[str, Any],
    *,
    benchmark_cases_by_device: dict[str, dict[str, Any]],
    declared_cases: dict[str, dict[str, Any]],
) -> str:
    manifest_workload_entry = str(manifest.get("workloadEntry") or "").strip()
    if manifest_workload_entry:
        summary_case = _find_summary_benchmark_case(
            manifest_workload_entry,
            benchmark_cases_by_device=benchmark_cases_by_device,
            declared_cases=declared_cases,
        )
        if summary_case is not None:
            return manifest_workload_entry

    for case_id in sorted(declared_cases):
        workload_entry = str(dict(declared_cases[case_id]).get("workloadEntry") or "").strip()
        if workload_entry:
            return workload_entry

    for device_id in sorted(benchmark_cases_by_device):
        cases = dict(benchmark_cases_by_device.get(device_id) or {})
        for case_id in sorted(cases):
            workload_entry = str(dict(cases[case_id]).get("workloadEntry") or "").strip()
            if workload_entry:
                return workload_entry

    return manifest_workload_entry


def _ratio(numerator: float | None, denominator: float | None) -> float | None:
    if numerator is None or denominator is None or denominator == 0.0:
        return None
    return round(float(numerator) / float(denominator), 2)


def _derived_ratio(*, latency_ratio: float | None, throughput_ratio: float | None) -> dict[str, Any] | None:
    if latency_ratio is not None:
        return {"value": latency_ratio, "basis": "latency"}
    if throughput_ratio is not None:
        return {"value": throughput_ratio, "basis": "throughput"}
    return None


def _record_sort_key(record: dict[str, Any]) -> tuple[float, str]:
    recorded_at = str(record.get("recordedAt") or "")
    parsed = _parse_timestamp(recorded_at)
    return ((parsed.timestamp() if parsed else 0.0), recorded_at)


def _summary_record(record: dict[str, Any]) -> dict[str, Any]:
    device = dict(record.get("device") or {})
    return {
        "mode": str(record.get("mode") or ""),
        "platform": str(record.get("platform") or ""),
        "device": {
            "id": str(device.get("id") or ""),
            "name": str(device.get("name") or ""),
        },
        "metrics": dict(record.get("metrics") or {}),
        "recordedAt": record.get("recordedAt"),
        "gitCommit": record.get("gitCommit"),
    }


def _record_platform_key(record: dict[str, Any]) -> str:
    payload = dict(record or {})
    device = dict(payload.get("device") or {})
    return _normalize_platform_key(
        str(
            payload.get("platform")
            or device.get("platformId")
            or device.get("platform")
            or ""
        )
    )


def _device_platform_key(
    device_id: str,
    *,
    modes: dict[str, Any] | None = None,
    cases: dict[str, Any] | None = None,
) -> str:
    platform_key = _normalize_platform_key(device_id)
    if platform_key and _looks_like_platform_key(device_id):
        return platform_key

    candidates: set[str] = set()
    for record in list(dict(modes or {}).values()):
        record_platform_key = _record_platform_key(dict(record or {}))
        if record_platform_key:
            candidates.add(record_platform_key)
    for case_payload in list(dict(cases or {}).values()):
        records = dict(dict(case_payload or {}).get("records") or {})
        for record in list(records.values()):
            record_platform_key = _record_platform_key(dict(record or {}))
            if record_platform_key:
                candidates.add(record_platform_key)
    if not candidates:
        return ""
    return sorted(candidates)[0]


def _choose_device_for_platform(by_device: dict[str, dict[str, Any]], platform_key: str) -> str | None:
    best_device_id: str | None = None
    best_score: tuple[int, float, str] = (-1, -1.0, "")

    for device_id, modes in by_device.items():
        device_platform_key = _device_platform_key(device_id, modes=modes)
        if platform_key and device_platform_key and device_platform_key != platform_key:
            continue
        latest_record = max(list(modes.values()), key=_record_sort_key, default=None)
        latest_timestamp = _record_sort_key(latest_record)[0] if latest_record else -1.0
        score = (len(modes), latest_timestamp, device_id)
        if score > best_score:
            best_score = score
            best_device_id = device_id

    return best_device_id


def _manifest_perf_matrices(manifest: dict[str, Any]) -> list[dict[str, Any]]:
    matrices = []
    for matrix in list(manifest.get("environmentMatrices") or []):
        payload = dict(matrix)
        goal_ids = [str(goal_id) for goal_id in list(payload.get("supportedGoals") or [])]
        if any(goal_id.startswith("perf.") for goal_id in goal_ids):
            matrices.append(payload)
    return matrices


def _manifest_platforms(manifest: dict[str, Any]) -> list[str]:
    platforms = []
    for matrix in _manifest_perf_matrices(manifest):
        execution_context = dict(matrix.get("executionContext") or {})
        platform_key = _normalize_platform_key(str(execution_context.get("hostPlatform") or ""))
        if platform_key:
            platforms.append(platform_key)
    return sorted(set(platforms))


def _pipeline_stage_kinds(manifest: dict[str, Any], pipeline_id: str) -> set[str]:
    if not pipeline_id:
        return set()
    for pipeline in list(manifest.get("executionPipelines") or []):
        payload = dict(pipeline)
        if str(payload.get("pipelineId") or "") != pipeline_id:
            continue
        return {
            str(stage.get("kind") or "")
            for stage in list(payload.get("stages") or [])
            if str(stage.get("kind") or "")
        }
    return set()


def _mode_selection_terms(mode: str) -> tuple[str, ...]:
    return {
        "managed": ("managed-benchmark", "managed-perf"),
        "native": ("native-benchmark", "native-perf"),
        "interpreter": ("interpreter-benchmark", "interpreter-perf"),
    }.get(mode, (mode,))


def _mode_stage_kinds(mode: str) -> tuple[str, ...]:
    return {
        "managed": ("runtime-perf-collect",),
        "native": ("native-runtime-perf", "mobile-native-perf"),
        "interpreter": ("interpreter-runtime-perf",),
    }.get(mode, ())


def _matrix_matches_mode(manifest: dict[str, Any], matrix: dict[str, Any], mode: str, platform_key: str) -> bool:
    goal_ids = [str(goal_id) for goal_id in list(matrix.get("supportedGoals") or [])]
    if not any(goal_id.startswith("perf.") for goal_id in goal_ids):
        return False

    execution_context = dict(matrix.get("executionContext") or {})
    host_key = _normalize_platform_key(str(execution_context.get("hostPlatform") or ""))
    if platform_key and host_key and host_key != platform_key:
        return False

    pipeline_id = str(matrix.get("pipelineId") or "")
    runtime_profile = str(execution_context.get("runtimeProfile") or "").lower()
    stage_kinds = _pipeline_stage_kinds(manifest, pipeline_id)
    expected_stage_kinds = set(_mode_stage_kinds(mode))
    if expected_stage_kinds.intersection(stage_kinds):
        return True

    haystack = " ".join((str(matrix.get("matrixId") or ""), pipeline_id, runtime_profile)).lower()
    return any(term in haystack for term in _mode_selection_terms(mode))


def _supported_modes_for_platform(
    manifest: dict[str, Any],
    *,
    platform_key: str,
) -> list[str]:
    supported = [
        mode
        for mode in _MODE_ORDER
        if any(_matrix_matches_mode(manifest, dict(matrix), mode, platform_key) for matrix in _manifest_perf_matrices(manifest))
    ]
    return _sort_modes(supported)


def _record_status_payload(mode: str, record: dict[str, Any], device_id: str) -> dict[str, Any]:
    device = dict(record.get("device") or {})
    payload = {
        "mode": mode,
        "status": "recorded",
        "deviceId": device_id,
        "deviceName": str(device.get("name") or device_id),
        "recordedAt": record.get("recordedAt"),
        "gitCommit": record.get("gitCommit"),
        "metrics": dict(record.get("metrics") or {}),
        "isStale": False,
        "staleReasonCode": "",
        "staleReasonLabel": "",
    }
    payload.update(_mode_reason_payload(status="recorded", declared_by="runtime execution"))
    return payload


def _mark_stale_record_entries(mode_status: dict[str, dict[str, Any]]) -> list[str]:
    recorded_entries = [
        (mode, entry)
        for mode, entry in mode_status.items()
        if str(entry.get("status") or "") == "recorded"
    ]
    latest_entry = max((entry for _, entry in recorded_entries), key=_record_sort_key, default=None)
    latest_git_commit = _record_git_commit(latest_entry or {})
    latest_recorded_at = str((latest_entry or {}).get("recordedAt") or "")
    stale_modes: list[str] = []

    for _, entry in recorded_entries:
        entry["isStale"] = False
        entry["staleReasonCode"] = ""
        entry["staleReasonLabel"] = ""

    if not latest_git_commit:
        return stale_modes

    for mode, entry in recorded_entries:
        entry_git_commit = _record_git_commit(entry)
        if not entry_git_commit or entry_git_commit == latest_git_commit:
            continue
        entry["isStale"] = True
        entry.update(
            _stale_record_reason(
                latest_git_commit=latest_git_commit,
                latest_recorded_at=latest_recorded_at,
            )
        )
        stale_modes.append(mode)

    return _sort_modes(stale_modes)


def _baseline_metric_payload(entry: dict[str, Any]) -> dict[str, Any]:
    metrics = dict(entry.get("metrics") or {})
    return {
        "mode": str(entry.get("mode") or "managed"),
        "status": str(entry.get("status") or "unsupported"),
        "reasonCode": str(entry.get("reasonCode") or ""),
        "reasonLabel": str(entry.get("reasonLabel") or ""),
        "durationMs": _metric_value(metrics, "meanDurationMs"),
        "opsPerSecond": _metric_value(metrics, "meanOpsPerSecond"),
        "recordedAt": entry.get("recordedAt"),
        "gitCommit": entry.get("gitCommit"),
        "isStale": bool(entry.get("isStale", False)),
        "staleReasonCode": str(entry.get("staleReasonCode") or ""),
        "staleReasonLabel": str(entry.get("staleReasonLabel") or ""),
    }


def _relative_to_managed_payload(
    *,
    mode: str,
    mode_status: dict[str, dict[str, Any]],
) -> dict[str, Any]:
    current_entry = dict(mode_status.get(mode) or {})
    managed_entry = dict(mode_status.get("managed") or {})
    current_status = str(current_entry.get("status") or "unsupported")
    managed_status = str(managed_entry.get("status") or "unsupported")
    current_git_commit = _record_git_commit(current_entry)
    managed_git_commit = _record_git_commit(managed_entry)
    current_is_stale = bool(current_entry.get("isStale", False))
    managed_is_stale = bool(managed_entry.get("isStale", False))

    payload = {
        "mode": mode,
        "status": current_status,
        "reasonCode": str(current_entry.get("reasonCode") or ""),
        "reasonLabel": str(current_entry.get("reasonLabel") or ""),
        "direction": "faster" if mode == "native" else "slower",
        "ratio": None,
        "durationMs": None,
        "opsPerSecond": None,
        "recordedAt": current_entry.get("recordedAt"),
        "gitCommit": current_entry.get("gitCommit"),
        "baselineStatus": managed_status,
        "baselineReasonCode": str(managed_entry.get("reasonCode") or ""),
        "baselineReasonLabel": str(managed_entry.get("reasonLabel") or ""),
        "isStale": current_is_stale or managed_is_stale,
        "staleReasonCode": "",
        "staleReasonLabel": "",
    }

    if current_status != "recorded":
        return payload
    if managed_status != "recorded":
        payload["status"] = "baseline-unavailable"
        payload.update(_baseline_unavailable_reason(managed_entry))
        return payload
    if current_git_commit and managed_git_commit and current_git_commit != managed_git_commit:
        payload["status"] = "stale"
        payload.update(
            _cross_commit_comparison_reason(
                current_commit=current_git_commit,
                managed_commit=managed_git_commit,
            )
        )
        return payload
    if payload["isStale"]:
        payload.update(_stale_comparison_note())

    current_metrics = dict(current_entry.get("metrics") or {})
    managed_metrics = dict(managed_entry.get("metrics") or {})
    payload["durationMs"] = _metric_value(current_metrics, "meanDurationMs")
    payload["opsPerSecond"] = _metric_value(current_metrics, "meanOpsPerSecond")

    if mode == "native":
        ratio = _derived_ratio(
            latency_ratio=_ratio(
                _metric_value(managed_metrics, "meanDurationMs"),
                _metric_value(current_metrics, "meanDurationMs"),
            ),
            throughput_ratio=_ratio(
                _metric_value(current_metrics, "meanOpsPerSecond"),
                _metric_value(managed_metrics, "meanOpsPerSecond"),
            ),
        )
    else:
        ratio = _derived_ratio(
            latency_ratio=_ratio(
                _metric_value(current_metrics, "meanDurationMs"),
                _metric_value(managed_metrics, "meanDurationMs"),
            ),
            throughput_ratio=_ratio(
                _metric_value(managed_metrics, "meanOpsPerSecond"),
                _metric_value(current_metrics, "meanOpsPerSecond"),
            ),
        )
    payload["ratio"] = ratio
    return payload


def _key_metrics_from_mode_status(mode_status: dict[str, dict[str, Any]]) -> dict[str, Any]:
    managed_metrics = dict(mode_status.get("managed", {}).get("metrics") or {})
    native_metrics = dict(mode_status.get("native", {}).get("metrics") or {})
    interpreter_metrics = dict(mode_status.get("interpreter", {}).get("metrics") or {})

    native_speedup = _derived_ratio(
        latency_ratio=_ratio(
            _metric_value(managed_metrics, "meanDurationMs"),
            _metric_value(native_metrics, "meanDurationMs"),
        ),
        throughput_ratio=_ratio(
            _metric_value(native_metrics, "meanOpsPerSecond"),
            _metric_value(managed_metrics, "meanOpsPerSecond"),
        ),
    )
    interpreter_overhead = _derived_ratio(
        latency_ratio=_ratio(
            _metric_value(interpreter_metrics, "meanDurationMs"),
            _metric_value(managed_metrics, "meanDurationMs"),
        ),
        throughput_ratio=_ratio(
            _metric_value(managed_metrics, "meanOpsPerSecond"),
            _metric_value(interpreter_metrics, "meanOpsPerSecond"),
        ),
    )
    return {
        "managedBaseline": _baseline_metric_payload(dict(mode_status.get("managed") or {"mode": "managed"})),
        "relativeToManaged": {
            "native": _relative_to_managed_payload(mode="native", mode_status=mode_status),
            "interpreter": _relative_to_managed_payload(mode="interpreter", mode_status=mode_status),
        },
        "nativeSpeedup": native_speedup,
        "interpreterOverhead": interpreter_overhead,
    }


def _build_platform_summary(
    *,
    subject_id: str,
    display_name: str,
    platform_key: str,
    by_device: dict[str, dict[str, Any]],
    comparisons: dict[str, Any],
    supported_modes: list[str],
) -> dict[str, Any]:
    device_id = _choose_device_for_platform(by_device, platform_key)
    latest_by_mode = dict(by_device.get(device_id, {})) if device_id else {}
    mode_status: dict[str, dict[str, Any]] = {}
    effective_supported_modes = _effective_supported_modes(supported_modes, latest_by_mode)

    recorded_modes = _sort_modes([mode for mode in latest_by_mode if mode in effective_supported_modes])
    missing_modes = _sort_modes([mode for mode in effective_supported_modes if mode not in latest_by_mode])
    unsupported_modes = _sort_modes([mode for mode in _MODE_ORDER if mode not in effective_supported_modes])

    for mode in _MODE_ORDER:
        if mode in latest_by_mode:
            mode_status[mode] = _record_status_payload(mode, latest_by_mode[mode], device_id or "unknown")
        elif mode in effective_supported_modes:
            mode_status[mode] = {
                "mode": mode,
                "status": "missing",
                **_mode_reason_payload(status="missing", declared_by="subject manifest"),
            }
        else:
            mode_status[mode] = {
                "mode": mode,
                "status": "unsupported",
                **_mode_reason_payload(status="unsupported", declared_by="subject manifest"),
            }

    stale_modes = _mark_stale_record_entries(mode_status)
    latest_record = max(list(latest_by_mode.values()), key=_record_sort_key, default=None)
    latest_ts = str(latest_record.get("recordedAt") or "") if latest_record else ""
    latest_dt = _parse_timestamp(latest_ts)
    is_stale = False
    if latest_dt is not None:
        age_days = (datetime.now(timezone.utc) - latest_dt.astimezone(timezone.utc)).total_seconds() / 86400.0
        is_stale = age_days > _STALE_AFTER_DAYS
    if stale_modes:
        is_stale = True

    comparison_payload = dict(comparisons.get(device_id) or {}) if device_id else {}
    device_name = ""
    if latest_record is not None:
        device_name = str(dict(latest_record.get("device") or {}).get("name") or device_id or "")

    return {
        "subjectId": subject_id,
        "displayName": display_name,
        "platformId": platform_key,
        "deviceId": device_id,
        "deviceName": device_name,
        "supportedModes": effective_supported_modes,
        "recordedModes": recorded_modes,
        "missingModes": missing_modes,
        "staleModes": stale_modes,
        "unsupportedModes": unsupported_modes,
        "modeStatus": mode_status,
        "coverage": {
            "supportedModeCount": len(effective_supported_modes),
            "recordedModeCount": len(recorded_modes),
            "missingModeCount": len(missing_modes),
            "staleModeCount": len(stale_modes),
            "unsupportedModeCount": len(unsupported_modes),
            "isComplete": len(effective_supported_modes) > 0 and len(missing_modes) == 0 and len(stale_modes) == 0,
            "needsAttention": bool(missing_modes or stale_modes),
        },
        "lastRecordedAt": latest_ts or None,
        "gitCommit": (latest_record or {}).get("gitCommit"),
        "isStale": is_stale,
        "comparison": comparison_payload.get("comparison"),
        "verdict": comparison_payload.get("verdict"),
        "keyMetrics": _key_metrics_from_mode_status(mode_status),
    }


def _build_case_summary(
    *,
    case_id: str,
    case_payload: dict[str, Any],
    platform_supported_modes: list[str],
) -> dict[str, Any]:
    meta = dict(case_payload.get("meta") or {})
    latest_by_mode = dict(case_payload.get("records") or {})
    declared_supported_modes = _sort_modes(
        [
            mode
            for mode in list(meta.get("supportedModes") or [])
            if not platform_supported_modes or mode in platform_supported_modes
        ]
    )
    supported_modes = _effective_supported_modes(
        declared_supported_modes or platform_supported_modes,
        latest_by_mode,
    )
    mode_status: dict[str, dict[str, Any]] = {}

    recorded_modes = _sort_modes([mode for mode in latest_by_mode if mode in supported_modes])
    missing_modes = _sort_modes([mode for mode in supported_modes if mode not in latest_by_mode])
    unsupported_modes = _sort_modes([mode for mode in _MODE_ORDER if mode not in supported_modes])

    for mode in _MODE_ORDER:
        if mode in latest_by_mode:
            mode_status[mode] = _record_status_payload(mode, latest_by_mode[mode], str(meta.get("deviceId") or "unknown"))
        elif mode in supported_modes:
            mode_status[mode] = {
                "mode": mode,
                "status": "missing",
                **_mode_reason_payload(status="missing", declared_by="case contract"),
            }
        else:
            mode_status[mode] = {
                "mode": mode,
                "status": "unsupported",
                **_mode_reason_payload(status="unsupported", declared_by="case contract"),
            }

    stale_modes = _mark_stale_record_entries(mode_status)
    return {
        "caseId": case_id,
        "displayName": str(meta.get("displayName") or meta.get("alias") or case_id),
        "alias": str(meta.get("alias") or case_id),
        "workloadEntry": str(meta.get("workloadEntry") or ""),
        "entryIndex": int(meta.get("entryIndex") or 0),
        "assemblyName": str(meta.get("assemblyName") or ""),
        "declaringType": str(meta.get("declaringType") or ""),
        "methodName": str(meta.get("methodName") or ""),
        "methodSignature": str(meta.get("methodSignature") or ""),
        "category": int(meta.get("category") or 0),
        "categoryLabel": str(meta.get("categoryLabel") or "Uncategorized"),
        "metrics": int(meta.get("metrics") or 0),
        "metricLabels": list(meta.get("metricLabels") or []),
        "requires": int(meta.get("requires") or 0),
        "requirementLabels": list(meta.get("requirementLabels") or []),
        "archetype": int(meta.get("archetype") or 0),
        "archetypeLabel": str(meta.get("archetypeLabel") or declared_metadata_labels_module.archetype_label(0)),
        "hotUpdateCapability": int(meta.get("hotUpdateCapability") or 0),
        "hotUpdateCapabilityLabels": list(meta.get("hotUpdateCapabilityLabels") or []),
        "capabilityFamily": int(meta.get("capabilityFamily") or 0),
        "capabilityFamilyLabel": str(meta.get("capabilityFamilyLabel") or ""),
        "capabilityItem": int(meta.get("capabilityItem") or 0),
        "capabilityItemLabel": str(meta.get("capabilityItemLabel") or ""),
        "ownerSubjectId": str(meta.get("ownerSubjectId") or ""),
        "supportStates": [int(value) for value in list(meta.get("supportStates") or [])],
        "supportStateLabels": [str(value) for value in list(meta.get("supportStateLabels") or [])],
        "proofRequired": bool(meta.get("proofRequired", False)),
        "benchmarkRequired": bool(meta.get("benchmarkRequired", False)),
        "warmupCount": int(meta.get("warmupCount") or 0),
        "iterationCount": int(meta.get("iterationCount") or 0),
        "invocationCount": int(meta.get("invocationCount") or 0),
        "supportedModes": supported_modes,
        "recordedModes": recorded_modes,
        "missingModes": missing_modes,
        "staleModes": stale_modes,
        "unsupportedModes": unsupported_modes,
        "modeStatus": mode_status,
        "isStale": bool(stale_modes),
        "coverage": {
            "supportedModeCount": len(supported_modes),
            "recordedModeCount": len(recorded_modes),
            "missingModeCount": len(missing_modes),
            "staleModeCount": len(stale_modes),
            "unsupportedModeCount": len(unsupported_modes),
            "isComplete": len(supported_modes) > 0 and len(missing_modes) == 0 and len(stale_modes) == 0,
            "needsAttention": bool(missing_modes or stale_modes),
        },
        "keyMetrics": _key_metrics_from_mode_status(mode_status),
    }


def _summarize_case_entries(entries: list[dict[str, Any]]) -> dict[str, Any]:
    cross_mode_count = sum(1 for entry in entries if len(list(entry.get("supportedModes") or [])) >= 2)
    managed_only_count = sum(1 for entry in entries if list(entry.get("supportedModes") or []) == ["managed"])
    stale_case_count = sum(1 for entry in entries if bool(entry.get("isStale")))
    missing_case_count = sum(1 for entry in entries if bool(entry.get("missingModes")))
    return {
        "caseCount": len(entries),
        "crossModeCaseCount": cross_mode_count,
        "managedOnlyCaseCount": managed_only_count,
        "fullyRecordedCaseCount": sum(1 for entry in entries if bool(dict(entry.get("coverage") or {}).get("isComplete"))),
        "missingCaseCount": missing_case_count,
        "staleCaseCount": stale_case_count,
        "attentionCaseCount": sum(1 for entry in entries if bool(entry.get("missingModes")) or bool(entry.get("isStale"))),
    }


def _summarize_subject_entries(entries: list[dict[str, Any]]) -> dict[str, Any]:
    latest_recorded_at = None
    latest_recorded_dt = None

    for entry in entries:
        current = _parse_timestamp(str(entry.get("lastRecordedAt") or ""))
        if current is None:
            continue
        if latest_recorded_dt is None or current > latest_recorded_dt:
            latest_recorded_dt = current
            latest_recorded_at = entry.get("lastRecordedAt")

    return {
        "subjectCount": len(entries),
        "fullyRecordedCount": sum(1 for entry in entries if bool(dict(entry.get("coverage") or {}).get("isComplete"))),
        "subjectsWithMissingRecords": sum(1 for entry in entries if bool(entry.get("missingModes"))),
        "staleSubjectCount": sum(1 for entry in entries if bool(entry.get("isStale"))),
        "recordedModeCount": sum(len(list(entry.get("recordedModes") or [])) for entry in entries),
        "missingModeCount": sum(len(list(entry.get("missingModes") or [])) for entry in entries),
        "unsupportedModeCount": sum(len(list(entry.get("unsupportedModes") or [])) for entry in entries),
        "latestRecordedAt": latest_recorded_at,
    }


def _load_json_if_exists(path: Path) -> dict[str, Any] | None:
    if not path.is_file():
        return None
    try:
        payload = json.loads(path.read_text(encoding="utf-8"))
    except (OSError, json.JSONDecodeError):
        return None
    return dict(payload) if isinstance(payload, dict) else None


def _load_formal_source(repo_root: Path) -> dict[str, Any] | None:
    master_root = verification_layout_module.archive_master_root(repo_root)
    if not master_root.is_dir():
        return None
    payload = {
        "closure": _load_json_if_exists(master_root / "closure-master.json"),
        "capability": _load_json_if_exists(master_root / "capability-master.json"),
        "evidenceClaim": _load_json_if_exists(master_root / "evidence-claims-master.json"),
        "stage": _load_json_if_exists(master_root / "stage-master.json"),
        "result": _load_json_if_exists(master_root / "result-master.json"),
    }
    if not any(value for value in payload.values()):
        return None
    # Only consider formal source usable if at least one table has non-empty items
    if not any(
        isinstance(value, dict) and len(list(value.get("items") or [])) > 0
        for value in payload.values()
    ):
        return None
    return payload


def _projection_mode_status(row: dict[str, Any]) -> dict[str, dict[str, Any]]:
    raw = dict(row.get("modeStatus") or {})
    if raw:
        return {
            mode: dict(entry or {})
            for mode, entry in raw.items()
        }

    result: dict[str, dict[str, Any]] = {}
    for mode in _MODE_ORDER:
        status = str(row.get(f"{mode}Status") or "").strip()
        entry = {
            "mode": mode,
            "status": status or "unsupported",
            "deviceId": str(row.get("deviceId") or "unknown"),
            "deviceName": str(row.get("deviceName") or row.get("deviceId") or ""),
            "recordedAt": row.get("lastRecordedAt"),
            "gitCommit": row.get("gitCommit"),
            "metrics": {},
            "isStale": bool(row.get("isStale", False)),
            "staleReasonCode": "",
            "staleReasonLabel": "",
        }
        duration_key = f"{mode}MeanDurationMs"
        ops_key = f"{mode}OpsPerSecond"
        metrics: dict[str, Any] = {}
        if isinstance(row.get(duration_key), (int, float)):
            metrics["meanDurationMs"] = float(row[duration_key])
        if isinstance(row.get(ops_key), (int, float)):
            metrics["meanOpsPerSecond"] = float(row[ops_key])
        if metrics:
            entry["metrics"] = metrics
        if status == "recorded":
            entry.update(_mode_reason_payload(status="recorded", declared_by="formal projection"))
        elif status == "missing":
            entry.update(_mode_reason_payload(status="missing", declared_by="formal projection"))
        else:
            entry.update(_mode_reason_payload(status="unsupported", declared_by="formal projection"))
        result[mode] = entry
    return result


def _projection_case_payload(row: dict[str, Any]) -> dict[str, Any]:
    mode_status = _projection_mode_status(row)
    supported_modes = _sort_modes(list(row.get("supportedModes") or []))
    recorded_modes = _sort_modes(
        [
            mode
            for mode, entry in mode_status.items()
            if str(entry.get("status") or "") == "recorded"
        ]
    )
    missing_modes = _sort_modes(
        [
            mode
            for mode, entry in mode_status.items()
            if str(entry.get("status") or "") == "missing"
        ]
    )
    stale_modes = _sort_modes(
        [
            mode
            for mode, entry in mode_status.items()
            if bool(entry.get("isStale", False))
        ]
    )
    unsupported_modes = _sort_modes(
        [
            mode
            for mode, entry in mode_status.items()
            if str(entry.get("status") or "") == "unsupported"
        ]
    )
    return {
        "caseId": str(row.get("stableId") or ""),
        "displayName": str(row.get("displayName") or row.get("alias") or row.get("stableId") or ""),
        "alias": str(row.get("alias") or row.get("stableId") or ""),
        "workloadEntry": str(row.get("workloadEntry") or row.get("method") or ""),
        "entryIndex": int(row.get("entryIndex") or 0),
        "assemblyName": str(row.get("assemblyName") or ""),
        "declaringType": str(row.get("declaringType") or ""),
        "methodName": str(row.get("methodName") or ""),
        "methodSignature": str(row.get("methodSignature") or ""),
        "category": int(row.get("category") or 0),
        "categoryLabel": str(row.get("categoryLabel") or "Uncategorized"),
        "metrics": int(row.get("metrics") or 0),
        "metricLabels": list(row.get("metricLabels") or []),
        "requires": int(row.get("requires") or 0),
        "requirementLabels": list(row.get("requirementLabels") or []),
        "archetype": int(row.get("archetype") or 0),
        "archetypeLabel": str(row.get("archetypeLabel") or declared_metadata_labels_module.archetype_label(0)),
        "hotUpdateCapability": int(row.get("hotUpdateCapability") or 0),
        "hotUpdateCapabilityLabels": list(row.get("hotUpdateCapabilityLabels") or []),
        "capabilityFamily": int(row.get("capabilityFamily") or 0),
        "capabilityFamilyLabel": str(row.get("capabilityFamilyLabel") or ""),
        "capabilityItem": int(row.get("capabilityItem") or 0),
        "capabilityItemLabel": str(row.get("capabilityItemLabel") or ""),
        "ownerSubjectId": str(row.get("ownerSubjectId") or ""),
        "supportStates": [int(value) for value in list(row.get("supportStates") or [])],
        "supportStateLabels": [str(value) for value in list(row.get("supportStateLabels") or [])],
        "proofRequired": bool(row.get("proofRequired", False)),
        "benchmarkRequired": bool(row.get("benchmarkRequired", False)),
        "warmupCount": int(row.get("warmupCount") or 0),
        "iterationCount": int(row.get("iterationCount") or 0),
        "invocationCount": int(row.get("invocationCount") or 0),
        "supportedModes": supported_modes,
        "recordedModes": recorded_modes,
        "missingModes": missing_modes,
        "staleModes": stale_modes,
        "unsupportedModes": unsupported_modes,
        "modeStatus": mode_status,
        "isStale": bool(stale_modes or row.get("isStale", False)),
        "coverage": {
            "supportedModeCount": len(supported_modes),
            "recordedModeCount": len(recorded_modes),
            "missingModeCount": len(missing_modes),
            "staleModeCount": len(stale_modes),
            "unsupportedModeCount": len(unsupported_modes),
            "isComplete": len(supported_modes) > 0 and len(missing_modes) == 0 and len(stale_modes) == 0,
            "needsAttention": bool(missing_modes or stale_modes),
        },
        "keyMetrics": _key_metrics_from_mode_status(mode_status),
        "lastRecordedAt": str(row.get("lastRecordedAt") or ""),
        "gitCommit": str(row.get("gitCommit") or ""),
        "deviceId": str(row.get("deviceId") or ""),
        "deviceName": str(row.get("deviceName") or row.get("deviceId") or ""),
        "platformId": str(row.get("platformId") or ""),
        "stableId": str(row.get("stableId") or ""),
    }


def _collect_data_from_formal_source(formal_source: dict[str, Any]) -> dict[str, Any]:
    verification_projection_module = _import_verification_projection_module()
    projected = verification_projection_module.project_inventory_tables(formal_source)
    benchmark_rows = list(projected.get("benchmark") or [])
    data: dict[str, Any] = {}

    for row in benchmark_rows:
        subject_id = str(row.get("subjectId") or row.get("ownerSubjectId") or "").strip()
        if not subject_id:
            continue
        device_id = str(row.get("deviceId") or "").strip()
        case_id = str(row.get("stableId") or "").strip()
        subject_payload = data.setdefault(
            subject_id,
            {
                "subjectId": subject_id,
                "displayName": str(row.get("subjectDisplayName") or subject_id),
                "summaryWorkloadEntry": str(row.get("subjectSummaryWorkloadEntry") or ""),
                "summaryBenchmarkCase": dict(row.get("subjectSummaryBenchmarkCase") or {}),
                "supportedModesByPlatform": dict(row.get("subjectSupportedModesByPlatform") or {}),
                "availablePlatforms": [],
                "platforms": dict(row.get("subjectPlatforms") or {}),
                "latestByDevice": dict(row.get("subjectLatestByDevice") or {}),
                "benchmarkCasesByDevice": {},
                "declaredBenchmarkCases": {},
                "caseSummaryByDevice": {},
                "history": {},
                "comparisons": {},
            },
        )
        if not subject_payload["platforms"] and isinstance(row.get("subjectPlatforms"), dict):
            subject_payload["platforms"] = dict(row.get("subjectPlatforms") or {})
        if not subject_payload["latestByDevice"] and isinstance(row.get("subjectLatestByDevice"), dict):
            subject_payload["latestByDevice"] = dict(row.get("subjectLatestByDevice") or {})
        if not subject_payload["supportedModesByPlatform"] and isinstance(row.get("subjectSupportedModesByPlatform"), dict):
            subject_payload["supportedModesByPlatform"] = dict(row.get("subjectSupportedModesByPlatform") or {})
        if not subject_payload["summaryWorkloadEntry"]:
            subject_payload["summaryWorkloadEntry"] = str(row.get("subjectSummaryWorkloadEntry") or "")
        if not subject_payload["summaryBenchmarkCase"] and isinstance(row.get("subjectSummaryBenchmarkCase"), dict):
            subject_payload["summaryBenchmarkCase"] = dict(row.get("subjectSummaryBenchmarkCase") or {})

        case_payload = _projection_case_payload(row)
        if device_id:
            subject_payload["benchmarkCasesByDevice"].setdefault(device_id, {})[case_id] = case_payload
        subject_payload["declaredBenchmarkCases"][case_id] = _summary_benchmark_case_payload(case_payload)

    for subject_id, subject_payload in data.items():
        benchmark_cases_by_device = dict(subject_payload.get("benchmarkCasesByDevice") or {})
        declared_cases = dict(subject_payload.get("declaredBenchmarkCases") or {})
        subject_payload["caseSummaryByDevice"] = {
            device_id: _summarize_case_entries(list(cases.values()))
            for device_id, cases in benchmark_cases_by_device.items()
        }
        if not subject_payload["summaryWorkloadEntry"]:
            subject_payload["summaryWorkloadEntry"] = _resolve_summary_workload_entry(
                {},
                benchmark_cases_by_device=benchmark_cases_by_device,
                declared_cases=declared_cases,
            )
        if not subject_payload["summaryBenchmarkCase"]:
            summary_case = _find_summary_benchmark_case(
                str(subject_payload.get("summaryWorkloadEntry") or ""),
                benchmark_cases_by_device=benchmark_cases_by_device,
                declared_cases=declared_cases,
            )
            subject_payload["summaryBenchmarkCase"] = summary_case or {}
        if not subject_payload["availablePlatforms"]:
            platforms = dict(subject_payload.get("platforms") or {})
            if platforms:
                subject_payload["availablePlatforms"] = sorted(platforms)
            else:
                subject_payload["availablePlatforms"] = sorted(
                    {
                        str(case.get("platformId") or "")
                        for cases in benchmark_cases_by_device.values()
                        for case in cases.values()
                        if str(case.get("platformId") or "")
                    }
                )
        if not subject_payload["latestByDevice"]:
            subject_payload["latestByDevice"] = {
                device_id: {
                    case_id: {
                        "modeStatus": dict(case_payload.get("modeStatus") or {}),
                        "lastRecordedAt": case_payload.get("lastRecordedAt"),
                        "gitCommit": case_payload.get("gitCommit"),
                    }
                    for case_id, case_payload in cases.items()
                }
                for device_id, cases in benchmark_cases_by_device.items()
            }
        if not subject_payload["summaryBenchmarkCase"]:
            subject_payload["summaryBenchmarkCase"] = None
        else:
            subject_payload["summaryBenchmarkCase"] = dict(subject_payload["summaryBenchmarkCase"])

    return data


def _collect_data(repo_root: Path, subject_ids: list[str] | None = None) -> dict[str, Any]:
    comparison_mod = _load("benchmark_comparison", _testing_support_path(repo_root, "benchmark_comparison.py"))
    subjects_mod = _load("subjects", _testing_support_path(repo_root, "subjects.py"))

    if subject_ids is None:
        subject_ids = subjects_mod.discover_perf_subject_ids(repo_root)

    data: dict[str, Any] = {}

    for subject_id in subject_ids:
        manifest = subjects_mod.load_subject_manifest(repo_root, subject_id)
        display_name = str(manifest.get("displayName") or subject_id)
        records_path = verification_layout_module.raw_benchmark_records_path(repo_root, subject_id)
        declared_cases = _load_declared_benchmark_cases(repo_root, subject_id)
        declared_case_lookup = _build_declared_case_lookup(declared_cases)

        seen_pairs: set[tuple[str, str]] = set()
        seen_case_pairs: set[tuple[str, str]] = set()
        by_device: dict[str, dict[str, Any]] = {}
        case_fallback_by_device: dict[str, dict[str, Any]] = {}
        history: dict[str, dict[str, list[Any]]] = {}
        case_records_by_device: dict[str, dict[str, dict[str, Any]]] = {}

        if records_path.exists():
            for record in _iter_jsonl_reverse(records_path):
                mode = str(record.get("mode") or "")
                device_id = str(dict(record.get("device") or {}).get("id") or "unknown")
                pair = (mode, device_id)

                benchmark_case = dict(record.get("benchmarkCase") or {})
                case_id = str(benchmark_case.get("stableId") or benchmark_case.get("alias") or "").strip()
                if case_id:
                    resolved_case_id = _resolve_declared_case_id(
                        benchmark_case,
                        declared_cases=declared_cases,
                        declared_case_lookup=declared_case_lookup,
                    )
                    if declared_cases and not resolved_case_id:
                        continue
                    summary_record = _summary_record(record)
                    history.setdefault(device_id, {}).setdefault(mode, [])
                    if len(history[device_id][mode]) < 20:
                        history[device_id][mode].append(summary_record)
                    if pair not in seen_case_pairs:
                        seen_case_pairs.add(pair)
                        case_fallback_by_device.setdefault(device_id, {})[mode] = summary_record
                    canonical_case_id = resolved_case_id or case_id
                    record_case_meta = {
                        "stableId": canonical_case_id,
                        "deviceId": device_id,
                        "alias": str(benchmark_case.get("alias") or case_id),
                        "displayName": str(benchmark_case.get("displayName") or benchmark_case.get("alias") or case_id),
                        "workloadEntry": str(benchmark_case.get("workloadEntry") or ""),
                        "assemblyName": str(benchmark_case.get("assemblyName") or ""),
                        "declaringType": str(benchmark_case.get("declaringType") or ""),
                        "methodName": str(benchmark_case.get("methodName") or ""),
                        "methodSignature": str(benchmark_case.get("methodSignature") or ""),
                        "category": int(benchmark_case.get("category") or 0),
                        "categoryLabel": declared_metadata_labels_module.benchmark_category_label(
                            benchmark_case.get("category")
                        ),
                        "metrics": int(benchmark_case.get("metrics") or 0),
                        "metricLabels": _labels_from_mask(
                            benchmark_case.get("metrics"),
                            declared_metadata_labels_module.METRIC_LABELS,
                        ),
                        "modes": int(benchmark_case.get("modes") or 0),
                        "supportedModes": list(benchmark_case.get("supportedModes") or _supported_modes_from_mask(benchmark_case.get("modes"))),
                        "requires": int(benchmark_case.get("requires") or 0),
                        "requirementLabels": _labels_from_mask(
                            benchmark_case.get("requires"),
                            declared_metadata_labels_module.RUNTIME_FEATURE_LABELS,
                        ),
                        "archetype": int(benchmark_case.get("archetype") or 0),
                        "archetypeLabel": declared_metadata_labels_module.archetype_label(
                            benchmark_case.get("archetype")
                        ),
                        "hotUpdateCapability": int(benchmark_case.get("hotUpdateCapability") or 0),
                        "hotUpdateCapabilityLabels": _labels_from_mask(
                            benchmark_case.get("hotUpdateCapability"),
                            declared_metadata_labels_module.HOT_UPDATE_CAPABILITY_LABELS,
                        ),
                        "warmupCount": int(benchmark_case.get("warmupCount") or 0),
                        "iterationCount": int(benchmark_case.get("iterationCount") or 0),
                        "invocationCount": int(benchmark_case.get("invocationCount") or 0),
                    }
                    record_case_meta.update(_capability_contract_payload(benchmark_case))
                    entry_index = benchmark_case.get("entryIndex")
                    if isinstance(entry_index, int) and not isinstance(entry_index, bool) and entry_index >= 0:
                        record_case_meta["entryIndex"] = int(entry_index)
                    case_payload = case_records_by_device.setdefault(device_id, {}).setdefault(
                        canonical_case_id,
                        {
                            "meta": {},
                            "records": {},
                        },
                    )
                    case_payload["meta"] = _merge_case_meta(
                        case_payload["meta"],
                        _merge_case_meta(record_case_meta, dict(declared_cases.get(canonical_case_id) or {})),
                    )
                    if mode and mode not in case_payload["records"]:
                        case_payload["records"][mode] = summary_record
                    continue
                summary_record = _summary_record(record)
                history.setdefault(device_id, {}).setdefault(mode, [])
                if len(history[device_id][mode]) < 20:
                    history[device_id][mode].append(summary_record)
                if pair not in seen_pairs:
                    seen_pairs.add(pair)
                    by_device.setdefault(device_id, {})[mode] = summary_record

        comparisons: dict[str, Any] = {}
        for device_id, modes in case_fallback_by_device.items():
            merged_modes = by_device.setdefault(device_id, {})
            for mode, record in modes.items():
                merged_modes.setdefault(mode, record)

        for device_id, modes in by_device.items():
            comparison = comparison_mod.compute_comparison(
                dict(dict(modes.get("managed") or {}).get("metrics") or {}),
                dict(dict(modes.get("native") or {}).get("metrics") or {}),
                dict(dict(modes.get("interpreter") or {}).get("metrics") or {}),
            )
            comparisons[device_id] = {
                "comparison": comparison,
                "verdict": comparison_mod.evaluate_targets(comparison),
            }

        platforms = sorted(
            set(_manifest_platforms(manifest))
            | {
                _device_platform_key(device_id, modes=modes)
                for device_id, modes in by_device.items()
            }
        )
        platform_summaries: dict[str, Any] = {}
        supported_modes_by_platform: dict[str, list[str]] = {}
        for platform_key in platforms:
            if not platform_key:
                continue
            supported_modes = _supported_modes_for_platform(
                manifest,
                platform_key=platform_key,
            )
            supported_modes_by_platform[platform_key] = supported_modes
            platform_summaries[platform_key] = _build_platform_summary(
                subject_id=subject_id,
                display_name=display_name,
                platform_key=platform_key,
                by_device=by_device,
                comparisons=comparisons,
                supported_modes=supported_modes,
            )

        for device_id in set(by_device) | set(case_records_by_device):
            cases = case_records_by_device.setdefault(device_id, {})
            for case_id, declared_case_meta in declared_cases.items():
                case_payload = cases.setdefault(case_id, {"meta": {}, "records": {}})
                case_payload["meta"] = _merge_case_meta(
                    case_payload["meta"],
                    _merge_case_meta(dict(declared_case_meta), {"deviceId": device_id}),
                )

        benchmark_cases_by_device: dict[str, dict[str, Any]] = {}
        for device_id, cases in case_records_by_device.items():
            platform_key = _device_platform_key(device_id, cases=cases)
            platform_supported_modes = supported_modes_by_platform.get(platform_key) or _sort_modes(
                [mode for mode in _MODE_ORDER if any(mode in dict(case_payload.get("records") or {}) for case_payload in cases.values())]
            )
            benchmark_cases_by_device[device_id] = {
                case_id: _build_case_summary(
                    case_id=case_id,
                    case_payload=case_payload,
                    platform_supported_modes=platform_supported_modes,
                )
                for case_id, case_payload in sorted(cases.items())
            }

        summary_workload_entry = _resolve_summary_workload_entry(
            manifest,
            benchmark_cases_by_device=benchmark_cases_by_device,
            declared_cases=declared_cases,
        )
        summary_benchmark_case = _find_summary_benchmark_case(
            summary_workload_entry,
            benchmark_cases_by_device=benchmark_cases_by_device,
            declared_cases=declared_cases,
        )

        data[subject_id] = {
            "subjectId": subject_id,
            "displayName": display_name,
            "summaryWorkloadEntry": summary_workload_entry,
            "summaryBenchmarkCase": summary_benchmark_case,
            "supportedModesByPlatform": supported_modes_by_platform,
            "availablePlatforms": sorted(platform_summaries),
            "platforms": platform_summaries,
            "latestByDevice": by_device,
            "benchmarkCasesByDevice": benchmark_cases_by_device,
            "declaredBenchmarkCases": declared_cases,
            "caseSummaryByDevice": {
                device_id: _summarize_case_entries(list(cases.values()))
                for device_id, cases in benchmark_cases_by_device.items()
            },
            "history": history,
            "comparisons": comparisons,
        }

    return data


def collect_dashboard_data(repo_root: Path, subject_ids: list[str] | None = None) -> dict[str, Any]:
    return _collect_data(repo_root, subject_ids)


def _iter_jsonl_reverse(path: Path):
    try:
        lines = path.read_text(encoding="utf-8").splitlines()
    except OSError:
        return
    for raw in reversed(lines):
        raw = raw.strip()
        if not raw:
            continue
        try:
            yield json.loads(raw)
        except json.JSONDecodeError:
            continue


def _build_overview(data: dict[str, Any], default_platform_prefix: str = _DEFAULT_PLATFORM) -> dict[str, Any]:
    subjects_out: dict[str, Any] = {}
    platform_entries: dict[str, list[dict[str, Any]]] = {}
    chosen_entries: list[dict[str, Any]] = []

    for subject_id, subject_data in data.items():
        platforms = dict(subject_data.get("platforms") or {})
        if not platforms:
            continue

        selected_platform = default_platform_prefix if default_platform_prefix in platforms else sorted(platforms)[0]
        selected_entry = dict(platforms[selected_platform])
        selected_device_id = str(selected_entry.get("deviceId") or "")
        subject_entry = {
            "subjectId": subject_id,
            "displayName": subject_data.get("displayName") or subject_id,
            "defaultPlatform": selected_platform,
            "availablePlatforms": list(subject_data.get("availablePlatforms") or []),
            "summaryWorkloadEntry": str(subject_data.get("summaryWorkloadEntry") or ""),
            "summaryBenchmarkCase": dict(subject_data.get("summaryBenchmarkCase") or {}),
            "caseSummary": dict((subject_data.get("caseSummaryByDevice") or {}).get(selected_device_id) or {}),
            "platforms": platforms,
            **selected_entry,
        }
        subjects_out[subject_id] = subject_entry
        chosen_entries.append(subject_entry)

        for platform_id, platform_entry in platforms.items():
            platform_entries.setdefault(platform_id, []).append(dict(platform_entry))

    platform_summaries = {
        platform_id: _summarize_subject_entries(entries)
        for platform_id, entries in sorted(platform_entries.items())
    }

    return {
        "generatedAt": datetime.now(timezone.utc).isoformat(),
        "defaultPlatform": default_platform_prefix,
        "availablePlatforms": sorted(platform_summaries),
        "modeOrder": list(_MODE_ORDER),
        "summary": _summarize_subject_entries(chosen_entries),
        "platformSummaries": platform_summaries,
        "subjects": subjects_out,
    }


def _dashboard_payload(data: dict[str, Any], overview: dict[str, Any]) -> dict[str, Any]:
    return {
        "generatedAt": overview["generatedAt"],
        "overview": overview,
        "subjects": data,
    }


def build_dashboard_overview(
    data: dict[str, Any],
    default_platform_prefix: str = _DEFAULT_PLATFORM,
) -> dict[str, Any]:
    return _build_overview(data, default_platform_prefix=default_platform_prefix)


def generate(repo_root: Path, output_path: Path, subject_ids: list[str] | None = None) -> None:
    data = _resolve_dashboard_data(repo_root, subject_ids=subject_ids)
    overview = _build_overview(data)
    template = _read_template(_testing_support_path(repo_root, "templates", "benchmark-dashboard.html"))
    payload = _dashboard_payload(data, overview)
    html = template.replace(
        "/* BENCHMARK_DATA_PLACEHOLDER */",
        f"window.BENCHMARK_DATA = {json.dumps(payload, ensure_ascii=False, default=str)};",
    )

    output_path.parent.mkdir(parents=True, exist_ok=True)
    output_path.write_text(html, encoding="utf-8")


def _resolve_dashboard_data(
    repo_root: Path,
    subject_ids: list[str] | None = None,
) -> dict[str, Any]:
    """Resolve dashboard data preferring formal source when it has benchmark content."""
    formal_source = _load_formal_source(repo_root)
    if formal_source is not None:
        projections_module = _import_verification_projection_module()
        projected = projections_module.project_inventory_tables(formal_source)
        benchmark_rows = list(projected.get("benchmark") or [])
        if benchmark_rows:
            return _collect_data_from_formal_source(formal_source)

    # Fall back to raw records when formal source has no benchmark data
    subjects_mod = _load("subjects", _testing_support_path(repo_root, "subjects.py"))
    resolved_ids = subject_ids if subject_ids is not None else subjects_mod.discover_perf_subject_ids(repo_root)
    return _collect_data(repo_root, resolved_ids)


def update_docs(repo_root: Path, subject_id: str | None = None) -> None:
    docs_root = verification_layout_module.benchmark_projection_root(repo_root)
    subjects_doc_root = docs_root / "subjects"
    subjects_doc_root.mkdir(parents=True, exist_ok=True)

    del subject_id
    data = _resolve_dashboard_data(repo_root)
    overview = _build_overview(data)

    for stale_path in list(subjects_doc_root.glob("*.json")):
        try:
            stale_path.unlink()
        except PermissionError:
            # Some sandboxed environments deny deletes but still allow the
            # current subject payloads to be rewritten in place.
            continue

    for current_subject_id, subject_payload in data.items():
        (subjects_doc_root / f"{current_subject_id}.json").write_text(
            json.dumps(subject_payload, ensure_ascii=False, indent=2, default=str),
            encoding="utf-8",
        )

    (docs_root / "overview.json").write_text(
        json.dumps(overview, ensure_ascii=False, indent=2, default=str),
        encoding="utf-8",
    )

    template = _read_template(_testing_support_path(repo_root, "templates", "benchmark-dashboard.html"))
    payload = _dashboard_payload(data, overview)
    html = template.replace(
        "/* BENCHMARK_DATA_PLACEHOLDER */",
        f"window.BENCHMARK_DATA = {json.dumps(payload, ensure_ascii=False, default=str)};",
    )
    (docs_root / "dashboard.html").write_text(html, encoding="utf-8")


def _read_template(path: Path) -> str:
    if not path.exists():
        _write_default_template(path)
    return path.read_text(encoding="utf-8")


def _write_default_template(path: Path) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(_DEFAULT_HTML, encoding="utf-8")


_DEFAULT_HTML = """<!DOCTYPE html>
<html lang=\\"en\\">
<head><meta charset=\\"UTF-8\\"><meta name=\\"viewport\\" content=\\"width=device-width, initial-scale=1.0\\"><title>IL2CPP Benchmark Dashboard</title></head>
<body><script>/* BENCHMARK_DATA_PLACEHOLDER */</script></body>
</html>
"""
