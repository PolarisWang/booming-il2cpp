from __future__ import annotations

import json
import sys
from pathlib import Path
from typing import Any, Callable

try:
    from .. import manifest as manifest_module
    from .. import tui as tui_module
    from ..testing import catalog as catalog_module
    from ..testing import contracts as contracts_module
    from ..testing import events as events_module
    from ..testing import public_specs as public_specs_module
    from ..testing import registry as registry_module
    from ..testing import selectors as selectors_module
    from ..testing import session as session_module
    from ..testing import reporting as reporting_module
    from ..testing import subject_executor as subject_executor_module
    from ..testing import subject_planner as subject_planner_module
    from ..testing import subject_reporting as subject_reporting_module
    from ..testing import subject_validations as subject_validations_module
    from .. import tooling as tooling_module
    from ..common import combine_process_output, run_process
    from ..commands import build as build_commands
    from ..result import CommandResult
except ImportError:
    root = Path(__file__).resolve().parents[1]
    sys.path.insert(0, str(root))
    import manifest as manifest_module
    import tui as tui_module
    from testing import catalog as catalog_module
    from testing import contracts as contracts_module
    from testing import events as events_module
    from testing import public_specs as public_specs_module
    from testing import registry as registry_module
    from testing import selectors as selectors_module
    from testing import session as session_module
    from testing import reporting as reporting_module
    from testing import subject_executor as subject_executor_module
    from testing import subject_planner as subject_planner_module
    from testing import subject_reporting as subject_reporting_module
    from testing import subject_validations as subject_validations_module
    import tooling as tooling_module
    from common import combine_process_output, run_process
    from commands import build as build_commands
    from result import CommandResult


PUBLIC_TEST_SPECS = public_specs_module.PUBLIC_TEST_SPECS


def find_public_test_suite_spec(family: str | None, suite: str | None) -> dict | None:
    return public_specs_module.find_public_test_suite_spec(family, suite)


def list_public_test_suites(manifest: dict, host_platform: str) -> list[dict]:
    del manifest
    return public_specs_module.list_public_test_suites(host_platform)


def resolve_legacy_test_command_id(family: str, suite: str, *, stage: str, host_platform: str) -> str | None:
    return public_specs_module.resolve_legacy_test_command_id(family, suite, stage=stage, host_platform=host_platform)


def _render_public_test_list(items: list[dict]) -> str:
    lines = ["Available test suites", ""]
    for item in items:
        lines.append(f"{item['id']}: stages={','.join(item['stages'])}")
    return "\n".join(lines) + "\n"


def _handle_public_test_list(command_text: str, host_platform: str, options: dict, manifest: dict) -> CommandResult:
    family = options.get("family")
    items = list_public_test_suites(manifest, host_platform)
    if family:
        items = [item for item in items if item["family"] == family]

    return CommandResult.success(
        command=command_text,
        host_platform=host_platform,
        target=family,
        payload={"items": items},
        text=_render_public_test_list(items),
    )


def _scan_registry(repo_root: Path, host_platform: str) -> registry_module.RegistryIndex:
    return registry_module.scan_registry(
        repo_root,
        host_platform=host_platform,
        public_suite_specs=PUBLIC_TEST_SPECS,
    )


def _render_registry_list(index: registry_module.RegistryIndex) -> str:
    lines = ["Unified Test Registry", ""]
    for title, items in (
        ("Suites", index.suites),
        ("Subjects", index.subjects),
        ("Module Verifications", index.module_verifications),
        ("System Scenarios", index.system_scenarios),
        ("Pipelines", index.pipelines),
    ):
        lines.append(f"{title}:")
        for item in items:
            lines.append(f"- {item['id']}")
    return "\n".join(lines) + "\n"


def _handle_registry_dispatch(
    command: dict,
    repo_root: Path,
    host_platform: str,
    command_text: str,
) -> CommandResult:
    index = _scan_registry(repo_root, host_platform)
    if command["id"] == "test-registry-refresh":
        snapshot = registry_module.write_registry_snapshot(repo_root, index)
        payload = {
            "currentPath": str(snapshot["currentPath"].relative_to(repo_root).as_posix()),
            "historyPath": str(snapshot["historyPath"].relative_to(repo_root).as_posix()),
            "flatItems": index.flat_items,
            "errors": index.errors,
            "warnings": index.warnings,
        }
        status_factory = CommandResult.failure if index.errors else CommandResult.success
        kwargs = {
            "command": command_text,
            "host_platform": host_platform,
            "target": "registry",
            "payload": payload,
            "text": _render_registry_list(index),
        }
        if index.errors:
            kwargs["errors"] = list(index.errors)
        return status_factory(**kwargs)

    if command["id"] == "test-registry-list":
        return CommandResult.success(
            command=command_text,
            host_platform=host_platform,
            target="registry",
            payload=index.to_dict(),
            text=_render_registry_list(index),
        )

    errors = list(index.errors)
    warnings = list(index.warnings)
    lines = ["Registry Consistency Check", ""]
    if errors:
        lines.append("Errors:")
        lines.extend(errors)
    if warnings:
        lines.append("Warnings:")
        lines.extend(warnings)
    if not errors and not warnings:
        lines.append("ok")

    if errors:
        return CommandResult.failure(
            command=command_text,
            host_platform=host_platform,
            target="registry",
            errors=errors,
            payload={"errors": errors, "warnings": warnings, "exitCode": 2},
            text="\n".join(lines) + "\n",
        )

    return CommandResult.success(
        command=command_text,
        host_platform=host_platform,
        target="registry",
        payload={"errors": errors, "warnings": warnings},
        text="\n".join(lines) + "\n",
    )


def _logs_root(repo_root: Path) -> Path:
    return repo_root / "artifacts" / "logs" / "tests"


def _find_subject_run_metadata(repo_root: Path, requested_run: str) -> dict[str, Any] | None:
    subject_root = repo_root / "artifacts" / "subjects"
    if not subject_root.is_dir():
        return None

    for summary_path in subject_root.glob(f"*/runs/{requested_run}/run-report/summary.json"):
        run_root = summary_path.parent
        events_path = run_root / "events.jsonl"
        return {
            "runId": requested_run,
            "summaryPath": str(summary_path.relative_to(repo_root).as_posix()),
            "eventsPath": str(events_path.relative_to(repo_root).as_posix()),
        }
    return None


def _read_json_if_exists(path: Path) -> dict[str, Any] | None:
    if not path.is_file():
        return None
    return json.loads(path.read_text(encoding="utf-8"))


