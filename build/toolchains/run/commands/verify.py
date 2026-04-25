from __future__ import annotations

import sys
from pathlib import Path
from typing import Any, Callable

try:
    from ..core import tooling as tooling_module
    from ..core.common import combine_process_output, run_process
    from ..core.result import CommandResult
    from ..testing import inventory_generator as inventory_generator_module
    from ..testing.events import build_event
except ImportError:
    root = Path(__file__).resolve().parents[1]
    sys.path.insert(0, str(root))
    from core import tooling as tooling_module
    from core.common import combine_process_output, run_process
    from core.result import CommandResult
    from testing import inventory_generator as inventory_generator_module
    from testing.events import build_event


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


def _handle_verification_v1(
    repo_root: Path,
    host_platform: str,
    command_text: str,
    options: dict[str, Any],
    progress_callback: Callable[[dict[str, Any]], None] | None = None,
) -> CommandResult:
    output_root = inventory_generator_module.resolve_inventory_output_root(repo_root, options.get("output"))
    active_unit = "verification-v1 formal refresh"
    _emit_event(progress_callback, event_type="stage-start", completed=0, total=1, active_unit=active_unit)
    try:
        payload = inventory_generator_module.refresh_inventory_outputs(
            repo_root,
            host_platform=host_platform,
            output_root=output_root,
        )
    except Exception as error:
        _emit_event(progress_callback, event_type="progress", completed=0, total=1, active_unit=active_unit, step_status="fail")
        message = str(error)
        if message and not message.endswith("\n"):
            message += "\n"
        return CommandResult.failure(
            command=command_text,
            host_platform=host_platform,
            target="verification-v1",
            errors=[str(error)],
            payload={
                "artifacts": [],
                "importantOutputs": [],
                "consoleText": str(error),
                "exitCode": 1,
            },
            text=message,
        )

    verification_data = dict(payload.get("verificationData") or {})
    important_outputs = [
        str(verification_data.get("latestResultPath") or ""),
        str(verification_data.get("masterResultPath") or ""),
        str(verification_data.get("reportSummaryPath") or ""),
    ]
    important_outputs = [item for item in important_outputs if item]
    foundation_dll_audit = dict(payload.get("foundationDllAudit") or {})
    if str(foundation_dll_audit.get("dashboardPath") or ""):
        important_outputs.append(str(foundation_dll_audit.get("dashboardPath")))
    if str(foundation_dll_audit.get("reportSummaryPath") or ""):
        important_outputs.append(str(foundation_dll_audit.get("reportSummaryPath")))
    codegen_stub_paths = [str(item) for item in list(verification_data.get("codegenStubPaths") or []) if str(item)]
    important_outputs.extend(codegen_stub_paths)
    text = "\n".join(
        [
            f"outputRoot: {payload.get('outputRoot')}",
            f"latestResult: {verification_data.get('latestResultPath')}",
            f"masterResult: {verification_data.get('masterResultPath')}",
            f"reportSummary: {verification_data.get('reportSummaryPath')}",
            f"artifacts: {len(list(payload.get('artifacts') or []))}",
        ]
    )
    _emit_event(progress_callback, event_type="progress", completed=1, total=1, active_unit=active_unit, step_status="ok")
    for artifact_path in list(payload.get("artifacts") or []):
        _emit_event(progress_callback, event_type="artifact", completed=1, total=1, active_unit=active_unit, path=str(artifact_path))
    return CommandResult.success(
        command=command_text,
        host_platform=host_platform,
        target="verification-v1",
        payload={
            **payload,
            "artifacts": list(payload.get("artifacts") or []),
            "importantOutputs": important_outputs,
            "consoleText": text,
        },
        text=text + "\n",
    )


def handle(
    command: dict,
    repo_root: Path,
    host_platform: str,
    command_text: str,
    options: dict[str, Any] | None = None,
    progress_callback: Callable[[dict[str, Any]], None] | None = None,
) -> CommandResult:
    if command["id"] == "verify-verification-v1":
        return _handle_verification_v1(
            repo_root,
            host_platform,
            command_text,
            options or {},
            progress_callback=progress_callback,
        )

    bootstrap = tooling_module.ensure_dotnet_available(command_text, host_platform)
    if not bootstrap.ready:
        message = bootstrap.output if bootstrap.output.endswith("\n") else bootstrap.output + "\n"
        return CommandResult.failure(
            command=command_text,
            host_platform=host_platform,
            target=command.get("target"),
            errors=bootstrap.errors,
            payload={"artifacts": [], "importantOutputs": [], "consoleText": bootstrap.output},
            text=message,
        )

    cmake_path, cmake_env = tooling_module.cmake_environment(repo_root)
    if cmake_path is None:
        return CommandResult.failure(
            command=command_text,
            host_platform=host_platform,
            target=command.get("target"),
            errors=["cmake not found"],
            payload={"artifacts": [], "importantOutputs": [], "consoleText": "cmake not found\n"},
            text="cmake not found\n",
        )

    host_profile = command["host_profile"]
    if host_profile == "windows":
        arguments = [
            "powershell",
            "-NoLogo",
            "-NoProfile",
            "-NonInteractive",
            "-ExecutionPolicy",
            "Bypass",
            "-File",
            str(repo_root / "build" / "scripts" / "verify-runtime-baseline.ps1"),
            "-HostProfile",
            "windows",
        ]
    else:
        arguments = [
            sys.executable,
            str(repo_root / "build" / "scripts" / "verify-runtime-baseline.py"),
            "--host-profile",
            "macos",
        ]

    active_unit = f"runtime-baseline low-level script ({host_profile})"
    _emit_event(progress_callback, event_type="stage-start", completed=0, total=1, active_unit=active_unit)
    completed = run_process(arguments, cwd=repo_root, env=cmake_env)
    output = "\n".join(part for part in [bootstrap.output.strip(), combine_process_output(completed)] if part)
    if completed.returncode != 0:
        _emit_event(progress_callback, event_type="progress", completed=0, total=1, active_unit=active_unit, step_status="fail")
        message = output if output else "runtime-baseline low-level script execution failed"
        if message and not message.endswith("\n"):
            message += "\n"
        failure_text = "Run failed: " + command_text + "\n"
        if output:
            failure_text += message
        failure_text += "- runtime-baseline low-level script execution failed\n"
        return CommandResult.failure(
            command=command_text,
            host_platform=host_platform,
            target=host_profile,
            errors=["runtime-baseline low-level script execution failed"],
            payload={"artifacts": [], "importantOutputs": [], "consoleText": output},
            text=failure_text,
        )

    artifact_path = str(repo_root / "artifacts" / "verify-runtime-baseline" / host_profile)
    _emit_event(progress_callback, event_type="progress", completed=1, total=1, active_unit=active_unit, step_status="ok")
    _emit_event(progress_callback, event_type="artifact", completed=1, total=1, active_unit=active_unit, path=artifact_path)
    return CommandResult.success(
        command=command_text,
        host_platform=host_platform,
        target=host_profile,
        payload={
            "artifacts": [artifact_path],
            "importantOutputs": [],
            "consoleText": output,
        },
        text=f"Run completed: {command_text}\n",
    )
