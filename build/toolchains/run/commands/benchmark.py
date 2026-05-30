"""benchmark.py 鈥?CLI command for running benchmarks and managing benchmark records.

Commands:
    run benchmark --subject <id> --mode <managed|native|interpreter> --record
    run benchmark --all --mode <mode> --record
    run benchmark --dashboard [--open]
    run benchmark status [--subject <id>] [--all]

`--record` only appends raw benchmark records. Refresh formal benchmark projections
with `run test inventory --json`.
"""
from __future__ import annotations

import importlib.util
import json
import os
import sys
import time
from pathlib import Path
from typing import Any

try:
    from ..testing import workspace_declared_collection as workspace_declared_collection_module
    from ..testing import verification_layout as verification_layout_module
except ImportError:
    run_root = Path(__file__).resolve().parents[1]
    if str(run_root) not in sys.path:
        sys.path.insert(0, str(run_root))
    from testing import workspace_declared_collection as workspace_declared_collection_module
    from testing import verification_layout as verification_layout_module

_MODE_ORDER = ("managed", "native", "interpreter")
_BENCHMARK_MODE_FLAGS = {
    "managed": 1 << 0,
    "native": 1 << 1,
    "interpreter": 1 << 2,
}
_ALL_BENCHMARK_MODE_FLAGS = sum(_BENCHMARK_MODE_FLAGS.values())
_BENCHMARK_HOST_EXECUTE_PARAMETER_TYPES = ("System.Int32",)


def _load(name: str, path: Path):
    spec = importlib.util.spec_from_file_location(name, path)
    assert spec and spec.loader
    mod = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(mod)  # type: ignore[union-attr]
    return mod


def _sanitize_identifier(value: str) -> str:
    sanitized = "".join(character if character.isalnum() else "_" for character in str(value).strip())
    return sanitized or "GeneratedHost"


def _normalize_host_platform(host_platform: str) -> str:
    value = str(host_platform or "").lower()
    if value.startswith("windows"):
        return "windows"
    if value.startswith("macos"):
        return "macos"
    if value.startswith("linux"):
        return "linux"
    return value


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


def _matrix_supports_perf_goal(matrix: dict[str, Any]) -> bool:
    return any(str(goal_id).startswith("perf.") for goal_id in list(matrix.get("supportedGoals") or []))


def _pipeline_stage_kinds(manifest: dict[str, Any], pipeline_id: str) -> set[str]:
    if not pipeline_id:
        return set()

    for pipeline in list(manifest.get("executionPipelines") or []):
        pipeline_payload = dict(pipeline)
        if str(pipeline_payload.get("pipelineId") or "") != pipeline_id:
            continue
        return {
            str(stage.get("kind") or "")
            for stage in list(pipeline_payload.get("stages") or [])
            if str(stage.get("kind") or "")
        }

    return set()


def _matrix_matches_benchmark_mode(
    manifest: dict[str, Any],
    *,
    matrix: dict[str, Any],
    mode: str,
    host_platform: str,
) -> bool:
    if not _matrix_supports_perf_goal(matrix):
        return False

    platform_key = _normalize_host_platform(host_platform)
    execution_context = dict(matrix.get("executionContext") or {})
    host_value = str(execution_context.get("hostPlatform") or "")
    host_key = _normalize_host_platform(host_value)
    if platform_key and host_key and host_key != platform_key:
        return False

    pipeline_id = str(matrix.get("pipelineId") or "")
    runtime_profile = str(execution_context.get("runtimeProfile") or "")
    stage_kinds = _pipeline_stage_kinds(manifest, pipeline_id)
    expected_stage_kinds = set(_mode_stage_kinds(mode))
    if expected_stage_kinds and expected_stage_kinds.intersection(stage_kinds):
        return True

    haystack = " ".join((str(matrix.get("matrixId") or ""), pipeline_id, runtime_profile)).lower()
    return any(term in haystack for term in _mode_selection_terms(mode))


def _select_benchmark_matrix_id(
    manifest: dict[str, Any],
    *,
    mode: str,
    host_platform: str,
) -> str:
    platform_key = _normalize_host_platform(host_platform)
    desired_terms = _mode_selection_terms(mode)
    expected_stage_kinds = set(_mode_stage_kinds(mode))

    best_matrix_id: str | None = None
    best_score = -1

    for matrix in list(manifest.get("environmentMatrices") or []):
        matrix_payload = dict(matrix)
        matrix_id = str(matrix_payload.get("matrixId") or "")
        pipeline_id = str(matrix_payload.get("pipelineId") or "")
        execution_context = dict(matrix_payload.get("executionContext") or {})
        host_value = str(execution_context.get("hostPlatform") or "")
        runtime_profile = str(execution_context.get("runtimeProfile") or "")
        host_key = _normalize_host_platform(host_value)
        stage_kinds = _pipeline_stage_kinds(manifest, pipeline_id)
        haystack = " ".join((matrix_id, pipeline_id, runtime_profile)).lower()

        if not _matrix_matches_benchmark_mode(
            manifest,
            matrix=matrix_payload,
            mode=mode,
            host_platform=host_platform,
        ):
            continue

        score = 0
        if platform_key and matrix_id.lower().startswith(platform_key):
            score += 4
        if platform_key and host_key == platform_key:
            score += 3
        if any(term == pipeline_id.lower() for term in desired_terms):
            score += 3
        if any(term in runtime_profile.lower() for term in desired_terms):
            score += 2
        if expected_stage_kinds and expected_stage_kinds.intersection(stage_kinds):
            score += 2
        if "benchmark" in pipeline_id.lower():
            score += 1

        if score > best_score:
            best_score = score
            best_matrix_id = matrix_id

    if best_matrix_id:
        return best_matrix_id

    raise ValueError(
        f"no {mode} benchmark matrix configured for subject '{manifest.get('subjectId') or '?'}'"
    )


