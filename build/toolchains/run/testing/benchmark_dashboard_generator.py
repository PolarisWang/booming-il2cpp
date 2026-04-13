"""Generate static benchmark dashboard data and HTML."""
from __future__ import annotations

import importlib.util
import json
from datetime import datetime, timezone
from pathlib import Path
from typing import Any


_MODE_ORDER = ("managed", "native", "interpreter")
_DEFAULT_PLATFORM = "windows-x64"
_STALE_AFTER_DAYS = 7.0
_MODE_FLAGS = {
    "managed": 1 << 0,
    "native": 1 << 1,
    "interpreter": 1 << 2,
}
_ALL_MODE_FLAGS = sum(_MODE_FLAGS.values())
_BENCHMARK_CATEGORY_LABELS = {
    1: "Runtime Dispatch",
    2: "Startup",
    3: "Allocation",
    4: "Hot Update",
}
_METRIC_LABELS = {
    1 << 0: "Wall Clock",
    1 << 1: "Managed Alloc",
    1 << 2: "Native Alloc",
    1 << 3: "Working Set",
}
_RUNTIME_FEATURE_LABELS = {
    1 << 0: "Generic Sharing",
    1 << 1: "Reflection",
    1 << 2: "Delegate",
    1 << 3: "Exception Flow",
    1 << 4: "Native Interop",
    1 << 5: "Hot Update",
}


def _load(name: str, path: Path):
    spec = importlib.util.spec_from_file_location(name, path)
    assert spec and spec.loader
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)  # type: ignore[union-attr]
    return module


def _sort_modes(values: list[str]) -> list[str]:
    order = {mode: index for index, mode in enumerate(_MODE_ORDER)}
    return sorted({str(value) for value in values if str(value)}, key=lambda item: order.get(item, 999))


def _supported_modes_from_mask(value: Any) -> list[str]:
    try:
        mask = int(value or 0)
    except (TypeError, ValueError):
        mask = 0
    if mask <= 0:
        mask = _ALL_MODE_FLAGS
    return [mode for mode in _MODE_ORDER if mask & _MODE_FLAGS[mode]]


def _labels_from_mask(value: Any, labels: dict[int, str]) -> list[str]:
    try:
        mask = int(value or 0)
    except (TypeError, ValueError):
        mask = 0
    return [label for bit, label in labels.items() if mask & bit]


def _normalize_platform_key(value: str) -> str:
    text = str(value or "").strip().lower()
    if not text:
        return ""
    parts = text.split("-")
    if len(parts) >= 2 and parts[0] in {"windows", "linux", "macos"}:
        return "-".join(parts[:2])
    return text


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

    cases: dict[str, dict[str, Any]] = {}
    for payload in list(dict(catalog).get("declaredBenchmarks") or []):
        item = dict(payload or {})
        stable_id = str(item.get("stableId") or "").strip()
        if not stable_id:
            continue
        modes = int(item.get("modes") or 0)
        cases[stable_id] = {
            "stableId": stable_id,
            "alias": str(item.get("alias") or "").strip() or stable_id,
            "displayName": str(item.get("alias") or "").strip() or stable_id,
            "workloadEntry": _declared_source_entry(item),
            "assemblyName": str(item.get("assemblyName") or ""),
            "declaringType": str(item.get("declaringType") or ""),
            "methodName": str(item.get("methodName") or ""),
            "methodSignature": str(item.get("methodSignature") or ""),
            "category": int(item.get("category") or 0),
            "categoryLabel": _BENCHMARK_CATEGORY_LABELS.get(int(item.get("category") or 0), "Uncategorized"),
            "metrics": int(item.get("metrics") or 0),
            "metricLabels": _labels_from_mask(item.get("metrics"), _METRIC_LABELS),
            "modes": modes,
            "supportedModes": _supported_modes_from_mask(modes),
            "requires": int(item.get("requires") or 0),
            "requirementLabels": _labels_from_mask(item.get("requires"), _RUNTIME_FEATURE_LABELS),
            "warmupCount": int(item.get("warmupCount") or 0),
            "iterationCount": int(item.get("iterationCount") or 0),
            "invocationCount": int(item.get("invocationCount") or 0),
        }
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