def _resolve_run_metadata(repo_root: Path, requested_run: str | None, *, prefer_current: bool) -> dict[str, Any] | None:
    logs_root = _logs_root(repo_root)
    if requested_run and requested_run not in {"last", "current"}:
        run_root = logs_root / requested_run
        if run_root.exists():
            return {
                "runId": requested_run,
                "summaryPath": str((run_root / "summary.json").relative_to(repo_root).as_posix()),
                "eventsPath": str((run_root / "events.jsonl").relative_to(repo_root).as_posix()),
            }
        return _find_subject_run_metadata(repo_root, requested_run)

    pointer_names: list[str] = []
    if requested_run == "current" or (requested_run is None and prefer_current):
        pointer_names.append("current.json")
    if requested_run in {None, "last"}:
        pointer_names.append("last.json")
    if requested_run == "current" and "last.json" not in pointer_names:
        pointer_names.append("last.json")

    for pointer_name in pointer_names:
        payload = _read_json_if_exists(logs_root / pointer_name)
        if payload is not None:
            return payload
    return None


def _render_summary(summary: dict[str, Any]) -> str:
    lines = ["Unified Test Summary"]
    planned_counts = dict(summary.get("plannedCounts") or {})
    counts_by_family = dict(summary.get("countsByFamily") or {})
    subject_results = list(summary.get("subjectResults") or [])
    failure_items = list(summary.get("failureItems") or [])
    phase_results = list(summary.get("phaseResults") or [])
    suite_results = list(summary.get("suiteResults") or [])

    lines.extend(
        [
            "",
            "Overall:",
            f"Run: {summary.get('runId', '-')}",
            f"Command: {summary.get('command', '-')}",
            f"Host: {summary.get('hostPlatform', '-')}",
            f"Status: {summary.get('finalStatus', '-')}",
        ]
    )
    if planned_counts:
        lines.extend(
            [
                f"Total units: {planned_counts.get('totalUnits', 0)}",
                f"Suite count: {planned_counts.get('suiteCount', 0)}",
                f"Subject count: {planned_counts.get('subjectCount', 0)}",
            ]
        )
    errors = list(summary.get("errors") or [])
    if errors:
        lines.append("Errors:")
        lines.extend(str(error) for error in errors)

    if phase_results:
        lines.append("")
        lines.append("Phases:")
        lines.extend(f"{phase.get('status', '-')}: {phase.get('phaseId', '-')}" for phase in phase_results)

    if counts_by_family:
        lines.append("")
        lines.append("Suite Breakdown:")
        for family in sorted(counts_by_family):
            counts = dict(counts_by_family[family] or {})
            lines.append(
                f"{family}: total {counts.get('total', 0)} | ok {counts.get('ok', 0)} | "
                f"fail {counts.get('fail', 0)} | skip {counts.get('skip', 0)} | aborted {counts.get('aborted', 0)}"
            )
    elif suite_results:
        lines.append("")
        lines.append("Suites:")
        lines.extend(f"{suite.get('status', '-')}: {suite.get('suiteId', '-')}" for suite in suite_results)

    if subject_results:
        lines.append("")
        lines.append("Subject Breakdown:")
        for subject in subject_results:
            matrix_counts = dict(subject.get("matrixStatusCounts") or {})
            lines.append(
                f"{subject.get('subjectId', '-')}: goal={subject.get('requestedGoalId', '-')} | "
                f"status={subject.get('status', '-')} | matrix total {matrix_counts.get('total', 0)} | "
                f"ok {matrix_counts.get('ok', 0)} | fail {matrix_counts.get('fail', 0)} | "
                f"skip {matrix_counts.get('skip', 0)} | aborted {matrix_counts.get('aborted', 0)}"
            )
            if subject.get("subjectSummaryPath"):
                lines.append(f"summary: {subject['subjectSummaryPath']}")

    if failure_items:
        lines.append("")
        lines.append("Failure Digest:")
        for failure_item in failure_items:
            lines.append(
                f"{failure_item.get('type', '-')}: {failure_item.get('status', '-')} | {failure_item.get('id', '-')}"
            )
            if failure_item.get("rerunCommand"):
                lines.append(f"rerun: {failure_item['rerunCommand']}")
            if failure_item.get("reportPath"):
                lines.append(f"report: {failure_item['reportPath']}")
            if failure_item.get("subjectSummaryPath"):
                lines.append(f"summary: {failure_item['subjectSummaryPath']}")

    return "\n".join(lines) + "\n"


def _handle_test_summary(command_text: str, repo_root: Path, host_platform: str, options: dict) -> CommandResult:
    metadata = _resolve_run_metadata(repo_root, options.get("run"), prefer_current=False)
    if metadata is None:
        return CommandResult.failure(
            command=command_text,
            host_platform=host_platform,
            target=None,
            errors=["no recorded test runs"],
            text="no recorded test runs\n",
        )

    summary_path = repo_root / str(metadata["summaryPath"])
    if not summary_path.is_file():
        return CommandResult.failure(
            command=command_text,
            host_platform=host_platform,
            target=metadata.get("runId"),
            errors=["summary file is not available"],
            text="summary file is not available\n",
        )

    summary = json.loads(summary_path.read_text(encoding="utf-8"))
    return CommandResult.success(
        command=command_text,
        host_platform=host_platform,
        target=summary.get("runId"),
        payload={"summaryPath": str(summary_path.relative_to(repo_root).as_posix())},
        text=_render_summary(summary),
    )


def _handle_test_watch(command_text: str, repo_root: Path, host_platform: str, options: dict) -> CommandResult:
    metadata = _resolve_run_metadata(repo_root, options.get("run"), prefer_current=True)
    if metadata is None:
        return CommandResult.failure(
            command=command_text,
            host_platform=host_platform,
            target=None,
            errors=["no active or recorded test runs"],
            text="no active or recorded test runs\n",
        )

    events_path = repo_root / str(metadata["eventsPath"])
    if not events_path.is_file():
        return CommandResult.failure(
            command=command_text,
            host_platform=host_platform,
            target=metadata.get("runId"),
            errors=["events file is not available"],
            text="events file is not available\n",
        )

    events = [
        json.loads(line)
        for line in events_path.read_text(encoding="utf-8").splitlines()
        if line.strip()
    ]
    return CommandResult.success(
        command=command_text,
        host_platform=host_platform,
        target=metadata.get("runId"),
        payload={"eventsPath": str(events_path.relative_to(repo_root).as_posix())},
        text=tui_module.render_test_progress_screen(events, repo_root=repo_root),
    )