def _supported_benchmark_modes(
    manifest: dict[str, Any],
    *,
    host_platform: str,
    requested_modes: list[str],
) -> list[str]:
    supported_modes: list[str] = []
    for mode in requested_modes:
        try:
            _select_benchmark_matrix_id(manifest, mode=mode, host_platform=host_platform)
        except ValueError:
            continue
        supported_modes.append(mode)
    return supported_modes


def _extract_runtime_performance(
    stage_results: list[dict[str, Any]],
    *,
    preferred_kind: str,
) -> dict[str, Any] | None:
    for stage_result in reversed(stage_results):
        if str(stage_result.get("kind") or "") != preferred_kind:
            continue
        performance = dict(dict(stage_result.get("details") or {}).get("performance") or {})
        metrics = dict(performance.get("metrics") or {})
        if metrics:
            return {
                "metrics": metrics,
                "regressionStatus": str(performance.get("regressionStatus") or "no-baseline"),
            }

    for stage_result in reversed(stage_results):
        if str(stage_result.get("bucket") or "") != "runtime":
            continue
        performance = dict(dict(stage_result.get("details") or {}).get("performance") or {})
        metrics = dict(performance.get("metrics") or {})
        if metrics:
            return {
                "metrics": metrics,
                "regressionStatus": str(performance.get("regressionStatus") or "no-baseline"),
            }

    return None


def _preferred_runtime_stage_kind(mode: str) -> str:
    return {
        "managed": "runtime-perf-collect",
        "native": "native-runtime-perf",
        "interpreter": "interpreter-runtime-perf",
    }.get(mode, "runtime")


def _declared_source_entry(entry: dict[str, Any]) -> str:
    assembly_name = str(entry.get("assemblyName") or "")
    declaring_type = str(entry.get("declaringType") or "")
    method_signature = str(entry.get("methodSignature") or "")
    if not assembly_name or not declaring_type or not method_signature:
        return ""
    type_name = declaring_type.rsplit(".", 1)[-1]
    return f"{assembly_name}/{type_name}::{method_signature}"


def _declared_benchmark_host_source_entry(
    repo_root: Path,
    subject_id: str,
    *,
    matrix_id: str = "",
    host_platform: str = "",
    workspace_manifests_module: Any | None = None,
    project_workspace_module: Any | None = None,
) -> str:
    testing_root = Path(__file__).resolve().parents[1] / "testing"
    workspace_manifests_mod = workspace_manifests_module or _load(
        "workspace_manifests",
        testing_root / "workspace_manifests.py",
    )
    assembly_name = f"{subject_id}.DeclaredBenchmarkHost"
    loaded_manifest = workspace_manifests_mod.load_subject_workspace_manifest(repo_root, subject_id)
    if matrix_id and not _workspace_manifest_supports_native_benchmark_host(
        loaded_manifest,
        matrix_id=matrix_id,
        workspace_manifests_module=workspace_manifests_mod,
    ):
        refreshed_manifest = _refresh_workspace_manifest_for_native_benchmark(
            repo_root,
            subject_id,
            matrix_id=matrix_id,
            host_platform=host_platform,
            workspace_manifests_module=workspace_manifests_mod,
            project_workspace_module=project_workspace_module,
        )
        if refreshed_manifest is not None:
            loaded_manifest = refreshed_manifest
    if loaded_manifest is not None:
        _, manifest = loaded_manifest
        managed_test_project = None
        if matrix_id:
            native_test_project = workspace_manifests_mod.find_native_test_project(
                manifest,
                matrix_id=matrix_id,
                host_kind="benchmark-host",
            )
            managed_test_project_id = (
                str(native_test_project.get("managedTestProjectId") or "").strip()
                if isinstance(native_test_project, dict)
                else ""
            )
            if managed_test_project_id and hasattr(workspace_manifests_mod, "find_managed_test_project_by_id"):
                managed_test_project = workspace_manifests_mod.find_managed_test_project_by_id(
                    manifest,
                    project_id=managed_test_project_id,
                )
        if managed_test_project is None:
            managed_test_project = workspace_manifests_mod.find_managed_test_project(
                manifest,
                host_kind="benchmark-host",
            )
        if isinstance(managed_test_project, dict):
            candidate_assembly_name = str(managed_test_project.get("assemblyName") or "").strip()
            if candidate_assembly_name:
                assembly_name = candidate_assembly_name

    sanitized_subject_id = _sanitize_identifier(subject_id)
    namespace_name = f"Chaos.Generated.ManagedTests.{sanitized_subject_id}"
    class_name = f"{sanitized_subject_id}DeclaredBenchmarkHost"
    parameter_signature = ",".join(_BENCHMARK_HOST_EXECUTE_PARAMETER_TYPES)
    return f"{assembly_name}/{namespace_name}.{class_name}::Execute({parameter_signature})"


