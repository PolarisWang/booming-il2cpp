from __future__ import annotations

from datetime import datetime, timezone
from pathlib import Path
from typing import Any
import sys

try:
    from ..core.common import read_json
    from . import benchmark_dashboard_generator as benchmark_dashboard_generator_module
    from . import capability_coverage as capability_coverage_module
    from . import declared_metadata_labels as declared_metadata_labels_module
    from . import public_specs as public_specs_module
    from . import registry as registry_module
    from . import verification_layout as verification_layout_module
    from . import workspace_declared_collection as workspace_declared_collection_module
    from . import workspace_manifests as workspace_manifests_module
except ImportError:
    root = Path(__file__).resolve().parents[1]
    sys.path.insert(0, str(root))
    from core.common import read_json
    from testing import benchmark_dashboard_generator as benchmark_dashboard_generator_module
    from testing import capability_coverage as capability_coverage_module
    from testing import declared_metadata_labels as declared_metadata_labels_module
    from testing import public_specs as public_specs_module
    from testing import registry as registry_module
    from testing import verification_layout as verification_layout_module
    from testing import workspace_declared_collection as workspace_declared_collection_module
    from testing import workspace_manifests as workspace_manifests_module


SOURCE_SCHEMA_VERSION = 1
MODE_ORDER = ("managed", "native", "interpreter")


def _utc_timestamp() -> str:
    return datetime.now(timezone.utc).isoformat().replace("+00:00", "Z")


def _relative(repo_root: Path, path: Path) -> str:
    try:
        return path.resolve().relative_to(repo_root.resolve()).as_posix()
    except ValueError:
        return path.as_posix()


def _int_value(value: Any) -> int:
    try:
        return int(value or 0)
    except (TypeError, ValueError):
        return 0


def _string_list(values: Any) -> list[str]:
    return [str(value) for value in list(values or []) if str(value)]


def _normalized_contract(item: dict[str, Any], *, repo_root: Path) -> dict[str, Any]:
    contract = capability_coverage_module.resolve_capability_contract(
        capability_family=item.get("capabilityFamily"),
        capability_item=item.get("capabilityItem"),
        repo_root=repo_root,
    )
    has_contract = bool(contract.get("ownerSubjectId")) or bool(contract.get("supportStates"))
    support_states = (
        [int(value) for value in list(contract.get("supportStates") or [])]
        if has_contract
        else [_int_value(value) for value in list(item.get("supportStates") or [])]
    )
    support_state_labels = (
        [str(value) for value in list(contract.get("supportStateLabels") or [])]
        if has_contract
        else _string_list(item.get("supportStateLabels"))
    )
    return {
        "capabilityFamily": _int_value(contract.get("capabilityFamily") or item.get("capabilityFamily")),
        "capabilityFamilyLabel": str(
            contract.get("capabilityFamilyLabel")
            or declared_metadata_labels_module.capability_family_label(item.get("capabilityFamily"))
        ),
        "capabilityItem": _int_value(contract.get("capabilityItem") or item.get("capabilityItem")),
        "capabilityItemLabel": str(
            contract.get("capabilityItemLabel")
            or declared_metadata_labels_module.capability_item_label(item.get("capabilityItem"))
        ),
        "ownerSubjectId": str(contract.get("ownerSubjectId") or item.get("ownerSubjectId") or ""),
        "supportStates": support_states,
        "supportStateLabels": support_state_labels
        or declared_metadata_labels_module.body_availability_labels(support_states),
        "proofRequired": bool(
            contract.get("proofRequired") if has_contract else item.get("proofRequired", False)
        ),
        "benchmarkRequired": bool(
            contract.get("benchmarkRequired") if has_contract else item.get("benchmarkRequired", False)
        ),
    }


def _normalize_declared_unit(item: dict[str, Any], *, repo_root: Path) -> dict[str, Any]:
    contract = _normalized_contract(item, repo_root=repo_root)
    return {
        "subjectId": str(item.get("subjectId") or ""),
        "stableId": str(item.get("stableId") or ""),
        "alias": str(item.get("alias") or ""),
        "assemblyName": str(item.get("assemblyName") or ""),
        "declaringType": str(item.get("declaringType") or ""),
        "methodName": str(item.get("methodName") or ""),
        "methodSignature": str(item.get("methodSignature") or ""),
        "sourceEntry": str(item.get("sourceEntry") or ""),
        "entryIndex": _int_value(item.get("entryIndex")),
        "category": _int_value(item.get("category")),
        "categoryLabel": str(
            item.get("categoryLabel") or declared_metadata_labels_module.unit_category_label(item.get("category"))
        ),
        **contract,
        "archetype": _int_value(item.get("archetype")),
        "archetypeLabel": str(
            item.get("archetypeLabel") or declared_metadata_labels_module.archetype_label(item.get("archetype"))
        ),
        "hotUpdateCapability": _int_value(item.get("hotUpdateCapability")),
        "hotUpdateCapabilityLabels": _string_list(item.get("hotUpdateCapabilityLabels"))
        or declared_metadata_labels_module.labels_from_mask(
            item.get("hotUpdateCapability"),
            declared_metadata_labels_module.HOT_UPDATE_CAPABILITY_LABELS,
        ),
        "requires": _int_value(item.get("requires")),
        "requirementLabels": _string_list(item.get("requirementLabels"))
        or declared_metadata_labels_module.labels_from_mask(
            item.get("requires"),
            declared_metadata_labels_module.RUNTIME_FEATURE_LABELS,
        ),
        "evidence": _int_value(item.get("evidence")),
        "evidenceLabels": _string_list(item.get("evidenceLabels"))
        or declared_metadata_labels_module.labels_from_mask(
            item.get("evidence"),
            declared_metadata_labels_module.EVIDENCE_LABELS,
        ),
        "priority": _int_value(item.get("priority")),
        "defaultGoalId": str(item.get("defaultGoalId") or ""),
        "defaultMatrixId": str(item.get("defaultMatrixId") or ""),
        "goalIds": _string_list(item.get("goalIds")),
        "matrixIds": _string_list(item.get("matrixIds")),
    }


