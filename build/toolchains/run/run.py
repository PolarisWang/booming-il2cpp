from __future__ import annotations

import sys
import time
from pathlib import Path
from typing import Any, Callable

if __package__ in (None, ""):
    sys.path.insert(0, str(Path(__file__).resolve().parent))
    from commands import clean as clean_commands
    from commands import deploy as deploy_commands
    from commands import doctor as doctor_commands
    from commands import build as build_commands
    from commands import inspect as inspect_commands
    from commands import prepare as prepare_commands
    from commands import project as project_commands
    from commands import test as test_commands
    from commands import benchmark as benchmark_commands
    from core import manifest as manifest_module
    from core import operation_reporting as operation_reporting_module
    import runtime as runtime_module
    import tui as tui_module
    from testing.events import build_event
    from core.result import CommandResult
else:
    from .commands import clean as clean_commands
    from .commands import deploy as deploy_commands
    from .commands import doctor as doctor_commands
    from .commands import build as build_commands
    from .commands import inspect as inspect_commands
    from .commands import prepare as prepare_commands
    from .commands import project as project_commands
    from .commands import test as test_commands
    from .commands import benchmark as benchmark_commands
    from .core import manifest as manifest_module
    from .core import operation_reporting as operation_reporting_module
    from . import runtime as runtime_module
    from . import tui as tui_module
    from .testing.events import build_event
    from .core.result import CommandResult


OPERATION_HANDLERS = {"build.dispatch", "prepare.dispatch", "project.dispatch", "deploy.dispatch"}


def resolve_repo_root() -> Path:
    return Path(__file__).resolve().parents[3]


def configure_stdout() -> None:
    reconfigure = getattr(sys.stdout, "reconfigure", None)
    if callable(reconfigure):
        reconfigure(errors="replace")


def render_result(result: CommandResult, json_output: bool, repo_root: Path) -> str:
    if json_output:
        events_path = result.payload.get("eventsPath")
        if isinstance(events_path, str):
            resolved_events_path = repo_root / events_path
            if resolved_events_path.is_file():
                return resolved_events_path.read_text(encoding="utf-8")
        return result.to_json() + "\n"

    text = result.text or ""
    if result.payload.get("reportKind") == "operation":
        footer = tui_module.render_operation_report_highlights(result.payload, repo_root=repo_root)
        if footer:
            if text and not text.endswith("\n"):
                text += "\n"
            if text:
                text += "\n"
            text += footer
    elif result.payload.get("sessionPath") and result.payload.get("summaryPath"):
        footer = tui_module.render_test_report_highlights(result.payload, repo_root=repo_root)
        if footer:
            if text and not text.endswith("\n"):
                text += "\n"
            if text:
                text += "\n"
            text += footer
    return text


def render_post_tui_prefix(used_fullscreen_tui: bool, result: CommandResult, json_output: bool) -> str:
    if not used_fullscreen_tui or json_output:
        return ""
    if not (result.text or ""):
        return ""
    return "\r\x1b[2K"


def build_test_progress_callback(repo_root: Path) -> Callable[[dict], None]:
    events: list[dict] = []
    rendered_history_count = 0
    wrote_header = False

    def _callback(event: dict) -> None:
        nonlocal rendered_history_count
        nonlocal wrote_header
        events.append(event)
        view = tui_module.build_test_progress_view(events, repo_root=repo_root)
        lines: list[str] = []
        if not wrote_header:
            lines.extend(["Unified Test Progress", f"Command: {view.command or '-'}", ""])
            wrote_header = True
        new_history = view.history_lines[rendered_history_count:]
        if new_history:
            lines.extend(new_history)
            rendered_history_count += len(new_history)
        if lines:
            sys.stdout.write("\n".join(lines) + "\n")
            sys.stdout.flush()

    return _callback