def _workspace_manifest_supports_native_benchmark_host(
    loaded_manifest: tuple[Path, dict[str, Any]] | None,
    *,
    matrix_id: str,
    workspace_manifests_module: Any,
) -> bool:
    if loaded_manifest is None:
        return False

    _, manifest = loaded_manifest
    requested_matrix_id = str(matrix_id or "").strip()
    if not requested_matrix_id:
        return True

    matrix_exists = any(
        str(item.get("matrixId") or "").strip() == requested_matrix_id
        for item in list(manifest.get("matrices") or [])
        if isinstance(item, dict)
    )
    if not matrix_exists:
        return False

    native_test_project = workspace_manifests_module.find_native_test_project(
        manifest,
        matrix_id=requested_matrix_id,
        host_kind="benchmark-host",
    )
    if not isinstance(native_test_project, dict):
        return False

    managed_test_project_id = str(native_test_project.get("managedTestProjectId") or "").strip()
    if not managed_test_project_id:
        return False

    managed_test_project = None
    if hasattr(workspace_manifests_module, "find_managed_test_project_by_id"):
        managed_test_project = workspace_manifests_module.find_managed_test_project_by_id(
            manifest,
            project_id=managed_test_project_id,
        )
    if not isinstance(managed_test_project, dict):
        return False

    return bool(str(managed_test_project.get("assemblyName") or "").strip())


def _refresh_workspace_manifest_for_native_benchmark(
    repo_root: Path,
    subject_id: str,
    *,
    matrix_id: str,
    host_platform: str,
    workspace_manifests_module: Any,
    project_workspace_module: Any | None = None,
) -> tuple[Path, dict[str, Any]] | None:
    normalized_host_platform = _normalize_host_platform(host_platform)
    if not matrix_id or not normalized_host_platform:
        return workspace_manifests_module.load_subject_workspace_manifest(repo_root, subject_id)

    project_workspace_mod = project_workspace_module or _load(
        "project_workspace",
        Path(__file__).resolve().parents[1] / "subject" / "project_workspace.py",
    )
    project_workspace_mod.generate_subject_workspace(
        repo_root,
        normalized_host_platform,
        {
            "id": f"subject/{subject_id}",
            "matrix": matrix_id,
            "variant": "PROFILE",
            "auto-refresh-missing-generated": True,
        },
    )
    return workspace_manifests_module.load_subject_workspace_manifest(repo_root, subject_id)


def _supported_modes_from_mask(value: Any) -> list[str]:
    try:
        mask = int(value or 0)
    except (TypeError, ValueError):
        mask = 0
    if mask <= 0:
        mask = _ALL_BENCHMARK_MODE_FLAGS
    return [mode for mode in _MODE_ORDER if mask & _BENCHMARK_MODE_FLAGS[mode]]


def _record_benchmark_case_payload(benchmark_case: dict[str, Any]) -> dict[str, Any]:
    payload = {
        "stableId": str(benchmark_case.get("stableId") or ""),
        "alias": str(benchmark_case.get("alias") or "") or str(benchmark_case.get("stableId") or ""),
        "displayName": str(benchmark_case.get("displayName") or "")
        or str(benchmark_case.get("alias") or "")
        or str(benchmark_case.get("stableId") or ""),
        "workloadEntry": str(benchmark_case.get("workloadEntry") or ""),
        "assemblyName": str(benchmark_case.get("assemblyName") or ""),
        "declaringType": str(benchmark_case.get("declaringType") or ""),
        "methodName": str(benchmark_case.get("methodName") or ""),
        "methodSignature": str(benchmark_case.get("methodSignature") or ""),
        "category": int(benchmark_case.get("category") or 0),
        "metrics": int(benchmark_case.get("metrics") or 0),
        "modes": int(benchmark_case.get("modes") or 0),
        "requires": int(benchmark_case.get("requires") or 0),
        "archetype": int(benchmark_case.get("archetype") or 0),
        "hotUpdateCapability": int(benchmark_case.get("hotUpdateCapability") or 0),
        "warmupCount": int(benchmark_case.get("warmupCount") or 0),
        "iterationCount": int(benchmark_case.get("iterationCount") or 0),
        "invocationCount": int(benchmark_case.get("invocationCount") or 0),
        "supportedModes": list(benchmark_case.get("supportedModes") or []),
    }
    entry_index = benchmark_case.get("entryIndex")
    if isinstance(entry_index, int) and not isinstance(entry_index, bool) and entry_index >= 0:
        payload["entryIndex"] = int(entry_index)
    return payload


def _benchmark_case_supports_mode(benchmark_case: dict[str, Any], mode: str) -> bool:
    supported_modes = [str(item) for item in list(benchmark_case.get("supportedModes") or _MODE_ORDER) if str(item)]
    return mode in supported_modes


def _declared_benchmark_entry_selection(benchmark_case: dict[str, Any]) -> dict[str, Any]:
    payload: dict[str, Any] = {
        "family": "declared-benchmark",
        "stableId": str(benchmark_case.get("stableId") or ""),
        "alias": str(benchmark_case.get("alias") or "") or str(benchmark_case.get("stableId") or ""),
    }
    entry_index = benchmark_case.get("entryIndex")
    if isinstance(entry_index, int) and not isinstance(entry_index, bool) and entry_index >= 0:
        payload["entryIndex"] = int(entry_index)
    return payload