def _normalize_declared_benchmark(item: dict[str, Any], *, repo_root: Path) -> dict[str, Any]:
    contract = _normalized_contract(item, repo_root=repo_root)
    return {
        "subjectId": str(item.get("subjectId") or ""),
        "stableId": str(item.get("stableId") or ""),
        "alias": str(item.get("alias") or ""),
        "assemblyName": str(item.get("assemblyName") or ""),
        "declaringType": str(item.get("declaringType") or ""),
        "methodName": str(item.get("methodName") or ""),
        "methodSignature": str(item.get("methodSignature") or ""),
        "sourceEntry": str(item.get("sourceEntry") or ""),
        "workloadEntry": str(item.get("workloadEntry") or ""),
        "entryIndex": _int_value(item.get("entryIndex")),
        "category": _int_value(item.get("category")),
        "categoryLabel": str(
            item.get("categoryLabel")
            or declared_metadata_labels_module.benchmark_category_label(item.get("category"))
        ),
        **contract,
        "archetype": _int_value(item.get("archetype")),
        "archetypeLabel": str(
            item.get("archetypeLabel") or declared_metadata_labels_module.archetype_label(item.get("archetype"))
        ),
        "hotUpdateCapability": _int_value(item.get("hotUpdateCapability")),
        "hotUpdateCapabilityLabels": _string_list(item.get("hotUpdateCapabilityLabels"))
        or declared_metadata_labels_module.labels_from_mask(
            item.get("hotUpdateCapability"),
            declared_metadata_labels_module.HOT_UPDATE_CAPABILITY_LABELS,
        ),
        "requires": _int_value(item.get("requires")),
        "requirementLabels": _string_list(item.get("requirementLabels"))
        or declared_metadata_labels_module.labels_from_mask(
            item.get("requires"),
            declared_metadata_labels_module.RUNTIME_FEATURE_LABELS,
        ),
        "metrics": _int_value(item.get("metrics")),
        "metricLabels": _string_list(item.get("metricLabels"))
        or declared_metadata_labels_module.labels_from_mask(
            item.get("metrics"),
            declared_metadata_labels_module.METRIC_LABELS,
        ),
        "modes": _int_value(item.get("modes")),
        "supportedModes": _string_list(item.get("supportedModes"))
        or declared_metadata_labels_module.supported_modes_from_mask(item.get("modes")),
        "warmupCount": _int_value(item.get("warmupCount")),
        "iterationCount": _int_value(item.get("iterationCount")),
        "invocationCount": _int_value(item.get("invocationCount")),
        "defaultGoalId": str(item.get("defaultGoalId") or ""),
        "defaultMatrixId": str(item.get("defaultMatrixId") or ""),
        "goalIds": _string_list(item.get("goalIds")),
        "matrixIds": _string_list(item.get("matrixIds")),
    }


def _collect_capability_contracts(repo_root: Path) -> list[dict[str, Any]]:
    subjects_root = verification_layout_module.owners_root(repo_root)
    feature_name_candidates = verification_layout_module.owner_features_name_candidates()
    contracts: list[dict[str, Any]] = []
    seen: set[tuple[str, int]] = set()
    if not subjects_root.is_dir():
        return contracts

    feature_paths: list[Path] = []
    for feature_name in feature_name_candidates:
        feature_paths.extend(sorted(subjects_root.glob(f"*/{feature_name}")))
        if feature_paths:
            break

    for feature_path in feature_paths:
        payload = read_json(feature_path)
        subject_id = str(payload.get("subjectId") or feature_path.parent.name).strip()
        for raw in list(payload.get("features") or []):
            if not isinstance(raw, dict):
                continue
            capability_item = _int_value(raw.get("capabilityItem"))
            if capability_item <= 0:
                continue
            key = (subject_id, capability_item)
            if key in seen:
                continue
            seen.add(key)
            contract = capability_coverage_module.resolve_capability_contract(
                capability_family=raw.get("capabilityFamily"),
                capability_item=capability_item,
                repo_root=repo_root,
            )
            contracts.append(
                {
                    "subjectId": subject_id,
                    "capabilityFamily": _int_value(contract.get("capabilityFamily") or raw.get("capabilityFamily")),
                    "capabilityFamilyLabel": str(
                        contract.get("capabilityFamilyLabel")
                        or declared_metadata_labels_module.capability_family_label(raw.get("capabilityFamily"))
                    ),
                    "capabilityItem": capability_item,
                    "capabilityItemLabel": str(
                        contract.get("capabilityItemLabel")
                        or declared_metadata_labels_module.capability_item_label(capability_item)
                    ),
                    "ownerSubjectId": str(contract.get("ownerSubjectId") or subject_id),
                    "supportStates": [int(value) for value in list(contract.get("supportStates") or raw.get("supportStates") or [])],
                    "supportStateLabels": _string_list(contract.get("supportStateLabels"))
                    or declared_metadata_labels_module.body_availability_labels(raw.get("supportStates") or []),
                    "proofRequired": bool(contract.get("proofRequired", raw.get("proofRequired", False))),
                    "benchmarkRequired": bool(contract.get("benchmarkRequired", raw.get("benchmarkRequired", False))),
                }
            )
    return sorted(contracts, key=lambda item: (item["subjectId"], item["capabilityFamily"], item["capabilityItem"]))


def _discover_local_subject_ids(repo_root: Path) -> list[str]:
    subject_ids: set[str] = set()
    subjects_root = verification_layout_module.owners_root(repo_root)
    manifest_name_candidates = verification_layout_module.owner_manifest_name_candidates()
    feature_name_candidates = verification_layout_module.owner_features_name_candidates()
    if subjects_root.is_dir():
        for manifest_name in [*feature_name_candidates, *manifest_name_candidates]:
            for manifest_path in sorted(subjects_root.glob(f"*/{manifest_name}")):
                if not manifest_path.is_file():
                    continue
                try:
                    payload = read_json(manifest_path)
                except Exception:
                    payload = {}
                subject_id = str(payload.get("subjectId") or manifest_path.parent.name).strip()
                if subject_id:
                    subject_ids.add(subject_id)

    workspace_root = verification_layout_module.subject_workspaces_root(repo_root)
    if workspace_root.is_dir():
        for manifest_path in sorted(workspace_root.glob("*/workspace.manifest.json")):
            if not manifest_path.is_file():
                continue
            try:
                payload = read_json(manifest_path)
            except Exception:
                payload = {}
            subject_id = str(payload.get("subjectId") or manifest_path.parent.name).strip()
            if subject_id:
                subject_ids.add(subject_id)

    return sorted(subject_ids)


def _collection_entries(collection: dict[str, Any], key: str) -> list[dict[str, Any]]:
    entries: list[dict[str, Any]] = []
    seen: set[str] = set()
    for raw in list(collection.get(key) or []):
        if not isinstance(raw, dict):
            continue
        stable_id = str(raw.get("stableId") or "").strip()
        if not stable_id or stable_id in seen:
            continue
        seen.add(stable_id)
        entries.append(
            {
                "stableId": stable_id,
                "alias": str(raw.get("alias") or ""),
                "entryIndex": _int_value(raw.get("entryIndex")),
            }
        )
    return entries