def _execute_legacy_command(
    legacy_command: dict,
    repo_root: Path,
    host_platform: str,
    command_text: str,
) -> CommandResult:
    handler = legacy_command["handler"]
    if handler == "build.dispatch":
        return build_commands.handle(legacy_command, repo_root, host_platform, command_text)
    manifest = manifest_module.load_run_manifest(
        repo_root,
        repo_root / "build" / "toolchains" / "run" / "run_manifest.json",
    )
    return handle(legacy_command, repo_root, host_platform, command_text, manifest)


def _execute_public_test_session(
    family: str,
    suite: str,
    stage: str,
    repo_root: Path,
    host_platform: str,
    command_text: str,
    manifest: dict,
) -> session_module.SessionResult:
    suite_spec = find_public_test_suite_spec(family, suite)
    legacy_command_id = resolve_legacy_test_command_id(family, suite, stage=stage, host_platform=host_platform)
    legacy_command = (
        manifest_module.find_command(manifest, legacy_command_id, host_platform, include_hidden=True)
        if legacy_command_id is not None
        else None
    )
    request = session_module.TestRequest(
        family=family,
        suite=suite,
        stage=stage,
        command_text=command_text,
    )
    return session_module.execute_suite_session(
        request,
        repo_root=repo_root,
        host_platform=host_platform,
        suite_spec=suite_spec,
        legacy_command=legacy_command,
        legacy_executor=_execute_legacy_command,
    )


def _empty_phase_status_counts() -> dict[str, int]:
    return {
        "total": 0,
        "ok": 0,
        "fail": 0,
        "skip": 0,
        "aborted": 0,
    }


def _normalize_unit_status(status: str) -> str:
    return status if status in {"ok", "fail", "skip", "aborted"} else "aborted"


def _build_planned_counts(items: list[dict[str, Any]]) -> dict[str, int]:
    subject_count = sum(1 for item in items if str(item.get("type") or "suite") == "subject")
    suite_count = len(items) - subject_count
    return {
        "totalUnits": len(items),
        "suiteCount": suite_count,
        "subjectCount": subject_count,
    }


def _build_counts_by_type(items: list[dict[str, Any]]) -> dict[str, dict[str, int]]:
    counts: dict[str, dict[str, int]] = {}
    for item in items:
        item_type = "subject" if str(item.get("type") or "suite") == "subject" else "suite"
        bucket = counts.setdefault(item_type, _empty_phase_status_counts())
        bucket["total"] += 1
    return counts


def _build_counts_by_family(items: list[dict[str, Any]]) -> dict[str, dict[str, int]]:
    counts: dict[str, dict[str, int]] = {}
    for item in items:
        if str(item.get("type") or "suite") == "subject":
            continue
        family = str(item.get("family") or "unknown")
        bucket = counts.setdefault(family, _empty_phase_status_counts())
        bucket["total"] += 1
    return counts


def _record_item_status(
    item: dict[str, Any],
    status: str,
    *,
    counts_by_type: dict[str, dict[str, int]],
    counts_by_family: dict[str, dict[str, int]],
) -> str:
    normalized_status = _normalize_unit_status(status)
    item_type = "subject" if str(item.get("type") or "suite") == "subject" else "suite"
    counts_by_type.setdefault(item_type, _empty_phase_status_counts())[normalized_status] += 1
    if item_type == "suite":
        family = str(item.get("family") or "unknown")
        counts_by_family.setdefault(family, _empty_phase_status_counts())[normalized_status] += 1
    return normalized_status


def _build_active_unit_context(item: dict[str, Any]) -> dict[str, Any]:
    item_type = "subject" if str(item.get("type") or "suite") == "subject" else "suite"
    context: dict[str, Any] = {
        "id": str(item.get("id") or ""),
        "type": item_type,
    }
    if item_type == "subject":
        context["subjectId"] = str(item.get("subjectId") or str(item.get("id") or "").removeprefix("subject/"))
        context["goalId"] = str(item.get("defaultGoalId") or item.get("requestedGoalId") or "")
        context["matrixId"] = str(item.get("defaultMatrixId") or item.get("matrixId") or "")
    else:
        context["family"] = str(item.get("family") or "")
        context["level"] = str(item.get("level") or "")
        context["primaryModuleId"] = str(item.get("primaryModuleId") or "")
    return context


def _build_rerun_command(item: dict[str, Any]) -> str:
    item_type = "subject" if str(item.get("type") or "suite") == "subject" else "suite"
    canonical_command = str(item.get("canonicalCommand") or "").strip()
    if canonical_command:
        return canonical_command if canonical_command.startswith("run ") else f"run {canonical_command}"
    if item_type == "subject":
        return f"run test subject --id {item['id']}"
    return f"run test {item.get('family')} {item.get('suite')}"


def _build_failure_item(
    item: dict[str, Any],
    *,
    status: str,
    run_context: dict[str, Any] | None = None,
    repo_root: Path | None = None,
    subject_result: dict[str, Any] | None = None,
) -> dict[str, Any]:
    item_type = "subject" if str(item.get("type") or "suite") == "subject" else "suite"
    failure_item = {
        "id": str(item.get("id") or ""),
        "type": item_type,
        "status": _normalize_unit_status(status),
        "rerunCommand": _build_rerun_command(item),
    }
    if item_type == "suite" and run_context is not None and repo_root is not None:
        session_root = Path(run_context["sessionRoot"])
        report_path = session_root / "suites" / str(item["id"]) / "report.json"
        failure_item["reportPath"] = report_path.relative_to(repo_root).as_posix()
    if item_type == "subject" and subject_result is not None and subject_result.get("subjectSummaryPath"):
        failure_item["subjectSummaryPath"] = str(subject_result["subjectSummaryPath"])
    return failure_item


def _build_aborted_subject_result(item: dict[str, Any]) -> dict[str, Any]:
    return {
        "subjectId": str(item.get("subjectId") or str(item.get("id") or "").removeprefix("subject/")),
        "requestedGoalId": str(item.get("defaultGoalId") or item.get("requestedGoalId") or ""),
        "status": "aborted",
        "matrixStatusCounts": {
            "total": 1,
            "ok": 0,
            "fail": 0,
            "skip": 0,
            "aborted": 1,
        },
    }


def _mark_remaining_items_aborted(
    remaining_items: list[dict[str, Any]],
    *,
    counts_by_type: dict[str, dict[str, int]],
    counts_by_family: dict[str, dict[str, int]],
    failure_items: list[dict[str, Any]],
    subject_results: list[dict[str, Any]],
) -> None:
    for remaining_item in remaining_items:
        _record_item_status(
            remaining_item,
            "aborted",
            counts_by_type=counts_by_type,
            counts_by_family=counts_by_family,
        )
        failure_items.append(_build_failure_item(remaining_item, status="aborted"))
        if str(remaining_item.get("type") or "suite") == "subject":
            subject_results.append(_build_aborted_subject_result(remaining_item))