def _benchmark_case_run_fragment(benchmark_case: dict[str, Any]) -> str:
    return _sanitize_identifier(
        str(benchmark_case.get("alias") or benchmark_case.get("stableId") or "case")
    ).lower()


def _append_benchmark_record(
    *,
    repo_root: Path,
    subject_id: str,
    mode: str,
    device: dict[str, Any],
    records_mod: Any,
    host_platform: str,
    metrics: dict[str, Any],
    regression_found: bool,
    benchmark_case: dict[str, Any] | None = None,
) -> dict[str, Any]:
    import subprocess
    import datetime

    try:
        git_out = subprocess.check_output(
            ["git", "rev-parse", "--short", "HEAD"],
            text=True,
            cwd=repo_root,
            timeout=5,
        ).strip()
    except Exception:
        git_out = "unknown"

    record: dict[str, Any] = {
        "runId": f"{datetime.datetime.now(datetime.timezone.utc).strftime('%Y%m%d-%H%M%S')}-{subject_id}-{mode}",
        "subject": subject_id,
        "mode": mode,
        "platform": str(host_platform),
        "device": device,
        "recordedAt": datetime.datetime.now(datetime.timezone.utc).isoformat().replace("+00:00", "Z"),
        "gitCommit": git_out,
        "gitBranch": "main",
        "metrics": dict(metrics),
    }
    if benchmark_case is not None:
        record["benchmarkCase"] = _record_benchmark_case_payload(benchmark_case)
    records_mod.append_record(repo_root, record)
    return {"record": record, "regressionFound": regression_found}


def _declared_benchmark_cases_from_catalog(catalog: dict[str, Any]) -> list[dict[str, Any]]:
    cases: list[dict[str, Any]] = []
    for payload in list(dict(catalog).get("declaredBenchmarks") or []):
        item = dict(payload or {})
        workload_entry = _declared_source_entry(item)
        stable_id = str(item.get("stableId") or "").strip()
        if not stable_id or not workload_entry:
            continue
        supported_modes = _supported_modes_from_mask(item.get("modes"))
        case_payload = {
            "stableId": stable_id,
            "alias": str(item.get("alias") or "").strip() or stable_id,
            "displayName": str(item.get("alias") or "").strip() or stable_id,
            "workloadEntry": workload_entry,
            "assemblyName": str(item.get("assemblyName") or ""),
            "declaringType": str(item.get("declaringType") or ""),
            "methodName": str(item.get("methodName") or ""),
            "methodSignature": str(item.get("methodSignature") or ""),
            "category": int(item.get("category") or 0),
            "metrics": int(item.get("metrics") or 0),
            "modes": int(item.get("modes") or 0),
            "requires": int(item.get("requires") or 0),
            "archetype": int(item.get("archetype") or 0),
            "hotUpdateCapability": int(item.get("hotUpdateCapability") or 0),
            "warmupCount": int(item.get("warmupCount") or 0),
            "iterationCount": int(item.get("iterationCount") or 0),
            "invocationCount": int(item.get("invocationCount") or 0),
            "supportedModes": supported_modes,
        }
        entry_index = item.get("entryIndex")
        if isinstance(entry_index, int) and not isinstance(entry_index, bool) and entry_index >= 0:
            case_payload["entryIndex"] = int(entry_index)
        cases.append(case_payload)
    return cases


def _discover_declared_benchmark_cases(
    repo_root: Path,
    subject_id: str,
    *,
    compiled_catalog_module: Any | None = None,
) -> list[dict[str, Any]]:
    workspace_collection = workspace_declared_collection_module.load_workspace_declared_collection(
        repo_root,
        subject_id,
        host_kind="benchmark-host",
    )
    if workspace_collection is not None:
        return _declared_benchmark_cases_from_catalog(workspace_collection)

    testing_root = repo_root / "build" / "toolchains" / "run" / "testing"
    try:
        compiled_catalog_mod = compiled_catalog_module or _load("compiled_catalog", testing_root / "compiled_catalog.py")
        catalog = compiled_catalog_mod.build_subject_declared_test_catalog(
            repo_root=repo_root,
            subject_id=subject_id,
            force_build=True,
        )
    except Exception:
        return []
    return _declared_benchmark_cases_from_catalog(dict(catalog))