def _unit_test_lookup(declared_unit_tests: list[dict[str, Any]]) -> dict[str, dict[str, dict[str, Any]]]:
    by_subject_and_alias: dict[str, dict[str, dict[str, Any]]] = {}
    by_subject_and_type: dict[str, dict[str, dict[str, Any]]] = {}
    by_subject_and_source_entry: dict[str, dict[str, dict[str, Any]]] = {}
    for item in declared_unit_tests:
        subject_id = str(item.get("subjectId") or "").strip()
        alias = str(item.get("alias") or "").strip().lower()
        declaring_type = str(item.get("declaringType") or "").strip()
        type_name = declaring_type.rsplit(".", 1)[-1].lower() if declaring_type else ""
        source_entry = str(item.get("sourceEntry") or "").strip()
        if subject_id and alias:
            by_subject_and_alias.setdefault(subject_id, {})[alias] = item
        if subject_id and type_name:
            by_subject_and_type.setdefault(subject_id, {})[type_name] = item
        if subject_id and source_entry:
            by_subject_and_source_entry.setdefault(subject_id, {})[source_entry] = item
    return {
        "bySubjectAndAlias": by_subject_and_alias,
        "bySubjectAndType": by_subject_and_type,
        "bySubjectAndSourceEntry": by_subject_and_source_entry,
    }


def _managed_collection_path(manifest: dict[str, Any], *, host_kind: str) -> str:
    project = workspace_manifests_module.find_managed_test_project(manifest, host_kind=host_kind)
    if project is None:
        return ""
    return str(project.get("collectionPath") or "")


def _collect_workspace_collections(repo_root: Path, subject_ids: list[str]) -> list[dict[str, Any]]:
    records: list[dict[str, Any]] = []
    for subject_id in sorted({subject_id for subject_id in subject_ids if subject_id}):
        loaded_manifest = workspace_manifests_module.load_subject_workspace_manifest(repo_root, subject_id)
        if loaded_manifest is None:
            continue
        manifest_path, manifest = loaded_manifest
        proof_collection = workspace_declared_collection_module.load_workspace_declared_collection(
            repo_root,
            subject_id,
            host_kind="proof-host",
        )
        benchmark_collection = workspace_declared_collection_module.load_workspace_declared_collection(
            repo_root,
            subject_id,
            host_kind="benchmark-host",
        )
        if proof_collection is None and benchmark_collection is None:
            continue

        collection_path = (
            _managed_collection_path(manifest, host_kind="proof-host")
            or _managed_collection_path(manifest, host_kind="benchmark-host")
        )
        declared_unit_tests = _collection_entries(proof_collection or benchmark_collection or {}, "declaredUnitTests")
        declared_benchmarks = _collection_entries(benchmark_collection or proof_collection or {}, "declaredBenchmarks")
        records.append(
            {
                "subjectId": subject_id,
                "manifestPath": _relative(repo_root, manifest_path),
                "collectionPath": collection_path,
                "declaredUnitTests": declared_unit_tests,
                "declaredBenchmarks": declared_benchmarks,
            }
        )
    return records


def _stage_kind_from_runtime_profile(runtime_profile: str) -> str:
    normalized = runtime_profile.strip().lower()
    if "hotupdate" in normalized:
        return "hotupdate-proof"
    if "native" in normalized:
        return "native-proof"
    return "managed-proof"


def _stage_kind_from_pipeline_report(report: dict[str, Any]) -> str:
    matrix_proof_linkage = dict(report.get("matrixProofLinkage") or {})
    proof_kind = str(matrix_proof_linkage.get("proofKind") or "").strip()
    if proof_kind in {"managed-proof", "native-proof", "hotupdate-proof"}:
        return proof_kind
    selection = dict(report.get("selection") or {})
    execution_context = dict(selection.get("executionContext") or {})
    return _stage_kind_from_runtime_profile(str(execution_context.get("runtimeProfile") or ""))


def _record_status(status: str) -> str:
    normalized = status.strip().lower()
    if normalized in {"ok", "pass", "passed"}:
        return "ok"
    if normalized in {"fail", "failed"}:
        return "fail"
    if normalized in {"aborted", "abort"}:
        return "aborted"
    return normalized


def _pipeline_report_from_artifact(repo_root: Path, artifact_path: str) -> dict[str, Any] | None:
    path = repo_root / artifact_path
    if not path.is_file():
        return None
    try:
        payload = read_json(path)
    except Exception:
        return None
    return dict(payload) if isinstance(payload, dict) else None


def _stage_kind_from_run_artifacts(repo_root: Path, artifact_paths: list[str]) -> str:
    for artifact_path in artifact_paths:
        normalized = artifact_path.replace("\\", "/")
        if not normalized.endswith("/report.json"):
            continue
        report = _pipeline_report_from_artifact(repo_root, artifact_path)
        if report is None:
            continue
        stage_kind = _stage_kind_from_pipeline_report(report)
        if stage_kind in {"managed-proof", "native-proof", "hotupdate-proof"}:
            return stage_kind
    return "managed-proof"


def _proof_record(
    *,
    subject_id: str,
    stable_id: str,
    alias: str,
    entry_index: int,
    status: str,
    stage_kind: str,
    run_id: str,
    summary_path: Path,
    artifact_path: str,
    errors: list[str],
    repo_root: Path,
    dispatch_subject_id: str = "",
) -> dict[str, Any]:
    record = {
        "subjectId": subject_id,
        "stableId": stable_id,
        "alias": alias,
        "entryIndex": entry_index,
        "status": _record_status(status),
        "stageKind": stage_kind,
        "runId": run_id,
        "summaryPath": _relative(repo_root, summary_path),
        "errors": errors,
    }
    if artifact_path.endswith("pipeline-report/report.json"):
        record["pipelineReportPath"] = artifact_path
    else:
        record["declaredReportPath"] = artifact_path
    if dispatch_subject_id:
        record["dispatchSubjectId"] = dispatch_subject_id
    return record


def _read_json_dict(path: Path) -> dict[str, Any] | None:
    if not path.is_file():
        return None
    try:
        payload = read_json(path)
    except Exception:
        return None
    return dict(payload) if isinstance(payload, dict) else None


def _dispatch_subject_id_from_report(repo_root: Path, report: dict[str, Any]) -> str:
    for raw_stage in list(report.get("stageResults") or []):
        stage = dict(raw_stage or {})
        for evidence_path in _string_list(stage.get("primaryEvidencePaths")):
            if not evidence_path.endswith("native-reference.manifest.json"):
                continue
            payload = _read_json_dict(repo_root / evidence_path)
            if payload is None:
                continue
            subject_id = str(payload.get("preferredAssemblyDispatchSubjectId") or "").strip()
            if subject_id:
                return subject_id
    return ""