def build_operation_progress_callback(repo_root: Path, run_context: dict[str, Any]) -> Callable[[dict], None]:
    events: list[dict] = []
    rendered_history_count = 0
    wrote_header = False

    def _callback(event: dict) -> None:
        nonlocal rendered_history_count
        nonlocal wrote_header
        operation_reporting_module.append_operation_event(run_context, event)
        events.append(event)
        view = tui_module.build_operation_progress_view(events, repo_root=repo_root)
        lines: list[str] = []
        if not wrote_header:
            lines.extend(["Unified Run Progress", f"Command: {view.command or '-'}", ""])
            wrote_header = True
        new_history = view.history_lines[rendered_history_count:]
        if new_history:
            lines.extend(new_history)
            rendered_history_count += len(new_history)
        if lines:
            sys.stdout.write("\n".join(lines) + "\n")
            sys.stdout.flush()

    return _callback


def add_legacy_test_migration_guidance(command: dict, result: CommandResult) -> CommandResult:
    if command.get("public", True):
        return result

    replacement_syntax = command.get("replacement_syntax")
    if not replacement_syntax:
        return result

    payload = dict(result.payload)
    payload["migration"] = {
        "deprecatedCommandId": command["id"],
        "replacementSyntax": replacement_syntax,
    }

    guidance = f"Deprecated test command. Use `run {replacement_syntax}` instead.\n"
    text = guidance + (result.text or "")

    return CommandResult(
        command=result.command,
        status=result.status,
        host_platform=result.host_platform,
        target=result.target,
        duration_ms=result.duration_ms,
        checks=result.checks,
        errors=result.errors,
        payload=payload,
        text=text,
    )


def attach_operation_report(
    result: CommandResult,
    *,
    repo_root: Path,
    host_platform: str,
    command_text: str,
    run_context: dict[str, Any],
    progress_callback: Callable[[dict], None] | None = None,
) -> CommandResult:
    payload = dict(result.payload)
    report = operation_reporting_module.finalize_operation_report(
        repo_root=repo_root,
        host_platform=host_platform,
        command_text=command_text,
        status=result.status,
        errors=list(result.errors),
        artifacts=list(payload.get("artifacts", [])),
        important_outputs=list(payload.get("importantOutputs", [])),
        console_text=str(payload.get("consoleText", result.text or "")),
        run_context=run_context,
    )
    payload.update(report)
    final_event = build_event(
        "final-summary",
        {
            "runId": report["runId"],
            "finalStatus": "ok" if result.status == "ok" else "fail",
            "exitCode": 0 if result.status == "ok" else int(payload.get("exitCode", 1) or 1),
            "errors": list(result.errors),
            "artifacts": list(payload.get("artifacts", [])),
            "importantOutputs": list(payload.get("importantOutputs", [])),
            "sessionPath": report["sessionPath"],
            "summaryPath": report["summaryPath"],
            "eventsPath": report["eventsPath"],
            "consolePath": report["consolePath"],
            "telemetryPath": report["telemetryPath"],
        },
        run_id=report["runId"],
        status="ok" if result.status == "ok" else "fail",
    )
    if progress_callback is not None:
        progress_callback(final_event)
    else:
        operation_reporting_module.append_operation_event(run_context, final_event)

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