def _run_subject_benchmark_pipeline(
    *,
    repo_root: Path,
    subject_id: str,
    mode: str,
    host_platform: str,
    subjects_module: Any | None = None,
    planner_module: Any | None = None,
    executor_module: Any | None = None,
    benchmark_case: dict[str, Any] | None = None,
    source_entry: str | None = None,
) -> dict[str, Any]:
    testing_root = repo_root / "build" / "toolchains" / "run" / "testing"
    subjects_mod = subjects_module or _load("subjects", testing_root / "subjects.py")
    executor_mod = executor_module or _load("subject_executor", testing_root / "subject_executor.py")

    try:
        manifest = subjects_mod.load_subject_manifest(repo_root, subject_id)
        matrix_id = _select_benchmark_matrix_id(manifest, mode=mode, host_platform=host_platform)
        run_id = f"benchmark-{subject_id}-{mode}-{int(time.time())}"
        if benchmark_case is None:
            execution_result = executor_mod.execute_subject_matrix(
                repo_root,
                subject_id,
                goal_id="perf.release",
                matrix_id=matrix_id,
                run_id=run_id,
            )
        else:
            planner_mod = planner_module or _load("subject_planner", testing_root / "subject_planner.py")
            workload_entry = str(benchmark_case.get("workloadEntry") or "")
            plan = planner_mod.build_plan(
                repo_root,
                subject_id,
                goal_id="perf.release",
                matrix_id=matrix_id,
                run_id=run_id,
                source_entry=source_entry,
                workload_entry=workload_entry,
                entry_selection=_declared_benchmark_entry_selection(benchmark_case),
            )
            execution_result = executor_mod.execute_plan(
                repo_root,
                plan,
                run_id=run_id,
            )
    except Exception as error:
        return {"error": str(error)}

    errors = [str(item) for item in list(execution_result.get("errors") or []) if str(item)]
    if errors:
        return {"error": "; ".join(errors)}

    performance = _extract_runtime_performance(
        list(execution_result.get("stageResults") or []),
        preferred_kind=_preferred_runtime_stage_kind(mode),
    )
    if performance is None:
        error_text = "; ".join(errors) or f"{mode} benchmark did not produce runtime metrics for {subject_id}"
        return {"error": error_text}

    return {
        "metrics": dict(performance["metrics"]),
        "regressionFound": performance["regressionStatus"] == "regressed",
        "executionResult": execution_result,
        "matrixId": matrix_id,
        "runId": run_id,
    }


def _run_native_benchmark_pipeline(
    *,
    repo_root: Path,
    subject_id: str,
    host_platform: str,
    subjects_module: Any | None = None,
    planner_module: Any | None = None,
    executor_module: Any | None = None,
    benchmark_case: dict[str, Any] | None = None,
    source_entry: str | None = None,
) -> dict[str, Any]:
    resolved_source_entry = source_entry
    if benchmark_case is not None and not resolved_source_entry:
        testing_root = repo_root / "build" / "toolchains" / "run" / "testing"
        subjects_mod = subjects_module or _load("subjects", testing_root / "subjects.py")
        manifest = subjects_mod.load_subject_manifest(repo_root, subject_id)
        matrix_id = _select_benchmark_matrix_id(manifest, mode="native", host_platform=host_platform)
        resolved_source_entry = _declared_benchmark_host_source_entry(
            repo_root,
            subject_id,
            matrix_id=matrix_id,
            host_platform=host_platform,
        )
    return _run_subject_benchmark_pipeline(
        repo_root=repo_root,
        subject_id=subject_id,
        mode="native",
        host_platform=host_platform,
        subjects_module=subjects_module,
        planner_module=planner_module,
        executor_module=executor_module,
        benchmark_case=benchmark_case,
        source_entry=resolved_source_entry,
    )


def _select_summary_benchmark_case(
    manifest: dict[str, Any],
    *,
    benchmark_cases: list[dict[str, Any]],
    mode: str,
) -> dict[str, Any] | None:
    supported_cases = [dict(item) for item in benchmark_cases if _benchmark_case_supports_mode(item, mode)]
    if not supported_cases:
        return None

    manifest_workload_entry = str(manifest.get("workloadEntry") or "").strip()
    if manifest_workload_entry:
        for benchmark_case in supported_cases:
            if str(benchmark_case.get("workloadEntry") or "").strip() == manifest_workload_entry:
                return benchmark_case

    return min(
        supported_cases,
        key=lambda item: (
            str(item.get("stableId") or ""),
            str(item.get("alias") or ""),
        ),
    )


def _extract_stage_manifest_path(
    execution_result: dict[str, Any],
    *,
    bucket: str,
    kind: str = "",
) -> str:
    for stage_result in reversed(list(execution_result.get("stageResults") or [])):
        if kind and str(stage_result.get("kind") or "") != kind:
            continue
        if bucket and str(stage_result.get("bucket") or "") != bucket:
            continue
        manifest_path = str(stage_result.get("manifestPath") or "").strip()
        if manifest_path:
            return manifest_path
    return ""


def _runtime_stage_from_plan(plan: dict[str, Any], *, preferred_kind: str) -> dict[str, Any] | None:
    for stage in list(plan.get("stagePlan") or []):
        if str(stage.get("kind") or "") == preferred_kind:
            return dict(stage)
    for stage in list(plan.get("stagePlan") or []):
        if str(stage.get("bucket") or "") == "runtime":
            return dict(stage)
    return None


