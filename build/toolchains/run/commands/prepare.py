from __future__ import annotations

import itertools
import sys
from pathlib import Path
from typing import Any, Callable

try:
    from ..common import write_json
    from ..result import CommandResult
    from .. import manifest as manifest_module
    from ..testing.events import build_event
    from . import doctor as doctor_commands
    from . import build as build_commands
    from . import test as test_commands
    from . import verify as verify_commands
except ImportError:
    root = Path(__file__).resolve().parents[1]
    sys.path.insert(0, str(root))
    from common import write_json
    from result import CommandResult
    import manifest as manifest_module
    from testing.events import build_event
    from commands import doctor as doctor_commands
    from commands import build as build_commands
    from commands import test as test_commands
    from commands import verify as verify_commands


SMOKE_PREPARE_STEPS = [
    ["test", "smoke", "all", "--stage", "build"],
]

VERIFY_WINDOWS_PREPARE_STEPS = [
    ["build", "native-contract", "abi"],
    ["build", "native-contract", "bridge"],
    *SMOKE_PREPARE_STEPS,
    ["build", "preset", "windows-x64-reference"],
    ["build", "platform", "android-arm64-smoke"],
    ["build", "platform", "linux-x64-packaging"],
]

VERIFY_MACOS_PREPARE_STEPS = [
    ["build", "native-contract", "abi"],
    ["build", "native-contract", "bridge"],
    *SMOKE_PREPARE_STEPS,
    ["build", "preset", "macos-reference"],
    ["build", "platform", "ios-arm64-packaging"],
    ["build", "platform", "linux-x64-packaging"],
]


def resolve_prepare_scope(command_id: str) -> str:
    mapping = {
        "prepare": "global",
        "prepare-smoke": "smoke",
        "prepare-verify-roadmap-0-windows": "verify-roadmap0-windows",
        "prepare-verify-roadmap-0-macos": "verify-roadmap0-macos",
    }
    return mapping[command_id]


def _unique_steps(steps: list[list[str]]) -> list[list[str]]:
    seen: set[tuple[str, ...]] = set()
    unique: list[list[str]] = []
    for step in steps:
        key = tuple(step)
        if key in seen:
            continue
        seen.add(key)
        unique.append(list(step))
    return unique


def _prepare_plan(scope: str, host_platform: str) -> list[list[str]]:
    if scope == "smoke":
        return [list(step) for step in SMOKE_PREPARE_STEPS]
    if scope == "verify-roadmap0-windows":
        return [list(step) for step in VERIFY_WINDOWS_PREPARE_STEPS]
    if scope == "verify-roadmap0-macos":
        return [list(step) for step in VERIFY_MACOS_PREPARE_STEPS]
    if scope == "global":
        host_specific = (
            VERIFY_WINDOWS_PREPARE_STEPS
            if host_platform == "windows"
            else VERIFY_MACOS_PREPARE_STEPS
            if host_platform == "macos"
            else SMOKE_PREPARE_STEPS
        )
        combined = [list(step) for step in itertools.chain(SMOKE_PREPARE_STEPS, host_specific)]
        return _unique_steps(combined)
    raise KeyError(f"unknown prepare scope: {scope}")


def _execute_prepare_step(
    step_argv: list[str],
    repo_root: Path,
    host_platform: str,
    manifest: dict,
) -> tuple[str, CommandResult]:
    step_text = " ".join(step_argv)
    parsed = manifest_module.parse_cli(step_argv, False, manifest, host_platform)
    command = parsed["command"]
    if command is None:
        return step_text, CommandResult.failure(
            command=step_text,
            host_platform=host_platform,
            target=None,
            errors=[f"unknown prepare step: {step_text}"],
            text=f"unknown prepare step: {step_text}\n",
        )

    handler = command["handler"]
    if handler == "build.dispatch":
        return step_text, build_commands.handle(command, repo_root, host_platform, step_text)
    if handler == "test.dispatch":
        return step_text, test_commands.handle(command, repo_root, host_platform, step_text, manifest, parsed["options"])
    if handler == "verify.dispatch":
        return step_text, verify_commands.handle(command, repo_root, host_platform, step_text)

    return step_text, CommandResult.failure(
        command=step_text,
        host_platform=host_platform,
        target=None,
        errors=[f"unsupported prepare step handler: {handler}"],
        text=f"unsupported prepare step handler: {handler}\n",
    )