def execute_command(
    command: dict | None,
    command_text: str,
    target: str | None,
    host_platform: str,
    manifest: dict,
    repo_root: Path,
    options: dict[str, object] | None = None,
    progress_callback: Callable[[dict], None] | None = None,
) -> CommandResult:
    if command is None:
        return CommandResult.failure(
            command=command_text,
            host_platform=host_platform,
            target=target,
            errors=[f"unknown command: {command_text}"],
            text=f"unknown command: {command_text}\n",
        )

    command_id = command["id"]
    if command_id == "menu":
        return inspect_commands.handle_menu(manifest, host_platform)
    if command_id == "help":
        return inspect_commands.handle_help(manifest, host_platform)
    if command_id == "list":
        return inspect_commands.handle_list(manifest, host_platform)
    if command_id == "capability":
        return inspect_commands.handle_capability(manifest, host_platform, target)

    if command_id == "bootstrap":
        return CommandResult.success(
            command="bootstrap",
            host_platform=host_platform,
            target=None,
            payload={"item": manifest_module.find_command(manifest, "bootstrap", host_platform)},
            text="bootstrap is handled by the wrapper. Use run bootstrap --yes.\n",
        )
    if command["handler"] == "build.dispatch":
        result = build_commands.handle(
            command,
            repo_root,
            host_platform,
            command_text,
            options or {},
            progress_callback=progress_callback,
        )
        return add_legacy_test_migration_guidance(command, result)
    if command["handler"] == "project.dispatch":
        return project_commands.handle(command, repo_root, host_platform, command_text, options or {}, progress_callback=progress_callback)
    if command["handler"] == "deploy.dispatch":
        return deploy_commands.handle(command, repo_root, host_platform, command_text, options or {})
    if command["handler"] == "test.dispatch":
        result = test_commands.handle(
            command,
            repo_root,
            host_platform,
            command_text,
            manifest,
            options or {},
            progress_callback=progress_callback,
        )
        return add_legacy_test_migration_guidance(command, result)
    if command["handler"] == "doctor.dispatch":
        return doctor_commands.handle(repo_root, host_platform, command_text)
    if command["handler"] == "prepare.dispatch":
        return prepare_commands.handle(command, repo_root, host_platform, command_text, manifest, progress_callback=progress_callback)
    if command["handler"] == "clean.dispatch":
        return clean_commands.handle(command, repo_root, host_platform, command_text)
    if command["handler"] == "benchmark.dispatch":
        # Reconstruct CLI args from the parsed options dict so that benchmark.dispatch
        # receives everything as a flat list (its internal parser handles --flags).
        opts = options or {}
        extra: list[str] = []
        if opts.get("subject"):
            extra += ["--subject", str(opts["subject"])]
        if opts.get("mode"):
            extra += ["--mode", str(opts["mode"])]
        if opts.get("record"):
            extra.append("--record")
        if opts.get("dashboard"):
            extra.append("--dashboard")
        if opts.get("open"):
            extra.append("--open")
        if opts.get("all"):
            extra.append("--all")
        if opts.get("output"):
            extra += ["--output", str(opts["output"])]
        # Also include any remaining positional tokens (e.g. "status")
        remaining_tokens = command_text.split()[1:]  # skip "benchmark"
        extra = remaining_tokens + extra
        exit_code = benchmark_commands.dispatch(extra, repo_root, host_platform)
        status = "ok" if exit_code == 0 else ("regression" if exit_code == 1 else "fail")
        return CommandResult(
            command=command_text,
            host_platform=host_platform,
            target=command.get("id", "benchmark"),
            status=status,
            errors=[],
            text=f"benchmark {status}\n",
        )

    return CommandResult.failure(
        command=command_text,
        host_platform=host_platform,
        target=target,
        errors=[f"unsupported handler: {command['handler']}"],
        text=f"unsupported handler: {command['handler']}\n",
    )


def resolve_tui_selection(
    parsed: dict[str, object],
    interactive: bool,
    manifest: dict,
    host_platform: str,
) -> tuple[dict[str, object] | None, bool]:
    command = parsed["command"]
    if parsed["json"] or command is None or command["id"] != "menu":
        return parsed, False

    if not tui_module.supports_fullscreen_tui():
        return parsed, False

    selection_argv = tui_module.run_fullscreen_menu(manifest, host_platform)
    if selection_argv is None:
        return None, True

    return manifest_module.parse_cli(selection_argv, interactive, manifest, host_platform), True


