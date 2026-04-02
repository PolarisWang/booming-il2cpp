from __future__ import annotations

import sys
import time
from pathlib import Path

if __package__ in (None, ""):
    sys.path.insert(0, str(Path(__file__).resolve().parent))
    from commands import clean as clean_commands
    from commands import doctor as doctor_commands
    from commands import build as build_commands
    from commands import inspect as inspect_commands
    from commands import prepare as prepare_commands
    from commands import test as test_commands
    from commands import verify as verify_commands
    import manifest as manifest_module
    import runtime as runtime_module
    from result import CommandResult
else:
    from .commands import clean as clean_commands
    from .commands import doctor as doctor_commands
    from .commands import build as build_commands
    from .commands import inspect as inspect_commands
    from .commands import prepare as prepare_commands
    from .commands import test as test_commands
    from .commands import verify as verify_commands
    from . import manifest as manifest_module
    from . import runtime as runtime_module
    from .result import CommandResult


def resolve_repo_root() -> Path:
    return Path(__file__).resolve().parents[3]


def render_result(result: CommandResult, json_output: bool) -> str:
    if json_output:
        return result.to_json() + "\n"

    return result.text or ""


def execute_command(
    command: dict | None,
    command_text: str,
    target: str | None,
    host_platform: str,
    manifest: dict,
    repo_root: Path,
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
        return build_commands.handle(command, repo_root, host_platform, command_text)
    if command["handler"] == "test.dispatch":
        return test_commands.handle(command, repo_root, host_platform, command_text)
    if command["handler"] == "verify.dispatch":
        return verify_commands.handle(command, repo_root, host_platform, command_text)
    if command["handler"] == "doctor.dispatch":
        return doctor_commands.handle(repo_root, host_platform, command_text)
    if command["handler"] == "prepare.dispatch":
        return prepare_commands.handle(command, repo_root, host_platform, command_text, manifest)
    if command["handler"] == "clean.dispatch":
        return clean_commands.handle(command, repo_root, host_platform, command_text)

    return CommandResult.failure(
        command=command_text,
        host_platform=host_platform,
        target=target,
        errors=[f"unsupported handler: {command['handler']}"],
        text=f"unsupported handler: {command['handler']}\n",
    )


def main(argv: list[str] | None = None) -> int:
    start = time.perf_counter()
    argv = list(sys.argv[1:] if argv is None else argv)
    repo_root = resolve_repo_root()
    manifest = manifest_module.load_run_manifest(repo_root)
    interactive = manifest_module.is_interactive_session()
    host_platform = manifest_module.detect_host_platform_family(runtime_module.detect_host_platform())
    parsed = manifest_module.parse_cli(argv, interactive, manifest, host_platform)
    result = execute_command(parsed["command"], parsed["command_text"], parsed["target"], host_platform, manifest, repo_root)
    result.duration_ms = int((time.perf_counter() - start) * 1000)
    sys.stdout.write(render_result(result, parsed["json"]))
    return 0 if result.status == "ok" else 1


if __name__ == "__main__":
    raise SystemExit(main())