def _run_native_benchmark_case_with_shared_build(
    *,
    repo_root: Path,
    subject_id: str,
    host_platform: str,
    benchmark_case: dict[str, Any],
    build_manifest_path: str,
    matrix_id: str,
    base_run_id: str,
    planner_module: Any | None = None,
    source_entry: str | None = None,
) -> dict[str, Any]:
    testing_root = repo_root / "build" / "toolchains" / "run" / "testing"
    planner_mod = planner_module or _load("subject_planner", testing_root / "subject_planner.py")
    resolved_source_entry = source_entry or _declared_benchmark_host_source_entry(
        repo_root,
        subject_id,
        matrix_id=matrix_id,
        host_platform=host_platform,
    )
    run_id = f"{base_run_id}-{_benchmark_case_run_fragment(benchmark_case)}"
    plan = planner_mod.build_plan(
        repo_root,
        subject_id,
        goal_id="perf.release",
        matrix_id=matrix_id,
        run_id=run_id,
        source_entry=resolved_source_entry,
        workload_entry=str(benchmark_case.get("workloadEntry") or ""),
        entry_selection=_declared_benchmark_entry_selection(benchmark_case),
    )
    runtime_stage = _runtime_stage_from_plan(
        plan,
        preferred_kind=_preferred_runtime_stage_kind("native"),
    )
    if runtime_stage is None:
        return {"error": f"native benchmark plan did not produce a runtime stage for {subject_id}"}

    return {"error": "native benchmark execution requires subject_workers module which has been removed"}


def _run_native_declared_benchmark_records(
    *,
    repo_root: Path,
    subject_id: str,
    device: dict[str, Any],
    records_mod: Any,
    host_platform: str,
    benchmark_cases: list[dict[str, Any]],
    subjects_module: Any | None = None,
    planner_module: Any | None = None,
    executor_module: Any | None = None,
) -> list[dict[str, Any]]:
    testing_root = repo_root / "build" / "toolchains" / "run" / "testing"
    subjects_mod = subjects_module or _load("subjects", testing_root / "subjects.py")
    native_cases = [dict(item) for item in benchmark_cases if _benchmark_case_supports_mode(item, "native")]
    if not native_cases:
        return []

    manifest = subjects_mod.load_subject_manifest(repo_root, subject_id)
    summary_case = _select_summary_benchmark_case(
        manifest,
        benchmark_cases=native_cases,
        mode="native",
    )
    if summary_case is None:
        return []

    source_entry = _declared_benchmark_host_source_entry(
        repo_root,
        subject_id,
        matrix_id=_select_benchmark_matrix_id(manifest, mode="native", host_platform=host_platform),
        host_platform=host_platform,
    )
    summary_pipeline_result = _run_native_benchmark_pipeline(
        repo_root=repo_root,
        subject_id=subject_id,
        host_platform=host_platform,
        subjects_module=subjects_mod,
        planner_module=planner_module,
        executor_module=executor_module,
        benchmark_case=summary_case,
        source_entry=source_entry,
    )
    if "error" in summary_pipeline_result:
        return [summary_pipeline_result]

    execution_result = dict(summary_pipeline_result.get("executionResult") or {})
    build_manifest_path = ""
    if len(native_cases) > 1:
        build_manifest_path = _extract_stage_manifest_path(
            execution_result,
            bucket="build",
            kind="build-target",
        )
        if not build_manifest_path:
            return [{"error": f"native benchmark build did not produce a reusable build manifest for {subject_id}"}]

    results = [
        _append_benchmark_record(
            repo_root=repo_root,
            subject_id=subject_id,
            mode="native",
            device=device,
            records_mod=records_mod,
            host_platform=host_platform,
            metrics=dict(summary_pipeline_result["metrics"]),
            regression_found=bool(summary_pipeline_result["regressionFound"]),
        ),
        _append_benchmark_record(
            repo_root=repo_root,
            subject_id=subject_id,
            mode="native",
            device=device,
            records_mod=records_mod,
            host_platform=host_platform,
            metrics=dict(summary_pipeline_result["metrics"]),
            regression_found=bool(summary_pipeline_result["regressionFound"]),
            benchmark_case=summary_case,
        ),
    ]

    matrix_id = str(summary_pipeline_result.get("matrixId") or "")
    base_run_id = str(summary_pipeline_result.get("runId") or f"benchmark-{subject_id}-native")
    summary_case_key = (
        str(summary_case.get("stableId") or ""),
        str(summary_case.get("alias") or ""),
    )
    for benchmark_case in native_cases:
        candidate_case_key = (
            str(benchmark_case.get("stableId") or ""),
            str(benchmark_case.get("alias") or ""),
        )
        if candidate_case_key == summary_case_key:
            continue

        case_pipeline_result = _run_native_benchmark_case_with_shared_build(
            repo_root=repo_root,
            subject_id=subject_id,
            host_platform=host_platform,
            benchmark_case=benchmark_case,
            build_manifest_path=build_manifest_path,
            matrix_id=matrix_id,
            base_run_id=base_run_id,
            planner_module=planner_module,
            source_entry=source_entry,
        )
        if "error" in case_pipeline_result:
            results.append(case_pipeline_result)
            continue

        results.append(
            _append_benchmark_record(
                repo_root=repo_root,
                subject_id=subject_id,
                mode="native",
                device=device,
                records_mod=records_mod,
                host_platform=host_platform,
                metrics=dict(case_pipeline_result["metrics"]),
                regression_found=bool(case_pipeline_result["regressionFound"]),
                benchmark_case=benchmark_case,
            )
        )

    return results