def prepare_state_path(repo_root: Path, scope: str) -> Path:
    return repo_root / "artifacts" / "run" / "prepare" / f"{scope}.json"


def resolve_clean_paths(repo_root: Path, scope: str) -> list[Path]:
    smoke_root = repo_root / "artifacts" / "smoke"
    run_root = repo_root / "artifacts" / "run"
    verify_root = repo_root / "artifacts" / "verify-roadmap-0"
    preset_root = repo_root / "artifacts" / "presets"

    if scope == "smoke":
        return [smoke_root, prepare_state_path(repo_root, "smoke")]
    if scope == "verify-roadmap0-windows":
        return [
            verify_root / "windows",
            preset_root / "windows-x64-reference",
            run_root / "native-contract-abi",
            run_root / "native-contract-bridge",
            run_root / "platform" / "android-arm64-smoke",
            run_root / "platform" / "linux-x64-packaging",
            prepare_state_path(repo_root, scope),
        ]
    if scope == "verify-roadmap0-macos":
        return [
            verify_root / "macos",
            preset_root / "macos-reference",
            run_root / "native-contract-abi",
            run_root / "native-contract-bridge",
            run_root / "platform" / "ios-arm64-packaging",
            run_root / "platform" / "linux-x64-packaging",
            prepare_state_path(repo_root, scope),
        ]
    if scope == "global":
        return [
            smoke_root,
            run_root,
            verify_root,
            preset_root / "windows-x64-reference",
            preset_root / "macos-reference",
            prepare_state_path(repo_root, "global"),
        ]
    raise KeyError(f"unknown clean scope: {scope}")


def _emit_event(
    progress_callback: Callable[[dict[str, Any]], None] | None,
    *,
    event_type: str,
    completed: int,
    total: int,
    active_unit: str,
    step_status: str | None = None,
    path: str | None = None,
) -> None:
    if progress_callback is None:
        return

    payload: dict[str, Any] = {
        "completedUnits": completed,
        "totalUnits": total,
        "activeUnit": active_unit,
    }
    if step_status is not None:
        payload["suiteStatus"] = step_status
    if path is not None:
        payload["path"] = path

    progress_callback(
        build_event(
            event_type,
            payload,
            status=step_status or "running",
        )
    )


def _summary_text(command_text: str, scope: str, executed: list[str], *, cached: bool = False) -> str:
    lines = [f"Run completed: {command_text}", f"Prepare scope: {scope}"]
    if cached:
        lines.append("Cache: reused existing prepared state")
    elif executed:
        lines.append(f"Executed steps ({len(executed)}):")
        lines.extend(f"{index}. {step}" for index, step in enumerate(executed, start=1))
    return "\n".join(lines) + "\n"