def _merge_phase_suite_ids(target: list[str], additions: list[str]) -> None:
    for suite_id in additions:
        if suite_id not in target:
            target.append(suite_id)


def _aggregate_phase_member_status(statuses: list[str]) -> str:
    if not statuses:
        return "aborted"
    if any(status == "fail" for status in statuses):
        return "fail"
    if any(status == "aborted" for status in statuses):
        return "aborted"
    if all(status == "skip" for status in statuses):
        return "skip"
    if all(status in {"ok", "skip"} for status in statuses):
        return "ok"
    return "aborted"


def _build_pipeline_phase_plan(selected_object: dict[str, Any]) -> list[dict[str, Any]]:
    phase_plan: list[dict[str, Any]] = []
    for phase in list(selected_object.get("phases") or []):
        phase_plan.append(
            {
                "phaseId": str(phase.get("id") or ""),
                "title": str(phase.get("title") or ""),
                "members": [
                    {
                        "type": str(member.get("type") or ""),
                        "id": str(member.get("id") or ""),
                    }
                    for member in list(phase.get("members") or [])
                ],
            }
        )
    return phase_plan


def _build_pipeline_phase_results(
    index: registry_module.RegistryIndex,
    selected_object: dict[str, Any],
    suite_results: list[dict[str, Any]],
) -> list[dict[str, Any]]:
    suite_status_by_id = {
        str(suite_result.get("suiteId") or ""): str(suite_result.get("status") or "aborted")
        for suite_result in suite_results
    }
    phase_results: list[dict[str, Any]] = []

    for phase in list(selected_object.get("phases") or []):
        member_counts = _empty_phase_status_counts()
        member_results: list[dict[str, Any]] = []
        resolved_phase_suite_ids: list[str] = []

        for member in list(phase.get("members") or []):
            member_id = str(member.get("id") or "")
            member_object = registry_module.find_registry_object(index, member_id)
            if member_object is None:
                resolved_suite_ids: list[str] = []
                member_status = "aborted"
                member_results.append(
                    {
                        "objectId": member_id,
                        "objectType": str(member.get("type") or ""),
                        "displayName": member_id,
                        "resolvedSuiteIds": resolved_suite_ids,
                        "status": member_status,
                    }
                )
            else:
                resolved_suite_ids = [item["id"] for item in registry_module.expand_execution_plan(index, member_id)]
                resolved_statuses = [suite_status_by_id.get(suite_id, "aborted") for suite_id in resolved_suite_ids]
                member_status = _aggregate_phase_member_status(resolved_statuses)
                member_results.append(
                    {
                        "objectId": member_id,
                        "objectType": str(member_object.get("type") or member.get("type") or ""),
                        "displayName": str(member_object.get("displayName") or member_id),
                        "canonicalCommand": str(member_object.get("canonicalCommand") or ""),
                        "resolvedSuiteIds": resolved_suite_ids,
                        "status": member_status,
                    }
                )

            member_counts["total"] += 1
            member_counts[member_status if member_status in member_counts else "aborted"] += 1
            _merge_phase_suite_ids(resolved_phase_suite_ids, resolved_suite_ids)

        phase_status = _aggregate_phase_member_status([member["status"] for member in member_results])
        phase_results.append(
            {
                "phaseId": str(phase.get("id") or ""),
                "title": str(phase.get("title") or ""),
                "status": phase_status,
                "memberCounts": member_counts,
                "memberResults": member_results,
                "suiteIds": resolved_phase_suite_ids,
            }
        )

    return phase_results


def _with_selected_object_context(
    result: CommandResult,
    selected_object: dict[str, Any],
    index: registry_module.RegistryIndex,
) -> CommandResult:
    payload = dict(result.payload)
    payload["selectedObject"] = selected_object
    payload["objectOwnership"] = {
        "primaryModuleId": selected_object.get("primaryModuleId"),
        "moduleIds": list(selected_object.get("moduleIds", [])),
        "subsystemIds": list(selected_object.get("subsystemIds", [])),
    }
    if selected_object.get("type") == "pipeline":
        payload["phasePlan"] = _build_pipeline_phase_plan(selected_object)
        payload["phaseResults"] = _build_pipeline_phase_results(
            index,
            selected_object,
            list(payload.get("suiteResults") or []),
        )
    if result.status != "ok":
        payload.setdefault("failureCode", "test.execution.object_failed")
        payload.setdefault(
            "parserHint",
            "Inspect the generated summary/report paths and re-run the canonical command for the selected object.",
        )
        human_hint = f"Execution failed while running `{selected_object['id']}`."
        summary_path = payload.get("summaryPath")
        console_path = payload.get("consolePath")
        canonical_command = selected_object.get("canonicalCommand")
        if isinstance(summary_path, str) and summary_path:
            human_hint += f" Check `{summary_path}` for the summarized failure."
        elif isinstance(console_path, str) and console_path:
            human_hint += f" Check `{console_path}` for the console output."
        if isinstance(canonical_command, str) and canonical_command:
            human_hint += f" Re-run with `{canonical_command}`."
        payload.setdefault("humanHint", human_hint)
    return CommandResult(
        command=result.command,
        status=result.status,
        host_platform=result.host_platform,
        target=result.target,
        duration_ms=result.duration_ms,
        checks=result.checks,
        errors=result.errors,
        payload=payload,
        text=result.text,
    )