def dispatch(args: list[str], repo_root: Path, host_platform: str) -> int:
    """Dispatch benchmark sub-commands.

    Returns exit code: 0 = success, 1 = regression, 2 = error.
    """
    testing_root = repo_root / "build" / "toolchains" / "run" / "testing"
    records_mod = _load("benchmark_records", testing_root / "benchmark_records.py")
    detector_mod = _load("device_detector", testing_root / "device_detector.py")
    dash_mod = _load("benchmark_dashboard_generator", testing_root / "benchmark_dashboard_generator.py")
    subjects_mod = _load("subjects", testing_root / "subjects.py")

    # 鈹€鈹€ Parse args 鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€
    subject_id: str | None = None
    mode: str | None = None
    do_record = False
    do_dashboard = False
    do_open = False
    do_status = False
    do_all = False
    output: str | None = None

    i = 0
    while i < len(args):
        a = args[i]
        if a == "--subject" and i + 1 < len(args):
            subject_id = args[i + 1]; i += 2
        elif a == "--mode" and i + 1 < len(args):
            mode = args[i + 1]; i += 2
        elif a == "--record":
            do_record = True; i += 1
        elif a == "--dashboard":
            do_dashboard = True; i += 1
        elif a == "--open":
            do_open = True; i += 1
        elif a == "status":
            do_status = True; i += 1
        elif a == "--all":
            do_all = True; i += 1
        elif a == "--output" and i + 1 < len(args):
            output = args[i + 1]; i += 2
        else:
            i += 1

    # 鈹€鈹€ status sub-command 鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€
    if do_status:
        return _cmd_status(repo_root, records_mod, subject_id, do_all)

    # 鈹€鈹€ dashboard sub-command 鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€
    if do_dashboard and not do_record:
        default_output_path = verification_layout_module.benchmark_projection_root(repo_root) / "dashboard.html"
        output_path = Path(output) if output else default_output_path
        print(f"Generating benchmark dashboard 鈫?{output_path}")
        if output is None or output_path == default_output_path:
            dash_mod.update_docs(repo_root)
        else:
            dash_mod.generate(repo_root, output_path)
        print("鉁?Dashboard generated")
        if do_open:
            import webbrowser
            webbrowser.open(output_path.as_uri())
        return 0

    # 鈹€鈹€ record sub-command 鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€
    if do_record:
        requested_modes = [mode] if mode else ["managed", "native", "interpreter"]
        subject_run_plan: list[tuple[str, list[str]]] = []

        if do_all:
            for candidate_subject_id in list(subjects_mod.discover_perf_subject_ids(repo_root)):
                manifest = subjects_mod.load_subject_manifest(repo_root, candidate_subject_id)
                supported_modes = _supported_benchmark_modes(
                    manifest,
                    host_platform=host_platform,
                    requested_modes=requested_modes,
                )
                if not supported_modes:
                    continue
                subject_run_plan.append((candidate_subject_id, supported_modes))
        elif subject_id:
            manifest = subjects_mod.load_subject_manifest(repo_root, subject_id)
            supported_modes = _supported_benchmark_modes(
                manifest,
                host_platform=host_platform,
                requested_modes=requested_modes,
            )
            if mode and mode not in supported_modes:
                print(
                    f"ERROR: subject '{subject_id}' does not support {mode} benchmark on {host_platform}",
                    file=sys.stderr,
                )
                return 2
            if not supported_modes:
                print(
                    f"ERROR: subject '{subject_id}' does not support benchmark recording on {host_platform}",
                    file=sys.stderr,
                )
                return 2
            subject_run_plan = [(subject_id, supported_modes)]
        else:
            print("ERROR: --subject or --all required with --record", file=sys.stderr)
            return 2

        device = detector_mod.load_or_detect(repo_root)
        errors_found = False
        regression_found = False

        for sid, modes_to_run in subject_run_plan:
            benchmark_cases = _discover_declared_benchmark_cases(repo_root, sid)
            for m in modes_to_run:
                supported_benchmark_cases = [
                    dict(item)
                    for item in benchmark_cases
                    if _benchmark_case_supports_mode(item, m)
                ]
                if m == "native" and supported_benchmark_cases:
                    native_results = _run_native_declared_benchmark_records(
                        repo_root=repo_root,
                        subject_id=sid,
                        device=device,
                        records_mod=records_mod,
                        host_platform=host_platform,
                        benchmark_cases=supported_benchmark_cases,
                    )
                    for native_result in native_results:
                        _print_result(sid, m, device, native_result)
                        if native_result.get("error"):
                            errors_found = True
                        if native_result.get("regressionFound"):
                            regression_found = True
                    continue

                result = _run_pipeline_and_record(
                    repo_root=repo_root,
                    subject_id=sid,
                    mode=m,
                    device=device,
                    records_mod=records_mod,
                    host_platform=host_platform,
                )
                _print_result(sid, m, device, result)
                if result.get("error"):
                    errors_found = True
                if result.get("regressionFound"):
                    regression_found = True

                for benchmark_case in supported_benchmark_cases:
                    case_result = _run_pipeline_and_record(
                        repo_root=repo_root,
                        subject_id=sid,
                        mode=m,
                        device=device,
                        records_mod=records_mod,
                        host_platform=host_platform,
                        benchmark_case=benchmark_case,
                    )
                    _print_result(sid, m, device, case_result)
                    if case_result.get("error"):
                        errors_found = True
                    if case_result.get("regressionFound"):
                        regression_found = True

        if errors_found:
            print("Benchmark execution failed; some runs did not generate records.")
            return 2
        if regression_found:
            print(
                "Benchmark records generated; regression verdict found. "
                "Run `run test inventory --json` to merge records and refresh verification/projections/benchmark/."
            )
            return 0
        print(
            "Benchmark records generated without execution errors. "
            "Run `run test inventory --json` to refresh formal benchmark projections."
        )
        return 0

    # 鈹€鈹€ Default: show help 鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€
    _print_help()
    return 0