def handle(
    command: dict,
    repo_root: Path,
    host_platform: str,
    command_text: str,
    manifest: dict,
    progress_callback: Callable[[dict[str, Any]], None] | None = None,
) -> CommandResult:
    scope = resolve_prepare_scope(command["id"])
    console_outputs: list[str] = []
    artifacts: list[str] = []
    important_outputs: list[dict[str, str]] = []
    plan_steps = _prepare_plan(scope, host_platform)
    total_units = len(plan_steps) + (1 if command["id"] == "prepare" else 0)
    completed_units = 0

    if command["id"] == "prepare":
        _emit_event(progress_callback, event_type="stage-start", completed=completed_units, total=total_units, active_unit="doctor")
        doctor_result = doctor_commands.handle(repo_root, host_platform, "doctor")
        if doctor_result.text:
            console_outputs.append(doctor_result.payload.get("consoleText", doctor_result.text))
        if doctor_result.status != "ok":
            _emit_event(progress_callback, event_type="progress", completed=completed_units, total=total_units, active_unit="doctor", step_status="fail")
            return CommandResult.failure(
                command=command_text,
                host_platform=host_platform,
                target=scope,
                errors=doctor_result.errors or ["prepare failed during preflight doctor check"],
                payload={
                    "prepareScope": scope,
                    "preparedCommands": [],
                    "artifacts": [],
                    "importantOutputs": [],
                    "consoleText": "\n".join(part for part in console_outputs if part),
                },
                text=f"Run failed: {command_text}\n- prepare failed during preflight doctor check\n",
            )
        completed_units += 1
        _emit_event(progress_callback, event_type="progress", completed=completed_units, total=total_units, active_unit="doctor", step_status="ok")

    state_path = prepare_state_path(repo_root, scope)
    if state_path.is_file():
        artifacts.append(str(state_path))
        important_outputs.append({"label": "Prepare state", "path": str(state_path)})
        _emit_event(progress_callback, event_type="artifact", completed=completed_units, total=total_units, active_unit="prepare-state", path=str(state_path))
        return CommandResult.success(
            command=command_text,
            host_platform=host_platform,
            target=scope,
            payload={
                "prepareScope": scope,
                "preparedCommands": [],
                "cached": True,
                "artifacts": artifacts,
                "importantOutputs": important_outputs,
                "consoleText": "\n".join(part for part in console_outputs if part),
            },
            text=_summary_text(command_text, scope, [], cached=True),
        )

    executed: list[str] = []

    for step_argv in plan_steps:
        step_text, result = _execute_prepare_step(step_argv, repo_root, host_platform, manifest)
        _emit_event(progress_callback, event_type="stage-start", completed=completed_units, total=total_units, active_unit=step_text)
        console_outputs.append(str(result.payload.get("consoleText", result.text or "")))
        if result.status != "ok":
            _emit_event(progress_callback, event_type="progress", completed=completed_units, total=total_units, active_unit=step_text, step_status="fail")
            return CommandResult.failure(
                command=command_text,
                host_platform=host_platform,
                target=scope,
                errors=[f"prepare failed while executing {step_text}"],
                payload={
                    "prepareScope": scope,
                    "preparedCommands": executed,
                    "artifacts": artifacts,
                    "importantOutputs": important_outputs,
                    "consoleText": "\n".join(part for part in console_outputs if part),
                },
                text=f"Run failed: {command_text}\n- prepare failed while executing {step_text}\n",
            )
        executed.append(step_text)
        completed_units += 1
        _emit_event(progress_callback, event_type="progress", completed=completed_units, total=total_units, active_unit=step_text, step_status="ok")
        for artifact in list(result.payload.get("artifacts", [])):
            artifacts.append(str(artifact))
            _emit_event(progress_callback, event_type="artifact", completed=completed_units, total=total_units, active_unit=step_text, path=str(artifact))

    write_json(
        state_path,
        {
            "scope": scope,
            "hostPlatform": host_platform,
            "preparedCommands": executed,
        },
    )
    artifacts.append(str(state_path))
    important_outputs.append({"label": "Prepare state", "path": str(state_path)})
    _emit_event(progress_callback, event_type="artifact", completed=completed_units, total=total_units, active_unit="prepare-state", path=str(state_path))

    return CommandResult.success(
        command=command_text,
        host_platform=host_platform,
        target=scope,
        payload={
            "prepareScope": scope,
            "preparedCommands": executed,
            "cached": False,
            "artifacts": artifacts,
            "importantOutputs": important_outputs,
            "consoleText": "\n".join(part for part in console_outputs if part),
        },
        text=_summary_text(command_text, scope, executed),
    )