def _mixed_execution_legacy_stdout_matches(
    *,
    alias: str,
    stdout_text: str,
) -> bool:
    required_patterns = {
        "interpreter-arithmetic-proof": [
            "mixed-aot-to-interpreter=",
            "mixed-interpreter-local-call=",
        ],
        "mixed-generic-flow-proof": [
            "mixed-interpreter-string-bridge=",
        ],
        "mixed-exception-flow-proof": [
            "mixed-interpreter-real-catch=",
            "mixed-interpreter-real-rethrow-caught=",
            "mixed-interpreter-real-leave-finally=",
        ],
        "mixed-delegate-flow-proof": [
            "mixed-interpreter-to-aot=",
        ],
        "mixed-execution-proof": [
            "mixed-aot-to-interpreter-before-load=",
            "mixed-interpreter-to-engine=",
            "mixed-aot-to-interpreter-after-unload=",
        ],
    }
    patterns = required_patterns.get(alias, [])
    return bool(patterns) and all(pattern in stdout_text for pattern in patterns)


def _legacy_proof_stable_id(
    report: dict[str, Any],
    *,
    repo_root: Path,
    lookup: dict[str, dict[str, dict[str, Any]]],
) -> tuple[str, str, int, str]:
    selection = dict(report.get("selection") or {})
    entry_selection = dict(selection.get("entrySelection") or {})
    if str(entry_selection.get("family") or "").strip() == "declared-unit-test":
        stable_id = str(entry_selection.get("stableId") or "").strip()
        if stable_id:
            return (
                stable_id,
                str(entry_selection.get("alias") or "").strip(),
                _int_value(entry_selection.get("entryIndex")),
                "",
            )

    subject_id = str(report.get("subjectId") or "").strip()
    alias = str(entry_selection.get("alias") or "").strip().lower()
    if subject_id and alias:
        match = dict(lookup.get("bySubjectAndAlias", {}).get(subject_id, {})).get(alias)
        if match:
            return (
                str(match.get("stableId") or "").strip(),
                str(match.get("alias") or "").strip(),
                _int_value(match.get("entryIndex")),
                "",
            )

    dispatch_subject_id = _dispatch_subject_id_from_report(repo_root, report)
    type_name = ""
    if dispatch_subject_id:
        head = dispatch_subject_id.split("::", 1)[0]
        type_name = head.rsplit("/", 1)[-1].strip().lower()
    if subject_id and type_name:
        match = dict(lookup.get("bySubjectAndType", {}).get(subject_id, {})).get(type_name)
        if match:
            return (
                str(match.get("stableId") or "").strip(),
                str(match.get("alias") or "").strip(),
                _int_value(match.get("entryIndex")),
                dispatch_subject_id,
            )

    selection = dict(report.get("selection") or {})
    source = dict(selection.get("source") or {})
    source_entry = str(source.get("entry") or "").strip()
    runtime_profile = str(dict(selection.get("executionContext") or {}).get("runtimeProfile") or "").strip().lower()
    if subject_id and source_entry and "managed-output" in runtime_profile:
        match = dict(lookup.get("bySubjectAndSourceEntry", {}).get(subject_id, {})).get(source_entry)
        if match:
            return (
                str(match.get("stableId") or "").strip(),
                str(match.get("alias") or "").strip(),
                _int_value(match.get("entryIndex")),
                "",
            )

    return "", "", 0, dispatch_subject_id


def _legacy_managed_stdout_proof_matches(
    report: dict[str, Any],
    *,
    repo_root: Path,
    lookup: dict[str, dict[str, dict[str, Any]]],
) -> list[tuple[str, str, int]]:
    selection = dict(report.get("selection") or {})
    subject_id = str(report.get("subjectId") or "").strip()
    source_entry = str(dict(selection.get("source") or {}).get("entry") or "").strip()
    runtime_profile = str(dict(selection.get("executionContext") or {}).get("runtimeProfile") or "").strip().lower()
    if subject_id != "MixedExecutionFeaturePack":
        return []
    if source_entry != "MixedExecutionFeaturePack/MixedExecutionProofEntry::Run()":
        return []
    if "managed-output" not in runtime_profile:
        return []

    stdout_path = ""
    for raw_stage in list(report.get("stageResults") or []):
        stage = dict(raw_stage or {})
        if str(stage.get("kind") or "").strip() != "runtime-managed-output":
            continue
        diagnostics = dict(stage.get("diagnostics") or {})
        stdout_path = str(diagnostics.get("stdoutPath") or "").strip()
        if not stdout_path:
            for evidence_path in _string_list(stage.get("primaryEvidencePaths")):
                if evidence_path.endswith("stdout.log"):
                    stdout_path = evidence_path
                    break
        if stdout_path:
            break
    if not stdout_path:
        return []
    stdout_file = repo_root / stdout_path
    if not stdout_file.is_file():
        return []
    try:
        stdout_text = stdout_file.read_text(encoding="utf-8")
    except UnicodeDecodeError:
        stdout_text = stdout_file.read_text(encoding="utf-8", errors="replace")

    matches: list[tuple[str, str, int]] = []
    subject_aliases = dict(lookup.get("bySubjectAndAlias", {}).get(subject_id, {}))
    for alias in (
        "interpreter-arithmetic-proof",
        "mixed-generic-flow-proof",
        "mixed-exception-flow-proof",
        "mixed-delegate-flow-proof",
        "mixed-execution-proof",
    ):
        item = dict(subject_aliases.get(alias) or {})
        if not item:
            continue
        if not _mixed_execution_legacy_stdout_matches(alias=alias, stdout_text=stdout_text):
            continue
        matches.append(
            (
                str(item.get("stableId") or "").strip(),
                str(item.get("alias") or "").strip(),
                _int_value(item.get("entryIndex")),
            )
        )
    return [item for item in matches if item[0]]