# ---------------------------------------------------------------------------
# Internal helpers
# ---------------------------------------------------------------------------


def _run_pipeline_and_record(
    *,
    repo_root: Path,
    subject_id: str,
    mode: str,
    device: dict[str, Any],
    records_mod: Any,
    host_platform: str,
    benchmark_case: dict[str, Any] | None = None,
) -> dict[str, Any]:
    """Run the benchmark subject in the given mode and append a record.

    All modes flow through the subject planner/executor pipeline so the same
    workload contract is consumed by managed, native and interpreter benchmarks.
    """
    manifest_path = verification_layout_module.owner_manifest_path(repo_root, subject_id)
    if not manifest_path.exists():
        return {"error": f"subject not found: {subject_id}"}

    if mode == "native":
        native_kwargs = {
            "repo_root": repo_root,
            "subject_id": subject_id,
            "host_platform": host_platform,
        }
        if benchmark_case is not None:
            native_kwargs["benchmark_case"] = benchmark_case
        pipeline_result = _run_native_benchmark_pipeline(**native_kwargs)
    else:
        pipeline_kwargs = {
            "repo_root": repo_root,
            "subject_id": subject_id,
            "mode": mode,
            "host_platform": host_platform,
        }
        if benchmark_case is not None:
            pipeline_kwargs["benchmark_case"] = benchmark_case
        pipeline_result = _run_subject_benchmark_pipeline(**pipeline_kwargs)
    if "error" in pipeline_result:
        return pipeline_result

    metrics = dict(pipeline_result.get("metrics") or {})
    regression_found = bool(pipeline_result.get("regressionFound"))
    if not metrics:
        return {"error": f"{mode} benchmark returned no metrics for {subject_id}"}

    return _append_benchmark_record(
        repo_root=repo_root,
        subject_id=subject_id,
        mode=mode,
        device=device,
        records_mod=records_mod,
        host_platform=host_platform,
        metrics=metrics,
        regression_found=regression_found,
        benchmark_case=benchmark_case,
    )

def _print_result(subject_id: str, mode: str, device: dict[str, Any], result: dict[str, Any]) -> None:
    if "error" in result:
        print(f"  鉁?{subject_id} / {mode}: {result['error']}")
        return
    rec = result.get("record") or {}
    m = rec.get("metrics") or {}
    latency = m.get("meanDurationMs", m.get("elapsedMilliseconds", "?"))
    ops = m.get("opsPerSecond")
    dev_name = device.get("name", device.get("id", "?"))
    case_payload = dict(rec.get("benchmarkCase") or {})
    case_label = str(case_payload.get("alias") or case_payload.get("stableId") or "")
    run_label = f"{subject_id} / {mode}"
    if case_label:
        run_label = f"{run_label} / {case_label}"
    print(f"  鉁?{run_label} / {dev_name}")
    print(f"      meanDurationMs: {latency} ms" + (f"   opsPerSecond: {ops:,.0f}" if ops else ""))


def _cmd_status(repo_root: Path, records_mod: Any, subject_id: str | None, do_all: bool) -> int:
    if subject_id:
        sids = [subject_id]
    elif do_all:
        sids = records_mod.list_subjects_with_records(repo_root)
    else:
        sids = records_mod.list_subjects_with_records(repo_root)

    if not sids:
        print("No benchmark records found.")
        print("Run: run benchmark --all --mode native --record")
        return 0

    for sid in sids:
        print(f"\n{sid}")
        # Latest per mode on any device
        from benchmark_records import _records_path, _iter_jsonl_reverse
        path = _records_path(repo_root, sid)
        seen: dict[str, Any] = {}
        for rec in _iter_jsonl_reverse(path):
            mode = rec.get("mode", "?")
            if mode not in seen:
                seen[mode] = rec
            if len(seen) >= 3:
                break
        for m, rec in seen.items():
            lat = rec.get("metrics", {}).get("meanDurationMs", "?")
            dev = rec.get("device", {}).get("id", "?")
            ts = rec.get("recordedAt", "?")
            print(f"  {m:15s}  {lat} ms   {dev}   {ts}")
    return 0


def _print_help() -> None:
    print("""usage: run benchmark [options]

Options:
  --subject <id>               Target subject (e.g. SolutionCorePack)
  --all                        All subjects that declare perf.release matrices
  --mode <managed|native|interpreter>  Execution mode (default: all modes)
  --record                     Run benchmark and record result to records.jsonl
  --dashboard [--open]         Generate HTML dashboard (optionally open in browser)
  --output <path>              Dashboard output path
  status [--subject <id>]      Show latest benchmark records

Examples:
  run benchmark --subject SolutionCorePack --mode native --record
  run benchmark --all --record
  run benchmark --dashboard --open
  run benchmark status --all
""")