def _summary_benchmark_case_payload(case_payload: dict[str, Any]) -> dict[str, Any]:
    return {
        "caseId": str(case_payload.get("caseId") or case_payload.get("stableId") or ""),
        "stableId": str(case_payload.get("stableId") or case_payload.get("caseId") or ""),
        "alias": str(case_payload.get("alias") or case_payload.get("displayName") or ""),
        "displayName": str(case_payload.get("displayName") or case_payload.get("alias") or ""),
        "workloadEntry": str(case_payload.get("workloadEntry") or ""),
        "assemblyName": str(case_payload.get("assemblyName") or ""),
        "declaringType": str(case_payload.get("declaringType") or ""),
        "methodName": str(case_payload.get("methodName") or ""),
        "methodSignature": str(case_payload.get("methodSignature") or ""),
        "category": int(case_payload.get("category") or 0),
        "categoryLabel": str(case_payload.get("categoryLabel") or "Uncategorized"),
        "metricLabels": list(case_payload.get("metricLabels") or []),
        "requirementLabels": list(case_payload.get("requirementLabels") or []),
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


def _choose_device_for_platform(by_device: dict[str, dict[str, Any]], platform_key: str) -> str | None:
    best_device_id: str | None = None
    best_score: tuple[int, float, str] = (-1, -1.0, "")

    for device_id, modes in by_device.items():
        if platform_key and _normalize_platform_key(device_id) != platform_key:
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
    return {
        "mode": mode,
        "status": "recorded",
        "deviceId": device_id,
        "deviceName": str(device.get("name") or device_id),
        "recordedAt": record.get("recordedAt"),
        "gitCommit": record.get("gitCommit"),
        "metrics": dict(record.get("metrics") or {}),
    }


def _baseline_metric_payload(entry: dict[str, Any]) -> dict[str, Any]:
    metrics = dict(entry.get("metrics") or {})
    return {
        "mode": str(entry.get("mode") or "managed"),
        "status": str(entry.get("status") or "unsupported"),
        "durationMs": _metric_value(metrics, "meanDurationMs"),
        "opsPerSecond": _metric_value(metrics, "meanOpsPerSecond"),
        "recordedAt": entry.get("recordedAt"),
        "gitCommit": entry.get("gitCommit"),
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

    payload = {
        "mode": mode,
        "status": current_status,
        "direction": "faster" if mode == "native" else "slower",
        "ratio": None,
        "durationMs": None,
        "opsPerSecond": None,
        "recordedAt": current_entry.get("recordedAt"),
        "gitCommit": current_entry.get("gitCommit"),
        "baselineStatus": managed_status,
    }

    if current_status != "recorded":
        return payload
    if managed_status != "recorded":
        payload["status"] = "baseline-unavailable"
        return payload

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

    recorded_modes = _sort_modes([mode for mode in latest_by_mode if mode in supported_modes])
    missing_modes = _sort_modes([mode for mode in supported_modes if mode not in latest_by_mode])
    unsupported_modes = _sort_modes([mode for mode in _MODE_ORDER if mode not in supported_modes])

    for mode in _MODE_ORDER:
        if mode in latest_by_mode:
            mode_status[mode] = _record_status_payload(mode, latest_by_mode[mode], device_id or "unknown")
        elif mode in supported_modes:
            mode_status[mode] = {"mode": mode, "status": "missing"}
        else:
            mode_status[mode] = {"mode": mode, "status": "unsupported"}

    latest_record = max(list(latest_by_mode.values()), key=_record_sort_key, default=None)
    latest_ts = str(latest_record.get("recordedAt") or "") if latest_record else ""
    latest_dt = _parse_timestamp(latest_ts)
    is_stale = False
    if latest_dt is not None:
        age_days = (datetime.now(timezone.utc) - latest_dt.astimezone(timezone.utc)).total_seconds() / 86400.0
        is_stale = age_days > _STALE_AFTER_DAYS

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
        "supportedModes": supported_modes,
        "recordedModes": recorded_modes,
        "missingModes": missing_modes,
        "unsupportedModes": unsupported_modes,
        "modeStatus": mode_status,
        "coverage": {
            "supportedModeCount": len(supported_modes),
            "recordedModeCount": len(recorded_modes),
            "missingModeCount": len(missing_modes),
            "unsupportedModeCount": len(unsupported_modes),
            "isComplete": len(supported_modes) > 0 and len(missing_modes) == 0,
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
    supported_modes = declared_supported_modes or platform_supported_modes
    mode_status: dict[str, dict[str, Any]] = {}

    recorded_modes = _sort_modes([mode for mode in latest_by_mode if mode in supported_modes])
    missing_modes = _sort_modes([mode for mode in supported_modes if mode not in latest_by_mode])
    unsupported_modes = _sort_modes([mode for mode in _MODE_ORDER if mode not in supported_modes])

    for mode in _MODE_ORDER:
        if mode in latest_by_mode:
            mode_status[mode] = _record_status_payload(mode, latest_by_mode[mode], str(meta.get("deviceId") or "unknown"))
        elif mode in supported_modes:
            mode_status[mode] = {"mode": mode, "status": "missing"}
        else:
            mode_status[mode] = {"mode": mode, "status": "unsupported"}

    return {
        "caseId": case_id,
        "displayName": str(meta.get("displayName") or meta.get("alias") or case_id),
        "alias": str(meta.get("alias") or case_id),
        "workloadEntry": str(meta.get("workloadEntry") or ""),
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
        "warmupCount": int(meta.get("warmupCount") or 0),
        "iterationCount": int(meta.get("iterationCount") or 0),
        "invocationCount": int(meta.get("invocationCount") or 0),
        "supportedModes": supported_modes,
        "recordedModes": recorded_modes,
        "missingModes": missing_modes,
        "unsupportedModes": unsupported_modes,
        "modeStatus": mode_status,
        "coverage": {
            "supportedModeCount": len(supported_modes),
            "recordedModeCount": len(recorded_modes),
            "missingModeCount": len(missing_modes),
            "unsupportedModeCount": len(unsupported_modes),
            "isComplete": len(supported_modes) > 0 and len(missing_modes) == 0,
        },
        "keyMetrics": _key_metrics_from_mode_status(mode_status),
    }


def _summarize_case_entries(entries: list[dict[str, Any]]) -> dict[str, Any]:
    cross_mode_count = sum(1 for entry in entries if len(list(entry.get("supportedModes") or [])) >= 2)
    managed_only_count = sum(1 for entry in entries if list(entry.get("supportedModes") or []) == ["managed"])
    return {
        "caseCount": len(entries),
        "crossModeCaseCount": cross_mode_count,
        "managedOnlyCaseCount": managed_only_count,
        "fullyRecordedCaseCount": sum(1 for entry in entries if bool(dict(entry.get("coverage") or {}).get("isComplete"))),
        "missingCaseCount": sum(1 for entry in entries if bool(entry.get("missingModes"))),
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


def _collect_data(repo_root: Path, subject_ids: list[str] | None = None) -> dict[str, Any]:
    testing_root = repo_root / "build" / "toolchains" / "run" / "testing"
    comparison_mod = _load("benchmark_comparison", testing_root / "benchmark_comparison.py")
    subjects_mod = _load("subjects", testing_root / "subjects.py")

    if subject_ids is None:
        subject_ids = subjects_mod.discover_perf_subject_ids(repo_root)

    data: dict[str, Any] = {}

    for subject_id in subject_ids:
        manifest = subjects_mod.load_subject_manifest(repo_root, subject_id)
        display_name = str(manifest.get("displayName") or subject_id)
        summary_workload_entry = str(manifest.get("workloadEntry") or "")
        records_path = repo_root / "subjects" / subject_id / "benchmark-records" / "records.jsonl"
        declared_cases = _load_declared_benchmark_cases(repo_root, subject_id)

        seen_pairs: set[tuple[str, str]] = set()
        by_device: dict[str, dict[str, Any]] = {}
        history: dict[str, dict[str, list[Any]]] = {}
        case_records_by_device: dict[str, dict[str, dict[str, Any]]] = {}

        if records_path.exists():
            for record in _iter_jsonl_reverse(records_path):
                mode = str(record.get("mode") or "")
                device_id = str(dict(record.get("device") or {}).get("id") or "unknown")
                benchmark_case = dict(record.get("benchmarkCase") or {})
                case_id = str(benchmark_case.get("stableId") or benchmark_case.get("alias") or "").strip()
                if case_id:
                    record_case_meta = {
                        "stableId": case_id,
                        "deviceId": device_id,
                        "alias": str(benchmark_case.get("alias") or case_id),
                        "displayName": str(benchmark_case.get("displayName") or benchmark_case.get("alias") or case_id),
                        "workloadEntry": str(benchmark_case.get("workloadEntry") or ""),
                        "assemblyName": str(benchmark_case.get("assemblyName") or ""),
                        "declaringType": str(benchmark_case.get("declaringType") or ""),
                        "methodName": str(benchmark_case.get("methodName") or ""),
                        "methodSignature": str(benchmark_case.get("methodSignature") or ""),
                        "category": int(benchmark_case.get("category") or 0),
                        "categoryLabel": _BENCHMARK_CATEGORY_LABELS.get(int(benchmark_case.get("category") or 0), "Uncategorized"),
                        "metrics": int(benchmark_case.get("metrics") or 0),
                        "metricLabels": _labels_from_mask(benchmark_case.get("metrics"), _METRIC_LABELS),
                        "modes": int(benchmark_case.get("modes") or 0),
                        "supportedModes": list(benchmark_case.get("supportedModes") or _supported_modes_from_mask(benchmark_case.get("modes"))),
                        "requires": int(benchmark_case.get("requires") or 0),
                        "requirementLabels": _labels_from_mask(benchmark_case.get("requires"), _RUNTIME_FEATURE_LABELS),
                        "warmupCount": int(benchmark_case.get("warmupCount") or 0),
                        "iterationCount": int(benchmark_case.get("iterationCount") or 0),
                        "invocationCount": int(benchmark_case.get("invocationCount") or 0),
                    }
                    case_payload = case_records_by_device.setdefault(device_id, {}).setdefault(
                        case_id,
                        {
                            "meta": {},
                            "records": {},
                        },
                    )
                    case_payload["meta"] = _merge_case_meta(
                        case_payload["meta"],
                        _merge_case_meta(dict(declared_cases.get(case_id) or {}), record_case_meta),
                    )
                    if mode and mode not in case_payload["records"]:
                        case_payload["records"][mode] = record
                    continue
                pair = (mode, device_id)

                history.setdefault(device_id, {}).setdefault(mode, [])
                if len(history[device_id][mode]) < 20:
                    history[device_id][mode].append(record)

                if pair not in seen_pairs:
                    seen_pairs.add(pair)
                    by_device.setdefault(device_id, {})[mode] = record

        comparisons: dict[str, Any] = {}
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

        platforms = sorted(set(_manifest_platforms(manifest)) | {_normalize_platform_key(device_id) for device_id in by_device})
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
            platform_key = _normalize_platform_key(device_id)
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


def generate(repo_root: Path, output_path: Path, subject_ids: list[str] | None = None) -> None:
    data = _collect_data(repo_root, subject_ids)
    overview = _build_overview(data)
    template = _read_template(repo_root / "build" / "toolchains" / "run" / "testing" / "templates" / "benchmark-dashboard.html")
    payload = _dashboard_payload(data, overview)
    html = template.replace(
        "/* BENCHMARK_DATA_PLACEHOLDER */",
        f"window.BENCHMARK_DATA = {json.dumps(payload, ensure_ascii=False, default=str)};",
    )

    output_path.parent.mkdir(parents=True, exist_ok=True)
    output_path.write_text(html, encoding="utf-8")


def update_docs(repo_root: Path, subject_id: str | None = None) -> None:
    testing_root = repo_root / "build" / "toolchains" / "run" / "testing"
    subjects_mod = _load("subjects", testing_root / "subjects.py")
    docs_root = repo_root / "docs" / "benchmark"
    subjects_doc_root = docs_root / "subjects"
    subjects_doc_root.mkdir(parents=True, exist_ok=True)

    del subject_id
    data = _collect_data(repo_root, subjects_mod.discover_perf_subject_ids(repo_root))
    overview = _build_overview(data)

    for stale_path in subjects_doc_root.glob("*.json"):
        stale_path.unlink()

    for current_subject_id, subject_payload in data.items():
        (subjects_doc_root / f"{current_subject_id}.json").write_text(
            json.dumps(subject_payload, ensure_ascii=False, indent=2, default=str),
            encoding="utf-8",
        )

    (docs_root / "overview.json").write_text(
        json.dumps(overview, ensure_ascii=False, indent=2, default=str),
        encoding="utf-8",
    )

    template = _read_template(testing_root / "templates" / "benchmark-dashboard.html")
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