def _collect_proof_evidence(
    repo_root: Path,
    *,
    declared_unit_tests: list[dict[str, Any]] | None = None,
) -> list[dict[str, Any]]:
    runs_root = repo_root / "artifacts" / "subjects"
    if not runs_root.is_dir():
        return []

    lookup = _unit_test_lookup(list(declared_unit_tests or []))
    latest_by_key: dict[tuple[str, str], tuple[float, dict[str, Any]]] = {}
    for summary_path in runs_root.rglob("run-report/summary.json"):
        if not summary_path.is_file():
            continue
        try:
            payload = read_json(summary_path)
        except Exception:
            continue
        if not isinstance(payload, dict):
            continue

        run_id = str(payload.get("runId") or "").strip()
        command = str(payload.get("command") or "").strip()
        final_status = _record_status(str(payload.get("finalStatus") or ""))
        errors = _string_list(payload.get("errors"))
        modified_at = summary_path.stat().st_mtime
        artifact_paths = _string_list(payload.get("artifacts"))
        subject_results = list(payload.get("subjectResults") or [])
        if command == "test declared-unit-test":
            stage_kind = _stage_kind_from_run_artifacts(repo_root, artifact_paths)
            for raw_result in subject_results:
                if not isinstance(raw_result, dict):
                    continue
                entry_selection = dict(raw_result.get("entrySelection") or {})
                if str(entry_selection.get("family") or "").strip() != "declared-unit-test":
                    continue
                stable_id = str(entry_selection.get("stableId") or "").strip()
                if not stable_id:
                    continue
                record = {
                    "subjectId": str(raw_result.get("subjectId") or "").strip(),
                    "stableId": stable_id,
                    "alias": str(entry_selection.get("alias") or "").strip(),
                    "entryIndex": _int_value(entry_selection.get("entryIndex")),
                    "status": _record_status(str(raw_result.get("status") or final_status or "")),
                    "stageKind": stage_kind,
                    "runId": run_id,
                    "summaryPath": _relative(repo_root, summary_path),
                    "subjectSummaryPath": str(raw_result.get("subjectSummaryPath") or "").strip(),
                    "errors": errors,
                }
                key = (stable_id, stage_kind)
                previous = latest_by_key.get(key)
                if previous is None or modified_at >= previous[0]:
                    latest_by_key[key] = (modified_at, record)
            continue

        if not command.endswith("test subject") and not command.startswith("run test subject"):
            continue

        for artifact_path in artifact_paths:
            if artifact_path.endswith("declared/unit") or artifact_path.endswith("declared/unit/"):
                continue
            if artifact_path.endswith("/report.json") and "/declared/unit/" in artifact_path.replace("\\", "/"):
                report = _pipeline_report_from_artifact(repo_root, artifact_path)
                if report is None:
                    continue
                stable_id, alias, entry_index, dispatch_subject_id = _legacy_proof_stable_id(
                    report,
                    repo_root=repo_root,
                    lookup=lookup,
                )
                if not stable_id:
                    continue
                stage_kind = _stage_kind_from_pipeline_report(report)
                if stage_kind not in {"managed-proof", "native-proof", "hotupdate-proof"}:
                    continue
                record = _proof_record(
                    subject_id=str(report.get("subjectId") or "").strip(),
                    stable_id=stable_id,
                    alias=alias,
                    entry_index=entry_index,
                    status=str(report.get("status") or final_status or ""),
                    stage_kind=stage_kind,
                    run_id=run_id,
                    summary_path=summary_path,
                    artifact_path=artifact_path,
                    errors=errors,
                    repo_root=repo_root,
                    dispatch_subject_id=dispatch_subject_id,
                )
                key = (stable_id, stage_kind)
                previous = latest_by_key.get(key)
                if previous is None or modified_at >= previous[0]:
                    latest_by_key[key] = (modified_at, record)
                continue
            if not artifact_path.endswith("pipeline-report/report.json"):
                continue
            report = _pipeline_report_from_artifact(repo_root, artifact_path)
            if report is None:
                continue
            stable_id, alias, entry_index, dispatch_subject_id = _legacy_proof_stable_id(
                report,
                repo_root=repo_root,
                lookup=lookup,
            )
            if not stable_id:
                continue
            stage_kind = _stage_kind_from_pipeline_report(report)
            if stage_kind not in {"managed-proof", "native-proof", "hotupdate-proof"}:
                continue
            record = _proof_record(
                subject_id=str(report.get("subjectId") or "").strip(),
                stable_id=stable_id,
                alias=alias,
                entry_index=entry_index,
                status=str(report.get("status") or final_status or ""),
                stage_kind=stage_kind,
                run_id=run_id,
                summary_path=summary_path,
                artifact_path=artifact_path,
                errors=errors,
                repo_root=repo_root,
                dispatch_subject_id=dispatch_subject_id,
            )
            key = (stable_id, stage_kind)
            previous = latest_by_key.get(key)
            if previous is None or modified_at >= previous[0]:
                latest_by_key[key] = (modified_at, record)

            if stage_kind == "managed-proof":
                for extra_stable_id, extra_alias, extra_entry_index in _legacy_managed_stdout_proof_matches(
                    report,
                    repo_root=repo_root,
                    lookup=lookup,
                ):
                    extra_record = _proof_record(
                        subject_id=str(report.get("subjectId") or "").strip(),
                        stable_id=extra_stable_id,
                        alias=extra_alias,
                        entry_index=extra_entry_index,
                        status=str(report.get("status") or final_status or ""),
                        stage_kind=stage_kind,
                        run_id=run_id,
                        summary_path=summary_path,
                        artifact_path=artifact_path,
                        errors=errors,
                        repo_root=repo_root,
                    )
                    extra_key = (extra_stable_id, stage_kind)
                    previous = latest_by_key.get(extra_key)
                    if previous is None or modified_at >= previous[0]:
                        latest_by_key[extra_key] = (modified_at, extra_record)

    return [
        record
        for _, record in sorted(
            latest_by_key.values(),
            key=lambda item: (
                str(item[1].get("subjectId") or ""),
                str(item[1].get("stableId") or ""),
                str(item[1].get("stageKind") or ""),
            ),
        )
    ]


def _device_platform(subject_payload: dict[str, Any], device_id: str) -> tuple[str, str]:
    platforms = dict(subject_payload.get("platforms") or {})
    for platform_id, raw_platform in platforms.items():
        platform = dict(raw_platform or {})
        if str(platform.get("deviceId") or "") == device_id:
            return str(platform_id), str(platform.get("deviceName") or "")
    if len(platforms) == 1:
        platform_id, raw_platform = next(iter(platforms.items()))
        return str(platform_id), str(dict(raw_platform or {}).get("deviceName") or "")
    return "", ""


def _normalize_mode_status(mode_status: dict[str, Any]) -> dict[str, dict[str, Any]]:
    normalized: dict[str, dict[str, Any]] = {}
    for mode in MODE_ORDER:
        raw = dict(mode_status.get(mode) or {})
        if not raw:
            continue
        payload: dict[str, Any] = {
            "mode": mode,
            "status": str(raw.get("status") or ""),
        }
        for key in ("recordedAt", "gitCommit", "reasonCode", "reasonLabel", "staleReasonCode", "staleReasonLabel"):
            value = str(raw.get(key) or "")
            if value:
                payload[key] = value
        if "isStale" in raw:
            payload["isStale"] = bool(raw.get("isStale"))
        metrics = dict(raw.get("metrics") or {})
        if metrics:
            payload["metrics"] = metrics
        normalized[mode] = payload
    return normalized


