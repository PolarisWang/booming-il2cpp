from __future__ import annotations

from datetime import datetime, timezone
from pathlib import Path
from typing import Any
import sys

try:
    from ..core.common import read_json, write_json
    from . import compiled_catalog as compiled_catalog_module
    from . import declared_metadata_labels as declared_metadata_labels_module
    from . import path_resolver as path_resolver_module
    from . import subjects as subjects_module
except ImportError:
    root = Path(__file__).resolve().parents[1]
    sys.path.insert(0, str(root))
    from core.common import read_json, write_json
    from testing import compiled_catalog as compiled_catalog_module
    from testing import declared_metadata_labels as declared_metadata_labels_module
    from testing import path_resolver as path_resolver_module
    from testing import subjects as subjects_module


def _utc_timestamp() -> str:
    return datetime.now(timezone.utc).replace(microsecond=0).isoformat().replace("+00:00", "Z")


def _relative(repo_root: Path, path: Path) -> str:
    return path.resolve().relative_to(repo_root.resolve()).as_posix()


def _normalize_host_platform(host_platform: str) -> str:
    if host_platform.startswith("windows"):
        return "windows"
    if host_platform.startswith("macos"):
        return "macos"
    if host_platform.startswith("linux"):
        return "linux"
    return host_platform


def _numeric_metric_keys(payload: dict[str, Any]) -> list[str]:
    return sorted(
        key
        for key, value in payload.items()
        if isinstance(key, str) and isinstance(value, (int, float)) and not isinstance(value, bool)
    )


def _status_counts(entries: list[dict[str, Any]]) -> dict[str, int]:
    fail_count = sum(1 for entry in entries if str(entry.get("status") or "fail") != "ok")
    ok_count = len(entries) - fail_count
    return {
        "total": len(entries),
        "ok": ok_count,
        "fail": fail_count,
    }


def _declared_source_entry(entry: dict[str, Any]) -> str:
    assembly_name = str(entry.get("assemblyName") or "")
    declaring_type = str(entry.get("declaringType") or "")
    method_signature = str(entry.get("methodSignature") or "")
    if not assembly_name or not declaring_type or not method_signature:
        return ""
    type_name = declaring_type.rsplit(".", 1)[-1]
    return f"{assembly_name}/{type_name}::{method_signature}"


def _load_declared_benchmark_cases(repo_root: Path, subject_id: str) -> dict[str, dict[str, Any]]:
    try:
        catalog = compiled_catalog_module.build_subject_declared_test_catalog(
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
            "categoryLabel": declared_metadata_labels_module.benchmark_category_label(item.get("category")),
            "metrics": int(item.get("metrics") or 0),
            "metricLabels": declared_metadata_labels_module.labels_from_mask(
                item.get("metrics"),
                declared_metadata_labels_module.METRIC_LABELS,
            ),
            "modes": int(item.get("modes") or 0),
            "supportedModes": declared_metadata_labels_module.supported_modes_from_mask(item.get("modes")),
            "requires": int(item.get("requires") or 0),
            "requirementLabels": declared_metadata_labels_module.labels_from_mask(
                item.get("requires"),
                declared_metadata_labels_module.RUNTIME_FEATURE_LABELS,
            ),
            "archetype": int(item.get("archetype") or 0),
            "archetypeLabel": declared_metadata_labels_module.archetype_label(item.get("archetype")),
            "hotUpdateCapability": int(item.get("hotUpdateCapability") or 0),
            "hotUpdateCapabilityLabels": declared_metadata_labels_module.labels_from_mask(
                item.get("hotUpdateCapability"),
                declared_metadata_labels_module.HOT_UPDATE_CAPABILITY_LABELS,
            ),
            "warmupCount": int(item.get("warmupCount") or 0),
            "iterationCount": int(item.get("iterationCount") or 0),
            "invocationCount": int(item.get("invocationCount") or 0),
        }
    return cases


def _summary_workload_entry(manifest: dict[str, Any], matrix: dict[str, Any]) -> str:
    matrix_source = dict(matrix.get("source") or {})
    return str(matrix.get("workloadEntry") or matrix_source.get("entry") or manifest.get("workloadEntry") or "")


def _summary_benchmark_case(
    declared_cases: dict[str, dict[str, Any]],
    *,
    workload_entry: str,
) -> dict[str, Any] | None:
    if not workload_entry:
        return None
    for case in declared_cases.values():
        if str(case.get("workloadEntry") or "") == workload_entry:
            return dict(case)
    return None


def build_perf_dashboard_config(repo_root: Path) -> dict[str, Any]:
    entries: list[dict[str, Any]] = []
    for record in subjects_module.discover_perf_subject_records(repo_root):
        subject_id = str(record["subjectId"])
        manifest = dict(record["manifest"])
        declared_cases = _load_declared_benchmark_cases(repo_root, subject_id)
        for matrix in list(manifest.get("environmentMatrices") or []):
            matrix_payload = dict(matrix)
            matrix_id = str(matrix_payload.get("matrixId") or "")
            execution_context = dict(matrix_payload.get("executionContext") or {})
            supported_goals = [
                str(goal_id)
                for goal_id in list(matrix_payload.get("supportedGoals") or [])
                if str(goal_id).startswith("perf.")
            ]
            if not matrix_id or not supported_goals:
                continue

            host_platform = str(execution_context.get("hostPlatform") or "")
            host_baseline_label = _normalize_host_platform(host_platform)
            baseline_path = path_resolver_module.subject_perf_baseline_path(
                repo_root,
                subject_id,
                matrix_id,
                host_baseline_label,
            )
            baseline_payload = (
                read_json(baseline_path)
                if baseline_path.is_file()
                else {}
            )
            if not isinstance(baseline_payload, dict):
                baseline_payload = {}

            metric_keys = _numeric_metric_keys(baseline_payload)
            baseline_path_text = _relative(repo_root, baseline_path) if baseline_path.is_file() else None

            if not baseline_path_text or not metric_keys:
                continue

            summary_workload_entry = _summary_workload_entry(manifest, matrix_payload)
            summary_benchmark_case = _summary_benchmark_case(
                declared_cases,
                workload_entry=summary_workload_entry,
            )

            for goal_id in supported_goals:
                entries.append(
                    {
                        "subjectId": subject_id,
                        "matrixId": matrix_id,
                        "goalId": goal_id,
                        "hostPlatform": host_platform,
                        "targetPlatform": str(execution_context.get("targetPlatform") or ""),
                        "toolchainProfile": str(execution_context.get("toolchainProfile") or ""),
                        "runtimeProfile": str(execution_context.get("runtimeProfile") or ""),
                        "baselinePath": baseline_path_text,
                        "metricKeys": metric_keys,
                        "summaryWorkloadEntry": summary_workload_entry,
                        "summaryBenchmarkCase": summary_benchmark_case,
                        "declaredBenchmarkCaseCount": len(declared_cases),
                        "status": "ok",
                    }
                )

    counts = _status_counts(entries)
    return {
        "configVersion": "v1",
        "generatedAt": _utc_timestamp(),
        "status": "ok" if counts["fail"] == 0 else "fail",
        "statusCounts": counts,
        "subjectIds": sorted({str(entry["subjectId"]) for entry in entries}),
        "goalIds": sorted({str(entry["goalId"]) for entry in entries}),
        "hostPlatforms": sorted({str(entry["hostPlatform"]) for entry in entries}),
        "targetPlatforms": sorted({str(entry["targetPlatform"]) for entry in entries}),
        "entries": entries,
    }


def write_perf_dashboard_config(path: Path, payload: dict[str, Any]) -> None:
    write_json(path, payload)