def execute_parsed_command(
    parsed: dict[str, object],
    *,
    interactive: bool,
    json_output: bool,
    host_platform: str,
    manifest: dict,
    repo_root: Path,
) -> CommandResult:
    progress_callback = None
    operation_run_context = None
    if (
        interactive
        and not json_output
        and parsed["command"] is not None
        and parsed["command"]["handler"] == "test.dispatch"
        and parsed["command"]["id"] not in {"test-list", "test-watch", "test-summary"}
    ):
        progress_callback = build_test_progress_callback(repo_root)
    elif parsed["command"] is not None and parsed["command"]["handler"] in OPERATION_HANDLERS:
        operation_run_context = operation_reporting_module.start_operation_report(
            repo_root=repo_root,
            host_platform=host_platform,
            command_text=str(parsed["command_text"]),
        )
        if interactive and not json_output:
            progress_callback = build_operation_progress_callback(repo_root, operation_run_context)
            progress_callback(
                build_event(
                    "session-start",
                    {
                        "command": str(parsed["command_text"]),
                    },
                    run_id=operation_run_context["runId"],
                    status="running",
                )
            )
        else:
            operation_reporting_module.append_operation_event(
                operation_run_context,
                build_event(
                    "session-start",
                    {
                        "command": str(parsed["command_text"]),
                    },
                    run_id=operation_run_context["runId"],
                    status="running",
                ),
            )
    result = execute_command(
        parsed["command"],
        parsed["command_text"],
        parsed["target"],
        host_platform,
        manifest,
        repo_root,
        parsed["options"],
        progress_callback=progress_callback,
    )
    if operation_run_context is not None:
        result = attach_operation_report(
            result,
            repo_root=repo_root,
            host_platform=host_platform,
            command_text=str(parsed["command_text"]),
            run_context=operation_run_context,
            progress_callback=progress_callback,
        )
    return result


def run_fullscreen_menu_session(
    *,
    interactive: bool,
    manifest: dict,
    host_platform: str,
    repo_root: Path,
) -> int:
    menu_state = tui_module.MenuState()
    selection_argv = tui_module.run_fullscreen_menu(manifest, host_platform, menu_state=menu_state)
    while selection_argv is not None:
        parsed = manifest_module.parse_cli(selection_argv, interactive, manifest, host_platform)
        result = execute_parsed_command(
            parsed,
            interactive=interactive,
            json_output=False,
            host_platform=host_platform,
            manifest=manifest,
            repo_root=repo_root,
        )
        sys.stdout.write(render_post_tui_prefix(True, result, False))
        sys.stdout.write(render_result(result, False, repo_root))
        sys.stdout.flush()
        selection_argv = tui_module.run_inline_menu(manifest, host_platform, menu_state=menu_state)
    return 0


def main(argv: list[str] | None = None) -> int:
    start = time.perf_counter()
    configure_stdout()
    argv = list(sys.argv[1:] if argv is None else argv)
    repo_root = resolve_repo_root()
    manifest = manifest_module.load_run_manifest(repo_root)
    interactive = manifest_module.is_interactive_session()
    host_platform = manifest_module.detect_host_platform_family(runtime_module.detect_host_platform())
    parsed = manifest_module.parse_cli(argv, interactive, manifest, host_platform)
    json_output = parsed["json"]
    if (
        not json_output
        and parsed["command"] is not None
        and parsed["command"]["id"] == "menu"
        and tui_module.supports_fullscreen_tui()
    ):
        return run_fullscreen_menu_session(
            interactive=interactive,
            manifest=manifest,
            host_platform=host_platform,
            repo_root=repo_root,
        )

    parsed, used_fullscreen_tui = resolve_tui_selection(parsed, interactive, manifest, host_platform)
    if parsed is None:
        result = CommandResult.success(
            command="menu",
            host_platform=host_platform,
            target=None,
            text="",
        )
    else:
        result = execute_parsed_command(
            parsed,
            interactive=interactive,
            json_output=json_output,
            host_platform=host_platform,
            manifest=manifest,
            repo_root=repo_root,
        )
    result.duration_ms = int((time.perf_counter() - start) * 1000)
    sys.stdout.write(render_post_tui_prefix(used_fullscreen_tui, result, json_output))
    sys.stdout.write(render_result(result, json_output, repo_root))
    if result.status == "ok":
        return 0
    exit_code = result.payload.get("exitCode")
    return exit_code if isinstance(exit_code, int) and exit_code > 0 else 1


if __name__ == "__main__":
    raise SystemExit(main())