def _normalize_benchmark_evidence_row(
    repo_root: Path,
    *,
    subject_path: Path,
    subject_payload: dict[str, Any],
    subject_id: str,
    device_id: str,
    case_id: str,
    raw_case: dict[str, Any],
) -> dict[str, Any]:
    stable_id = str(raw_case.get("stableId") or raw_case.get("caseId") or case_id)
    platform_id, platform_device_name = _device_platform(subject_payload, device_id)
    device_name = str(raw_case.get("deviceName") or platform_device_name or "")
    contract = _normalized_contract(raw_case, repo_root=repo_root)
    return {
        "subjectId": subject_id,
        "caseId": str(raw_case.get("caseId") or case_id or stable_id),
        "stableId": stable_id,
        "alias": str(raw_case.get("alias") or ""),
        "displayName": str(raw_case.get("displayName") or raw_case.get("alias") or stable_id),
        "workloadEntry": str(raw_case.get("workloadEntry") or ""),
        "entryIndex": _int_value(raw_case.get("entryIndex")),
        "assemblyName": str(raw_case.get("assemblyName") or ""),
        "declaringType": str(raw_case.get("declaringType") or ""),
        "methodName": str(raw_case.get("methodName") or ""),
        "methodSignature": str(raw_case.get("methodSignature") or ""),
        "category": _int_value(raw_case.get("category")),
        "categoryLabel": str(
            raw_case.get("categoryLabel")
            or declared_metadata_labels_module.benchmark_category_label(raw_case.get("category"))
        ),
        "metrics": _int_value(raw_case.get("metrics")),
        "metricLabels": _string_list(raw_case.get("metricLabels"))
        or declared_metadata_labels_module.labels_from_mask(
            raw_case.get("metrics"),
            declared_metadata_labels_module.METRIC_LABELS,
        ),
        "modes": _int_value(raw_case.get("modes")),
        "supportedModes": _string_list(raw_case.get("supportedModes"))
        or declared_metadata_labels_module.supported_modes_from_mask(raw_case.get("modes")),
        "requires": _int_value(raw_case.get("requires")),
        "requirementLabels": _string_list(raw_case.get("requirementLabels"))
        or declared_metadata_labels_module.labels_from_mask(
            raw_case.get("requires"),
            declared_metadata_labels_module.RUNTIME_FEATURE_LABELS,
        ),
        "archetype": _int_value(raw_case.get("archetype")),
        "archetypeLabel": str(
            raw_case.get("archetypeLabel") or declared_metadata_labels_module.archetype_label(raw_case.get("archetype"))
        ),
        "hotUpdateCapability": _int_value(raw_case.get("hotUpdateCapability")),
        "hotUpdateCapabilityLabels": _string_list(raw_case.get("hotUpdateCapabilityLabels"))
        or declared_metadata_labels_module.labels_from_mask(
            raw_case.get("hotUpdateCapability"),
            declared_metadata_labels_module.HOT_UPDATE_CAPABILITY_LABELS,
        ),
        "warmupCount": _int_value(raw_case.get("warmupCount")),
        "iterationCount": _int_value(raw_case.get("iterationCount")),
        "invocationCount": _int_value(raw_case.get("invocationCount")),
        **contract,
        "supportedModesDeclared": _string_list(raw_case.get("supportedModes")),
        "recordedModes": _string_list(raw_case.get("recordedModes")),
        "missingModes": _string_list(raw_case.get("missingModes")),
        "staleModes": _string_list(raw_case.get("staleModes")),
        "unsupportedModes": _string_list(raw_case.get("unsupportedModes")),
        "deviceId": str(raw_case.get("deviceId") or device_id),
        "deviceName": device_name,
        "platformId": str(raw_case.get("platformId") or platform_id),
        "modeStatus": _normalize_mode_status(dict(raw_case.get("modeStatus") or {})),
        "lastRecordedAt": str(raw_case.get("lastRecordedAt") or ""),
        "gitCommit": str(raw_case.get("gitCommit") or ""),
        "isStale": bool(raw_case.get("isStale", False)),
        "sourceSubjectPath": _relative(repo_root, subject_path),
    }


def _benchmark_records_path(repo_root: Path, subject_id: str) -> Path:
    return verification_layout_module.raw_benchmark_records_path(repo_root, subject_id)


def _benchmark_declared_lookup(
    declared_benchmarks: list[dict[str, Any]],
) -> dict[str, dict[str, dict[str, Any]]]:
    by_subject: dict[str, dict[str, dict[str, Any]]] = {}
    for item in declared_benchmarks:
        subject_id = str(item.get("subjectId") or "").strip()
        workload_entry = str(item.get("workloadEntry") or "").strip()
        if not subject_id or not workload_entry:
            continue
        by_subject.setdefault(subject_id, {})[workload_entry] = dict(item)
    return by_subject


def _benchmark_mode_from_runtime_profile(runtime_profile: str) -> str:
    normalized = runtime_profile.strip().lower()
    if "interpreter" in normalized:
        return "interpreter"
    if "native" in normalized:
        return "native"
    return "managed"


def _empty_mode_status_payload(mode: str, *, status: str) -> dict[str, Any]:
    if status == "recorded":
        return {"mode": mode, "status": status}
    if status == "unsupported":
        return {
            "mode": mode,
            "status": status,
            "reasonCode": "unsupported-by-contract",
            "reasonLabel": "This mode is not declared by the benchmark contract.",
        }
    return {
        "mode": mode,
        "status": status,
        "reasonCode": "missing-record",
        "reasonLabel": "Declared by case contract, but no benchmark record was found.",
    }


def _legacy_benchmark_metrics(report: dict[str, Any], perf_summary: dict[str, Any]) -> dict[str, Any]:
    for source in (
        dict(perf_summary.get("metrics") or {}),
        dict(report.get("metrics") or {}),
        dict(dict(report.get("performance") or {}).get("metrics") or {}),
    ):
        if source:
            return source
    return {}


def _merge_legacy_benchmark_row(
    bucket: dict[tuple[str, str, str, str], dict[str, Any]],
    *,
    declared: dict[str, Any],
    mode: str,
    metrics: dict[str, Any],
    summary_path: str,
    platform_id: str,
    device_id: str,
    device_name: str,
    git_commit: str,
    recorded_at: str,
) -> None:
    key = (
        str(declared.get("stableId") or ""),
        str(declared.get("subjectId") or ""),
        platform_id,
        device_id,
    )
    supported_modes = _string_list(declared.get("supportedModes"))
    if not supported_modes:
        supported_modes = declared_metadata_labels_module.supported_modes_from_mask(declared.get("modes"))
    row = bucket.setdefault(
        key,
        {
            **dict(declared),
            "deviceId": device_id,
            "deviceName": device_name,
            "platformId": platform_id,
            "supportedModes": supported_modes,
            "recordedModes": [],
            "missingModes": [],
            "staleModes": [],
            "unsupportedModes": [],
            "modeStatus": {},
            "sourceSubjectPath": summary_path,
            "lastRecordedAt": recorded_at,
            "gitCommit": git_commit,
            "isStale": False,
        },
    )
    row["sourceSubjectPath"] = summary_path
    if recorded_at:
        row["lastRecordedAt"] = recorded_at
    if git_commit:
        row["gitCommit"] = git_commit
    row.setdefault("modeStatus", {})[mode] = {
        "mode": mode,
        "status": "recorded",
        "metrics": dict(metrics),
        "recordedAt": recorded_at,
        "gitCommit": git_commit,
        "reasonCode": "recorded",
        "reasonLabel": "Benchmark record captured.",
    }
    for known_mode in MODE_ORDER:
        if known_mode in row["modeStatus"]:
            continue
        if known_mode in supported_modes:
            row["modeStatus"][known_mode] = _empty_mode_status_payload(known_mode, status="missing")
        else:
            row["modeStatus"][known_mode] = _empty_mode_status_payload(known_mode, status="unsupported")
    row["recordedModes"] = [current for current in MODE_ORDER if row["modeStatus"].get(current, {}).get("status") == "recorded"]
    row["missingModes"] = [current for current in MODE_ORDER if row["modeStatus"].get(current, {}).get("status") == "missing"]
    row["unsupportedModes"] = [
        current for current in MODE_ORDER if row["modeStatus"].get(current, {}).get("status") == "unsupported"
    ]


