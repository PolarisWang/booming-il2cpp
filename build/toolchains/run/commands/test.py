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
    from ..testing import public_specs as public_specs_module
    from ..testing import registry as registry_module
    from ..testing import selectors as selectors_module
    from ..testing import session as session_module
    from ..testing import reporting as reporting_module
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
    from testing import public_specs as public_specs_module
    from testing import registry as registry_module
    from testing import selectors as selectors_module
    from testing import session as session_module
    from testing import reporting as reporting_module
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


def _read_json_if_exists(path: Path) -> dict[str, Any] | None:
    if not path.is_file():
        return None
    return json.loads(path.read_text(encoding="utf-8"))


def _resolve_run_metadata(repo_root: Path, requested_run: str | None, *, prefer_current: bool) -> dict[str, Any] | None:
    logs_root = _logs_root(repo_root)
    if requested_run and requested_run not in {"last", "current"}:
        run_root = logs_root / requested_run
        return {
            "runId": requested_run,
            "summaryPath": str((run_root / "summary.json").relative_to(repo_root).as_posix()),
            "eventsPath": str((run_root / "events.jsonl").relative_to(repo_root).as_posix()),
        }

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
    lines = [
        "Unified Test Summary",
        f"Run: {summary.get('runId', '-')}",
        f"Command: {summary.get('command', '-')}",
        f"Status: {summary.get('finalStatus', '-')}",
    ]
    errors = list(summary.get("errors") or [])
    if errors:
        lines.append("Errors:")
        lines.extend(str(error) for error in errors)

    suite_results = list(summary.get("suiteResults") or [])
    phase_results = list(summary.get("phaseResults") or [])
    if phase_results:
        lines.append("Phases:")
        lines.extend(f"{phase.get('status', '-')}: {phase.get('phaseId', '-')}" for phase in phase_results)
    if suite_results:
        lines.append("Suites:")
        lines.extend(f"{suite.get('status', '-')}: {suite.get('suiteId', '-')}" for suite in suite_results)

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
    artifacts: list[str] = []
    for unit_index, item in enumerate(items):
        stage_start_event = reporting_module.build_event(
            "stage-start",
            {
                "completedUnits": unit_index,
                "totalUnits": len(items),
                "activeUnit": item["id"],
            },
            run_id=run_context["runId"],
            suite_id=item["id"],
            stage=stage,
            status="running",
        )
        reporting_module.append_session_event(repo_root, run_context, stage_start_event)
        if progress_callback is not None:
            progress_callback(stage_start_event)

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
            status="ok" if session_result.status == "ok" else "fail",
        )
        progress_event = reporting_module.build_event(
            "progress",
            {
                "completedUnits": unit_index + 1,
                "totalUnits": len(items),
                "activeUnit": item["id"],
                "suiteStatus": "ok" if session_result.status == "ok" else "fail",
            },
            run_id=run_context["runId"],
            suite_id=item["id"],
            stage=stage,
            status="ok" if session_result.status == "ok" else "fail",
        )
        reporting_module.append_session_event(repo_root, run_context, stage_finish_event)
        reporting_module.append_session_event(repo_root, run_context, progress_event)
        if progress_callback is not None:
            progress_callback(stage_finish_event)
            progress_callback(progress_event)

        if session_result.status != "ok":
            result = CommandResult.failure(
                command=command_text,
                host_platform=host_platform,
                target=target,
                errors=session_result.errors or [f"test execution failed while running {item['id']}"],
                payload={
                    "items": items,
                    "suiteResults": suite_results,
                    "artifacts": artifacts,
                    "exitCode": session_result.exit_code,
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
        payload={"items": items, "suiteResults": suite_results, "artifacts": artifacts, "exitCode": 0},
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

    stage = str(normalized_options.get("stage") or "all")
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
    if command_id in {"test-suite", "test-module", "test-system", "test-pipeline"}:
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
    items = list_public_test_suites(manifest, host_platform) if command_id in {"test-all", "test-family-all"} else []
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
        index=_scan_registry(repo_root, host_platform),
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

    host_embedding_project = repo_root / "tests" / "smoke" / "input" / "HostEmbeddingLite" / "HostEmbeddingLite.csproj"
    build = run_process(["dotnet", "build", str(host_embedding_project), "-c", "Release"], cwd=repo_root)
    build_output = "\n".join(part for part in [bootstrap.output.strip(), combine_process_output(build)] if part)
    if build.returncode != 0:
        return _failure(command_text, host_platform, command.get("target"), build_output, ["HostEmbeddingLite build failed"])

    host_embedding_dll = repo_root / "artifacts" / "smoke" / "bin" / "HostEmbeddingLite" / "Release" / "net8.0" / "HostEmbeddingLite.dll"
    trace_output = repo_root / command["actual_trace_path"]
    trace_output.parent.mkdir(parents=True, exist_ok=True)

    export = run_process(
        [
            "dotnet",
            str(host_embedding_dll),
            "--trace-platform",
            command["trace_platform"],
            "--trace-output",
            str(trace_output),
        ],
        cwd=repo_root,
    )
    export_output = combine_process_output(export)
    if export.returncode != 0:
        return _failure(command_text, host_platform, command.get("target"), "\n".join(part for part in [build_output, export_output] if part), ["trace export failed"])

    compare_args = [
        sys.executable,
        str(repo_root / "tests" / "contracts" / "trace" / "compare-warmup-trace.py"),
        str(repo_root / command["expected_trace_path"]),
        str(trace_output),
    ]

    compare = run_process(compare_args, cwd=repo_root)
    compare_output = combine_process_output(compare)
    output = "\n".join(part for part in [build_output, export_output, compare_output] if part)
    if compare.returncode != 0:
        return _failure(command_text, host_platform, command.get("target"), output, ["trace compare failed"])

    return _success(command_text, host_platform, command.get("target"), output, [str(trace_output)])


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