def _write_json_document(path: Path, payload: dict[str, Any]) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(payload, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")


def _resolve_subject_matrix_report_path(plan: dict[str, Any]) -> str:
    for stage in list(plan.get("stagePlan") or []):
        if str(stage.get("bucket") or "") == "report":
            return str(dict(stage.get("paths") or {}).get("manifestPath") or "")
    pipeline_report_root = str(dict(plan.get("artifactsRoot") or {}).get("pipelineReportRoot") or "")
    if pipeline_report_root:
        return f"{pipeline_report_root}/report.json"
    return ""


def _render_subject_summary(subject_summary: dict[str, Any]) -> str:
    lines = [
        "Subject Summary",
        f"Run: {subject_summary.get('runId', '-')}",
        f"Subject: {subject_summary.get('subjectId', '-')}",
        f"Goal: {subject_summary.get('requestedGoalId', '-')}",
        f"Status: {subject_summary.get('status', '-')}",
    ]
    matrix_results = list(subject_summary.get("matrixResults") or [])
    if matrix_results:
        lines.append("Matrices:")
        lines.extend(f"{matrix.get('status', '-')}: {matrix.get('matrixId', '-')}" for matrix in matrix_results)
    return "\n".join(lines) + "\n"


def _run_subject_object(
    *,
    index: registry_module.RegistryIndex,
    selected_object: dict[str, Any],
    normalized_options: dict[str, Any],
    repo_root: Path,
    host_platform: str,
    command_text: str,
) -> CommandResult:
    subject_key = str(normalized_options.get("subject") or selected_object.get("subjectId") or "")
    goal_id = str(normalized_options.get("goal") or "") or None
    matrix_id = str(normalized_options.get("matrix") or "") or None
    validation_profile_id = str(normalized_options.get("validation_profile") or "") or None
    validation_kind = str(normalized_options.get("validation_kind") or "") or None
    variant = str(normalized_options.get("variant") or "") or None
    run_id = reporting_module.build_run_id(host_platform)

    try:
        plan = subject_planner_module.build_plan(
            repo_root,
            subject_key,
            goal_id=goal_id,
            matrix_id=matrix_id,
            validation_profile_id=validation_profile_id,
            validation_kind=validation_kind,
            variant=variant,
            run_id=run_id,
        )
    except (FileNotFoundError, RuntimeError, ValueError) as error:
        return _with_selected_object_context(
            CommandResult.failure(
                command=command_text,
                host_platform=host_platform,
                target=str(selected_object["id"]),
                errors=[str(error)],
                payload={"exitCode": 2},
                text=f"{error}\n",
            ),
            selected_object,
            index,
        )

    artifacts_root = dict(plan.get("artifactsRoot") or {})
    run_context = reporting_module.start_session_report(
        repo_root=repo_root,
        host_platform=host_platform,
        command_text=command_text,
        run_id=run_id,
        session_root=repo_root / str(artifacts_root["runReportRoot"]),
        pointer_roots=[
            repo_root / str(artifacts_root["runsRoot"]),
            repo_root / "artifacts" / "logs" / "tests",
        ],
    )

    def event_writer(event: dict[str, Any]) -> None:
        reporting_module.append_session_event(repo_root, run_context, event)

    selection = dict(plan.get("selection") or {})
    try:
        execution_result = subject_executor_module.execute_plan(
            repo_root,
            plan,
            run_id=run_id,
            event_writer=event_writer,
        )
    except Exception as error:
        execution_result = {
            "subjectId": str(selection.get("subjectId") or subject_key),
            "matrixId": str(selection.get("matrixId") or matrix_id or ""),
            "goalId": str(selection.get("goalId") or goal_id or ""),
            "status": "fail",
            "terminalStageId": "",
            "terminalBucket": str(dict(selection.get("artifactPlan") or {}).get("evidenceTerminalBucket") or ""),
            "stageResults": [],
            "errors": [str(error)],
            "events": [],
        }

    generated_at = events_module.utc_timestamp()
    matrix_report_path = _resolve_subject_matrix_report_path(plan)
    matrix_report: dict[str, Any]
    matrix_report_abspath = repo_root / matrix_report_path if matrix_report_path else None
    if matrix_report_abspath is not None and matrix_report_abspath.is_file():
        matrix_report = json.loads(matrix_report_abspath.read_text(encoding="utf-8"))
    else:
        matrix_report = subject_reporting_module.build_matrix_report(
            plan,
            execution_result,
            run_id=str(run_context["runId"]),
            generated_at=generated_at,
        )
        if matrix_report_path:
            subject_reporting_module.materialize_matrix_report_artifacts(
                repo_root,
                matrix_report_path=matrix_report_path,
                matrix_report=matrix_report,
            )
            _write_json_document(repo_root / matrix_report_path, matrix_report)

    validation_outcome = subject_validations_module.run_subject_validations(
        repo_root,
        plan,
        run_id=str(run_context["runId"]),
    )
    validation_results = list(validation_outcome.get("validationResults") or [])
    validation_errors = list(validation_outcome.get("errors") or [])
    if validation_results:
        matrix_report["validationResults"] = validation_results
    if validation_errors:
        matrix_report.setdefault("errors", [])
        matrix_report["errors"] = list(matrix_report.get("errors") or []) + validation_errors
        matrix_report["status"] = "fail"
    if matrix_report_path and (validation_results or validation_errors):
        _write_json_document(repo_root / matrix_report_path, matrix_report)

    subject_summary_path = f"{plan['artifactsRoot']['subjectReportRoot']}/summary.json"
    subject_summary = subject_reporting_module.build_subject_summary(
        subject_id=str(selection.get("subjectId") or subject_key),
        requested_goal_id=str(selection.get("goalId") or goal_id or ""),
        matrix_reports=[matrix_report],
        matrix_report_paths={str(matrix_report.get("matrixId") or ""): matrix_report_path},
        run_id=str(run_context["runId"]),
        generated_at=generated_at,
    )
    _write_json_document(repo_root / subject_summary_path, subject_summary)
    subject_result = subject_reporting_module.build_subject_result(
        subject_summary,
        subject_summary_path=subject_summary_path,
    )

    artifacts = [
        path
        for path in [
            matrix_report_path,
            subject_summary_path,
            *list(matrix_report.get("reportArtifacts") or []),
            *list(matrix_report.get("releaseReportPaths") or []),
            *list(validation_outcome.get("artifacts") or []),
        ]
        if path
    ]
    errors = list(execution_result.get("errors") or []) + validation_errors
    succeeded = str(matrix_report.get("status") or execution_result.get("status") or "fail") == "ok"
    result_factory = CommandResult.success if succeeded else CommandResult.failure
    kwargs: dict[str, Any] = {
        "command": command_text,
        "host_platform": host_platform,
        "target": str(selected_object["id"]),
        "payload": {
            "artifacts": artifacts,
            "exitCode": 0 if succeeded else 1,
            "subjectResults": [subject_result],
            "validationResults": validation_results,
        },
        "text": _render_subject_summary(subject_summary),
    }
    if not succeeded:
        kwargs["errors"] = errors or [f"subject execution failed: {selected_object['id']}"]
    result = result_factory(**kwargs)
    result = _attach_session_report(result, repo_root, host_platform, command_text, run_context=run_context)
    return _with_selected_object_context(result, selected_object, index)


def _run_suite_items(
    items: list[dict[str, Any]],
    *,
    index: registry_module.RegistryIndex,
    target: str,
    stage: str,
    repo_root: Path,
    host_platform: str,
    command_text: str,
    manifest: dict,
    progress_callback: Callable[[dict[str, Any]], None] | None = None,
    selected_object: dict[str, Any] | None = None,
) -> CommandResult:
    if not items:
        return CommandResult.failure(
            command=command_text,
            host_platform=host_platform,
            target=target,
            errors=["no matching test suites"],
            text="no matching test suites\n",
        )

    run_context = reporting_module.start_session_report(
        repo_root=repo_root,
        host_platform=host_platform,
        command_text=command_text,
    )
    session_start_event = reporting_module.build_event(
        "session-start",
        {
            "runId": run_context["runId"],
            "command": command_text,
            "hostPlatform": host_platform,
        },
        run_id=run_context["runId"],
        status="running",
    )
    if progress_callback is not None:
        progress_callback(session_start_event)
        progress_callback(
            reporting_module.build_event(
                "progress",
                {
                    "completedUnits": 0,
                    "totalUnits": len(items),
                    "activeUnit": items[0]["id"],
                },
                run_id=run_context["runId"],
                status="running",
            )
        )

    outputs: list[str] = []
    suite_results: list[dict] = []
    subject_results: list[dict] = []
    artifacts: list[str] = []
    planned_counts = _build_planned_counts(items)
    counts_by_type = _build_counts_by_type(items)
    counts_by_family = _build_counts_by_family(items)
    failure_items: list[dict[str, Any]] = []
    for unit_index, item in enumerate(items):
        item_type = str(item.get("type") or "suite")
        active_unit_context = _build_active_unit_context(item)
        stage_start_event = reporting_module.build_event(
            "stage-start",
            {
                "completedUnits": unit_index,
                "totalUnits": len(items),
                "activeUnit": item["id"],
                "activeUnitContext": active_unit_context,
            },
            run_id=run_context["runId"],
            suite_id=item["id"],
            stage=stage,
            status="running",
        )
        reporting_module.append_session_event(repo_root, run_context, stage_start_event)
        if progress_callback is not None:
            progress_callback(stage_start_event)

        unit_status = "fail"
        unit_exit_code = 1
        unit_errors: list[str] = []
        if item_type == "subject":
            subject_object = registry_module.find_registry_object(index, str(item["id"]))
            if subject_object is None:
                unit_errors = [f"registry object not found: {item['id']}"]
            else:
                subject_result = _run_subject_object(
                    index=index,
                    selected_object=subject_object,
                    normalized_options={"id": str(subject_object["id"])},
                    repo_root=repo_root,
                    host_platform=host_platform,
                    command_text=f"test subject --id {subject_object['id']}",
                )
                outputs.append(subject_result.text or "")
                current_subject_results = list(subject_result.payload.get("subjectResults", []))
                subject_results.extend(current_subject_results)
                artifacts.extend(list(subject_result.payload.get("artifacts", [])))
                if current_subject_results:
                    unit_status = str(current_subject_results[0].get("status") or "aborted")
                else:
                    unit_status = "ok" if subject_result.status == "ok" else "fail"
                unit_exit_code = int(subject_result.payload.get("exitCode", 1))
                unit_errors = list(subject_result.errors)
                if _normalize_unit_status(unit_status) != "ok":
                    failure_items.append(
                        _build_failure_item(
                            item,
                            status=unit_status,
                            subject_result=current_subject_results[0] if current_subject_results else None,
                        )
                    )
        else:
            session_result = _execute_public_test_session(
                item["family"],
                item["suite"],
                stage,
                repo_root,
                host_platform,
                f"test {item['family']} {item['suite']}",
                manifest,
            )
            outputs.append(session_result.text or "")
            suite_results.extend(session_result.suite_results)
            artifacts.extend(session_result.artifacts)
            if session_result.suite_results:
                unit_status = str(session_result.suite_results[-1].get("status") or "aborted")
            else:
                unit_status = "ok" if session_result.status == "ok" else "fail"
            unit_exit_code = session_result.exit_code
            unit_errors = list(session_result.errors)
            if _normalize_unit_status(unit_status) != "ok":
                failure_items.append(
                    _build_failure_item(
                        item,
                        status=unit_status,
                        run_context=run_context,
                        repo_root=repo_root,
                    )
                )

        unit_status = _record_item_status(
            item,
            unit_status,
            counts_by_type=counts_by_type,
            counts_by_family=counts_by_family,
        )

        stage_finish_event = reporting_module.build_event(
            "stage-finish",
            {
                "completedUnits": unit_index + 1,
                "totalUnits": len(items),
                "activeUnit": item["id"],
            },
            run_id=run_context["runId"],
            suite_id=item["id"],
            stage=stage,
            status=unit_status,
        )
        progress_event = reporting_module.build_event(
            "progress",
            {
                "completedUnits": unit_index + 1,
                "totalUnits": len(items),
                "activeUnit": item["id"],
                "suiteStatus": unit_status,
            },
            run_id=run_context["runId"],
            suite_id=item["id"],
            stage=stage,
            status=unit_status,
        )
        reporting_module.append_session_event(repo_root, run_context, stage_finish_event)
        reporting_module.append_session_event(repo_root, run_context, progress_event)
        if progress_callback is not None:
            progress_callback(stage_finish_event)
            progress_callback(progress_event)

        if unit_status != "ok":
            _mark_remaining_items_aborted(
                items[unit_index + 1 :],
                counts_by_type=counts_by_type,
                counts_by_family=counts_by_family,
                failure_items=failure_items,
                subject_results=subject_results,
            )
            result = CommandResult.failure(
                command=command_text,
                host_platform=host_platform,
                target=target,
                errors=unit_errors or [f"test execution failed while running {item['id']}"],
                payload={
                    "items": items,
                    "plannedCounts": planned_counts,
                    "countsByType": counts_by_type,
                    "countsByFamily": counts_by_family,
                    "failureItems": failure_items,
                    "suiteResults": suite_results,
                    "subjectResults": subject_results,
                    "artifacts": artifacts,
                    "exitCode": unit_exit_code,
                },
                text="".join(outputs),
            )
            if selected_object is not None:
                result = _with_selected_object_context(result, selected_object, index)
            result = _attach_session_report(result, repo_root, host_platform, command_text, run_context=run_context)
            if selected_object is not None:
                result = _with_selected_object_context(result, selected_object, index)
            return result

    result = CommandResult.success(
        command=command_text,
        host_platform=host_platform,
        target=target,
        payload={
            "items": items,
            "plannedCounts": planned_counts,
            "countsByType": counts_by_type,
            "countsByFamily": counts_by_family,
            "failureItems": failure_items,
            "suiteResults": suite_results,
            "subjectResults": subject_results,
            "artifacts": artifacts,
            "exitCode": 0,
        },
        text="".join(outputs),
    )
    if selected_object is not None:
        result = _with_selected_object_context(result, selected_object, index)
    result = _attach_session_report(result, repo_root, host_platform, command_text, run_context=run_context)
    if selected_object is not None:
        result = _with_selected_object_context(result, selected_object, index)
    return result


def _handle_registry_object_dispatch(
    object_kind: str,
    repo_root: Path,
    host_platform: str,
    command_text: str,
    manifest: dict,
    options: dict,
    progress_callback: Callable[[dict[str, Any]], None] | None = None,
) -> CommandResult:
    try:
        normalized_options = selectors_module.normalize_selector_options(object_kind, options)
    except ValueError as error:
        return CommandResult.failure(
            command=command_text,
            host_platform=host_platform,
            target=None,
            errors=[str(error)],
            payload={"exitCode": 2},
            text=f"{error}\n",
        )

    index = _scan_registry(repo_root, host_platform)
    object_id = str(normalized_options["id"])
    selected_object = registry_module.find_registry_object(index, object_id)
    if selected_object is None:
        return CommandResult.failure(
            command=command_text,
            host_platform=host_platform,
            target=object_id,
            errors=[f"registry object not found: {object_id}"],
            payload={
                "failureCode": "test.registry.object_not_found",
                "parserHint": "Run `run test registry list --json` to inspect available registry object ids.",
                "humanHint": (
                    f"Registry object `{object_id}` was not found. "
                    "Run `run test registry list --json` or "
                    "`run test registry check-consistency --json` to inspect the current registry."
                ),
                "exitCode": 2,
            },
            text=f"registry object not found: {object_id}\n",
        )
    object_id = str(selected_object["id"])

    stage = str(normalized_options.get("stage") or "all")
    if object_kind == "subject":
        return _run_subject_object(
            index=index,
            selected_object=selected_object,
            normalized_options=normalized_options,
            repo_root=repo_root,
            host_platform=host_platform,
            command_text=command_text,
        )

    try:
        plan = registry_module.expand_execution_plan(index, object_id)
    except ValueError as error:
        return CommandResult.failure(
            command=command_text,
            host_platform=host_platform,
            target=object_id,
            errors=[str(error)],
            payload={"exitCode": 2},
            text=f"{error}\n",
        )

    return _run_suite_items(
        plan,
        index=index,
        target=object_id,
        stage=stage,
        repo_root=repo_root,
        host_platform=host_platform,
        command_text=command_text,
        manifest=manifest,
        progress_callback=progress_callback,
        selected_object=selected_object,
    )


def _handle_public_test_dispatch(
    command: dict,
    repo_root: Path,
    host_platform: str,
    command_text: str,
    manifest: dict,
    options: dict,
    progress_callback: Callable[[dict[str, Any]], None] | None = None,
) -> CommandResult:
    command_id = command["id"]
    if command_id == "test-list":
        return _handle_public_test_list(command_text, host_platform, options, manifest)
    if command_id == "test-watch":
        return _handle_test_watch(command_text, repo_root, host_platform, options)
    if command_id == "test-summary":
        return _handle_test_summary(command_text, repo_root, host_platform, options)
    if command_id in {"test-registry-refresh", "test-registry-list", "test-registry-check-consistency"}:
        return _handle_registry_dispatch(command, repo_root, host_platform, command_text)
    if command_id in {"test-suite", "test-subject", "test-module", "test-system", "test-pipeline"}:
        return _handle_registry_object_dispatch(
            command_id.removeprefix("test-"),
            repo_root,
            host_platform,
            command_text,
            manifest,
            options,
            progress_callback=progress_callback,
        )

    if command_id in {"test-all", "test-family-all"}:
        if options.get("strict"):
            try:
                catalog_module.scan_catalog(repo_root, host_platform=host_platform, strict=True)
            except ValueError:
                return CommandResult.failure(
                    command=command_text,
                    host_platform=host_platform,
                    target=options.get("family") or "all",
                    errors=["invalid suites found while scanning catalog"],
                    payload={"exitCode": 2},
                    text="invalid suites found while scanning catalog\n",
                )

    family = options.get("family")
    index = _scan_registry(repo_root, host_platform)
    items = list_public_test_suites(manifest, host_platform) if command_id in {"test-all", "test-family-all"} else []
    if command_id == "test-all":
        items = [*items, *[dict(subject) for subject in index.subjects]]
    if family and items:
        items = [item for item in items if item["family"] == family]
    if command_id == "test-family-suite":
        items = [
            {
                "id": f"{options.get('family')}/{options.get('suite')}",
                "family": options.get("family"),
                "suite": options.get("suite"),
                "stages": [options.get("stage", "all")],
            }
        ]
    target = family or "all" if command_id in {"test-all", "test-family-all"} else f"{options.get('family')}/{options.get('suite')}"

    return _run_suite_items(
        items,
        index=index,
        target=target,
        stage=options.get("stage", "all"),
        repo_root=repo_root,
        host_platform=host_platform,
        command_text=command_text,
        manifest=manifest,
        progress_callback=progress_callback,
    )


def _attach_session_report(
    result: CommandResult,
    repo_root: Path,
    host_platform: str,
    command_text: str,
    run_context: dict[str, Any] | None = None,
) -> CommandResult:
    try:
        report = reporting_module.write_session_report(
            repo_root=repo_root,
            host_platform=host_platform,
            command_text=command_text,
            status=result.status,
            suite_results=list(result.payload.get("suiteResults", [])),
            phase_results=list(result.payload.get("phaseResults", [])),
            text=result.text or "",
            errors=list(result.errors),
            artifacts=list(result.payload.get("artifacts", [])),
            subject_results=list(result.payload.get("subjectResults", [])),
            planned_counts=dict(result.payload.get("plannedCounts") or {}),
            counts_by_type=dict(result.payload.get("countsByType") or {}),
            counts_by_family=dict(result.payload.get("countsByFamily") or {}),
            failure_items=[dict(item) for item in list(result.payload.get("failureItems") or [])],
            run_context=run_context,
        )
    except OSError:
        return result

    payload = dict(result.payload)
    payload.update(report)
    return CommandResult(
        command=result.command,
        status=result.status,
        host_platform=result.host_platform,
        target=result.target,
        duration_ms=result.duration_ms,
        checks=result.checks,
        errors=result.errors,
        payload=payload,
        text=result.text,
    )


def _success(command_text: str, host_platform: str, target: str | None, output: str, artifacts: list[str]) -> CommandResult:
    return CommandResult.success(
        command=command_text,
        host_platform=host_platform,
        target=target,
        payload={"artifacts": artifacts},
        text=(output + "\n") if output else "",
    )


def _failure(command_text: str, host_platform: str, target: str | None, output: str, errors: list[str]) -> CommandResult:
    message = output if output else "\n".join(errors)
    if message and not message.endswith("\n"):
        message += "\n"
    return CommandResult.failure(
        command=command_text,
        host_platform=host_platform,
        target=target,
        errors=errors,
        text=message,
    )


def _ensure_smoke_build(command: dict, repo_root: Path, host_platform: str, command_text: str) -> tuple[bool, str, list[str]]:
    bootstrap = tooling_module.ensure_dotnet_available(command_text, host_platform)
    if not bootstrap.ready:
        return False, bootstrap.output, bootstrap.errors

    project_path = repo_root / command["project_path"]
    completed = run_process(["dotnet", "build", str(project_path), "-c", "Release"], cwd=repo_root)
    output = "\n".join(part for part in [bootstrap.output.strip(), combine_process_output(completed)] if part)
    if completed.returncode != 0:
        return False, output, ["smoke build prerequisite failed"]
    return True, output, []


def _run_smoke_project(command: dict, repo_root: Path, host_platform: str, command_text: str) -> CommandResult:
    build_ok, build_output, build_errors = _ensure_smoke_build(command, repo_root, host_platform, command_text)
    if not build_ok:
        return _failure(command_text, host_platform, command.get("target"), build_output, build_errors)

    dll_path = repo_root / command["dll_path"]
    completed = run_process(["dotnet", str(dll_path)], cwd=repo_root)
    runtime_output = combine_process_output(completed)
    output = "\n".join(part for part in [build_output, runtime_output] if part)
    if completed.returncode != 0:
        return _failure(command_text, host_platform, command.get("target"), output, ["managed smoke execution failed"])

    for expected in command["expected_patterns"]:
        if expected not in runtime_output:
            return _failure(command_text, host_platform, command.get("target"), output, [f"missing expected smoke output: {expected}"])

    return _success(command_text, host_platform, command.get("target"), output, [str(dll_path)])


def _run_trace_compare(command: dict, repo_root: Path, host_platform: str, command_text: str) -> CommandResult:
    bootstrap = tooling_module.ensure_dotnet_available(command_text, host_platform)
    if not bootstrap.ready:
        return _failure(command_text, host_platform, command.get("target"), bootstrap.output, bootstrap.errors)

    output = bootstrap.output.strip()
    run_id = reporting_module.build_run_id(host_platform)
    try:
        execution_result = subject_executor_module.execute_subject_matrix(
            repo_root,
            str(command["subject_id"]),
            goal_id=str(command.get("goal_id") or "") or None,
            matrix_id=str(command.get("matrix_id") or "") or None,
            run_id=run_id,
        )
    except Exception as error:
        error_text = str(error)
        return _failure(
            command_text,
            host_platform,
            command.get("target"),
            "\n".join(part for part in [output, error_text] if part),
            [error_text],
        )

    execution_errors = [str(item) for item in list(execution_result.get("errors") or []) if str(item)]
    if str(execution_result.get("status") or "fail") != "ok":
        return _failure(
            command_text,
            host_platform,
            command.get("target"),
            "\n".join(part for part in [output, *execution_errors] if part),
            execution_errors or ["trace compare failed"],
        )

    trace_paths = subject_executor_module.trace_paths_from_execution(repo_root, execution_result)
    if not trace_paths:
        return _failure(command_text, host_platform, command.get("target"), output, ["trace compare produced no trace artifacts"])

    return _success(
        command_text,
        host_platform,
        command.get("target"),
        output,
        [str(repo_root / trace_path) for trace_path in trace_paths],
    )


def _run_contract_check(command: dict, repo_root: Path, host_platform: str, command_text: str) -> CommandResult:
    target = str(command.get("contract_check") or command.get("target") or "")
    try:
        if target == "analysis-schema":
            contracts_module.assert_json_files_parse(contracts_module.analysis_contract_json_paths(repo_root))
            contracts_module.validate_analysis_contracts(repo_root)
        elif target == "trace-schema":
            contracts_module.assert_json_files_parse(contracts_module.trace_contract_json_paths(repo_root))
            contracts_module.validate_trace_schema_contracts(repo_root)
        else:
            return _failure(command_text, host_platform, command.get("target"), "", [f"unsupported contract check: {target}"])
    except RuntimeError as error:
        return _failure(command_text, host_platform, command.get("target"), "", [str(error)])

    return _success(command_text, host_platform, command.get("target"), "", [])


def _run_python_unittest(command: dict, repo_root: Path, host_platform: str, command_text: str) -> CommandResult:
    test_module = str(command.get("test_module") or "").strip()
    if not test_module:
        return _failure(command_text, host_platform, command.get("target"), "", ["python unittest target is required"])

    completed = run_process([sys.executable, "-m", "unittest", test_module], cwd=repo_root)
    output = combine_process_output(completed)
    if completed.returncode != 0:
        return _failure(command_text, host_platform, command.get("target"), output, [f"python unittest failed: {test_module}"])

    artifacts = [
        str((repo_root / artifact).resolve())
        for artifact in list(command.get("artifacts") or [])
    ]
    return _success(command_text, host_platform, command.get("target"), output, artifacts)


def handle(
    command: dict,
    repo_root: Path,
    host_platform: str,
    command_text: str,
    manifest: dict | None = None,
    options: dict | None = None,
    progress_callback: Callable[[dict[str, Any]], None] | None = None,
) -> CommandResult:
    if command["id"] in {
        "test-suite",
        "test-subject",
        "test-module",
        "test-system",
        "test-pipeline",
        "test-family-suite",
        "test-family-all",
        "test-all",
        "test-list",
        "test-watch",
        "test-summary",
        "test-registry-refresh",
        "test-registry-list",
        "test-registry-check-consistency",
    }:
        return _handle_public_test_dispatch(
            command,
            repo_root,
            host_platform,
            command_text,
            manifest or {},
            options or {},
            progress_callback=progress_callback,
        )

    kind = command["kind"]
    if kind == "smoke-run":
        return _run_smoke_project(command, repo_root, host_platform, command_text)
    if kind == "contract-check":
        return _run_contract_check(command, repo_root, host_platform, command_text)
    if kind == "python-unittest":
        return _run_python_unittest(command, repo_root, host_platform, command_text)
    if kind == "registry-object-alias":
        return _handle_registry_object_dispatch(
            str(command["registry_object_kind"]),
            repo_root,
            host_platform,
            command_text,
            manifest or {},
            {"id": str(command["registry_object_id"])},
            progress_callback=progress_callback,
        )
    if kind == "trace-compare":
        return _run_trace_compare(command, repo_root, host_platform, command_text)

    return _failure(command_text, host_platform, command.get("target"), "", [f"unsupported test kind: {kind}"])