def _collect_legacy_benchmark_evidence(
    repo_root: Path,
    *,
    declared_benchmarks: list[dict[str, Any]],
) -> list[dict[str, Any]]:
    runs_root = repo_root / "artifacts" / "subjects"
    if not runs_root.is_dir():
        return []

    declared_lookup = _benchmark_declared_lookup(declared_benchmarks)
    if not declared_lookup:
        return []

    rows: dict[tuple[str, str, str, str], dict[str, Any]] = {}
    for summary_path in runs_root.rglob("run-report/summary.json"):
        if not summary_path.is_file():
            continue
        try:
            payload = read_json(summary_path)
        except Exception:
            continue
        if not isinstance(payload, dict):
            continue
        command = str(payload.get("command") or "").strip()
        if "test subject" not in command:
            continue
        subject_id = str(dict(list(payload.get("subjectResults") or [{}])[0]).get("subjectId") or "").strip()
        declared_by_workload = declared_lookup.get(subject_id)
        if not declared_by_workload:
            continue
        for artifact_path in _string_list(payload.get("artifacts")):
            if not artifact_path.endswith("pipeline-report/report.json"):
                continue
            report = _pipeline_report_from_artifact(repo_root, artifact_path)
            if report is None:
                continue
            if str(report.get("goalId") or "").strip() != "perf.release":
                continue
            selection = dict(report.get("selection") or {})
            workload_entry = str(selection.get("workloadEntry") or "").strip()
            declared = declared_by_workload.get(workload_entry)
            if declared is None:
                continue
            metrics = _legacy_benchmark_metrics(report, {})
            if not metrics:
                continue
            execution_context = dict(selection.get("executionContext") or {})
            platform_id = str(
                execution_context.get("targetPlatform")
                or execution_context.get("hostPlatform")
                or "windows-x64"
            )
            mode = _benchmark_mode_from_runtime_profile(str(execution_context.get("runtimeProfile") or ""))
            _merge_legacy_benchmark_row(
                rows,
                declared=declared,
                mode=mode,
                metrics=metrics,
                summary_path=_relative(repo_root, summary_path.parent.parent / "matrices" / str(report.get("matrixId") or "") / "validations" / "perf" / "summary.json"),
                platform_id=platform_id,
                device_id="legacy-host",
                device_name="Legacy Host",
                git_commit=str(report.get("gitCommit") or ""),
                recorded_at=str(report.get("generatedAt") or ""),
            )
            rows[(
                str(declared.get("stableId") or ""),
                str(declared.get("subjectId") or ""),
                platform_id,
                "legacy-host",
            )]["legacyCompatibilityClaim"] = True
    return sorted(
        rows.values(),
        key=lambda item: (str(item.get("stableId") or ""), str(item.get("deviceId") or ""), str(item.get("platformId") or "")),
    )


def _capability_id(item: dict[str, Any]) -> str:
    return f"capability/{_int_value(item.get('capabilityFamily'))}/{_int_value(item.get('capabilityItem'))}"


def _codegen_route_from_profile(profile_code: str) -> str:
    normalized = profile_code.strip().lower()
    if "hotupdate" in normalized:
        return "hotupdate"
    if "managed" in normalized:
        return "managed"
    return "native"


def _managed_source_refs_for_capability(
    *,
    owner_subject_id: str,
    capability_family: int,
    capability_item: int,
    declared_unit_tests: list[dict[str, Any]],
    declared_benchmarks: list[dict[str, Any]],
) -> list[dict[str, str]]:
    refs: list[dict[str, str]] = []
    seen: set[tuple[str, str]] = set()
    for item in [*declared_unit_tests, *declared_benchmarks]:
        if str(item.get("ownerSubjectId") or "") != owner_subject_id:
            continue
        if _int_value(item.get("capabilityFamily")) != capability_family:
            continue
        if _int_value(item.get("capabilityItem")) != capability_item:
            continue
        label = str(item.get("sourceEntry") or item.get("workloadEntry") or item.get("stableId") or "").strip()
        if not label:
            continue
        key = (f"artifact/verification-catalog/catalog/owners/{owner_subject_id}", label)
        if key in seen:
            continue
        seen.add(key)
        refs.append(
            {
                "kind": "managed-source",
                "path": key[0],
                "label": label,
                "hash": "",
            }
        )
    return refs


def _generated_from_evidence_ids(
    *,
    owner_subject_id: str,
    capability_family: int,
    capability_item: int,
    route_code: str,
    declared_unit_tests: list[dict[str, Any]],
) -> list[str]:
    stage_kind = {
        "managed": "managed-proof",
        "native": "native-proof",
        "hotupdate": "hotupdate-proof",
    }.get(route_code, "native-proof")
    evidence_ids: list[str] = []
    seen: set[str] = set()
    for item in declared_unit_tests:
        if str(item.get("ownerSubjectId") or "") != owner_subject_id:
            continue
        if _int_value(item.get("capabilityFamily")) != capability_family:
            continue
        if _int_value(item.get("capabilityItem")) != capability_item:
            continue
        stable_id = str(item.get("stableId") or "").strip()
        if not stable_id:
            continue
        evidence_id = f"evidence-claim/unit/{stable_id}/{stage_kind}"
        if evidence_id in seen:
            continue
        seen.add(evidence_id)
        evidence_ids.append(evidence_id)
    return evidence_ids


def _collect_codegen_stubs(
    repo_root: Path,
    *,
    capability_contracts: list[dict[str, Any]],
    declared_unit_tests: list[dict[str, Any]],
    declared_benchmarks: list[dict[str, Any]],
) -> list[dict[str, Any]]:
    stubs: list[dict[str, Any]] = []
    for contract in capability_contracts:
        owner_subject_id = str(contract.get("ownerSubjectId") or "").strip()
        if not owner_subject_id:
            continue
        capability_family = _int_value(contract.get("capabilityFamily"))
        capability_item = _int_value(contract.get("capabilityItem"))
        managed_source_refs = _managed_source_refs_for_capability(
            owner_subject_id=owner_subject_id,
            capability_family=capability_family,
            capability_item=capability_item,
            declared_unit_tests=declared_unit_tests,
            declared_benchmarks=declared_benchmarks,
        )
        if not managed_source_refs:
            continue
        baseline_root = verification_layout_module.owner_codegen_stubs_root(repo_root, owner_subject_id)
        if not baseline_root.is_dir():
            continue
        for profile_dir in sorted(path for path in baseline_root.iterdir() if path.is_dir()):
            profile_code = profile_dir.name
            route_code = _codegen_route_from_profile(profile_code)
            stubs.append(
                {
                    "ownerSubjectId": owner_subject_id,
                    "capabilityId": _capability_id(contract),
                    "routeCode": route_code,
                    "profileCode": profile_code,
                    "managedSourceRefs": managed_source_refs,
                    "stubRefs": [
                        {
                            "kind": "stub-dir",
                            "path": _relative(repo_root, profile_dir),
                            "label": profile_code,
                            "hash": "",
                        }
                    ],
                    "generatedFromEvidenceIds": _generated_from_evidence_ids(
                        owner_subject_id=owner_subject_id,
                        capability_family=capability_family,
                        capability_item=capability_item,
                        route_code=route_code,
                        declared_unit_tests=declared_unit_tests,
                    ),
                }
            )
    return sorted(
        stubs,
        key=lambda item: (
            str(item.get("ownerSubjectId") or ""),
            str(item.get("capabilityId") or ""),
            str(item.get("routeCode") or ""),
            str(item.get("profileCode") or ""),
        ),
    )


def _empty_benchmark_projection(repo_root: Path) -> tuple[dict[str, Any], dict[str, dict[str, Any]], list[dict[str, Any]]]:
    try:
        overview = benchmark_dashboard_generator_module.build_dashboard_overview({})
    except Exception:
        overview = {}
    return dict(overview or {}), {}, []


def _load_benchmark_projection(
    repo_root: Path,
    *,
    subject_ids: list[str],
) -> tuple[dict[str, Any], dict[str, dict[str, Any]], list[dict[str, Any]]]:
    try:
        data = benchmark_dashboard_generator_module.collect_dashboard_data(
            repo_root,
            sorted({subject_id for subject_id in subject_ids if subject_id}),
        )
        overview = benchmark_dashboard_generator_module.build_dashboard_overview(data)
    except Exception:
        return _empty_benchmark_projection(repo_root)

    evidence_rows: list[dict[str, Any]] = []
    for subject_id, payload in sorted(dict(data or {}).items()):
        subject_payload = dict(payload or {})
        cases_by_device = dict(subject_payload.get("benchmarkCasesByDevice") or {})
        for device_id, raw_cases in cases_by_device.items():
            if not isinstance(raw_cases, dict):
                continue
            for case_id, raw_case in sorted(raw_cases.items()):
                if not isinstance(raw_case, dict):
                    continue
                evidence_rows.append(
                    _normalize_benchmark_evidence_row(
                        repo_root,
                        subject_path=_benchmark_records_path(repo_root, subject_id),
                        subject_payload=subject_payload,
                        subject_id=subject_id,
                        device_id=str(device_id),
                        case_id=str(case_id),
                        raw_case=raw_case,
                    )
                )
    return dict(overview or {}), {subject_id: dict(payload) for subject_id, payload in dict(data or {}).items()}, evidence_rows


def collect_inventory_source(repo_root: Path, *, host_platform: str) -> dict[str, Any]:
    registry_index = registry_module.scan_registry(
        repo_root,
        host_platform=host_platform,
        public_suite_specs=public_specs_module.PUBLIC_TEST_SPECS,
    )
    declared_unit_tests = [
        _normalize_declared_unit(dict(item), repo_root=repo_root)
        for item in list(registry_index.declared_unit_tests)
    ]
    declared_benchmarks = [
        _normalize_declared_benchmark(dict(item), repo_root=repo_root)
        for item in list(registry_index.declared_benchmarks)
    ]
    subject_ids = [
        str(item.get("subjectId") or "")
        for item in [*declared_unit_tests, *declared_benchmarks, *list(registry_index.subjects)]
        if str(item.get("subjectId") or "")
    ]
    subject_ids = sorted({*subject_ids, *_discover_local_subject_ids(repo_root)})
    capability_contracts = _collect_capability_contracts(repo_root)
    workspace_collections = _collect_workspace_collections(repo_root, subject_ids)
    proof_evidence = _collect_proof_evidence(repo_root, declared_unit_tests=declared_unit_tests)
    benchmark_overview, benchmark_subjects, benchmark_evidence = _load_benchmark_projection(
        repo_root,
        subject_ids=subject_ids,
    )
    legacy_benchmark_evidence = _collect_legacy_benchmark_evidence(
        repo_root,
        declared_benchmarks=declared_benchmarks,
    )
    merged_benchmark_evidence: dict[tuple[str, str, str], dict[str, Any]] = {}
    for item in [*legacy_benchmark_evidence, *benchmark_evidence]:
        key = (
            str(item.get("stableId") or ""),
            str(item.get("deviceId") or ""),
            str(item.get("platformId") or ""),
        )
        if key[0]:
            merged_benchmark_evidence[key] = dict(item)
    codegen_stubs = _collect_codegen_stubs(
        repo_root,
        capability_contracts=capability_contracts,
        declared_unit_tests=declared_unit_tests,
        declared_benchmarks=declared_benchmarks,
    )

    return {
        "schemaVersion": SOURCE_SCHEMA_VERSION,
        "generatedAt": _utc_timestamp(),
        "hostPlatform": host_platform,
        "registryErrors": list(registry_index.errors),
        "registryWarnings": list(registry_index.warnings),
        "benchmarkOverview": benchmark_overview,
        "declaredUnitTests": sorted(declared_unit_tests, key=lambda item: item["stableId"]),
        "declaredBenchmarks": sorted(declared_benchmarks, key=lambda item: (item["stableId"], item["workloadEntry"])),
        "capabilityContracts": capability_contracts,
        "workspaceCollections": workspace_collections,
        "proofEvidence": proof_evidence,
        "benchmarkSubjects": benchmark_subjects,
        "benchmarkEvidence": sorted(
            merged_benchmark_evidence.values(),
            key=lambda item: (item["stableId"], item["deviceId"], item["platformId"]),
        ),
        "codegenStubs": codegen_stubs,
    }
