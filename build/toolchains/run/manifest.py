from __future__ import annotations

import os
import sys
from pathlib import Path
from typing import Any

if __package__ in (None, ""):

    sys.path.insert(0, str(Path(__file__).resolve().parent))
    from common import read_json
else:
    from .common import read_json


DEFAULT_RUN_MANIFEST = Path("build/toolchains/run/run_manifest.json")


def resolve_manifest_path(repo_root: Path, manifest_path: str | Path | None = None) -> Path:
    path = Path(manifest_path) if manifest_path is not None else repo_root / DEFAULT_RUN_MANIFEST
    if not path.is_absolute():
        path = repo_root / path
    return path.resolve()


def load_run_manifest(repo_root: Path, manifest_path: str | Path | None = None) -> dict[str, Any]:
    return read_json(resolve_manifest_path(repo_root, manifest_path))


def list_group_titles(manifest: dict[str, Any]) -> list[str]:
    return [group["title"] for group in manifest["groups"]]


def _is_command_visible(command: dict[str, Any]) -> bool:
    return bool(command.get("public", True))


def list_commands(
    manifest: dict[str, Any],
    host_platform: str | None = None,
    include_hidden: bool = False,
) -> list[dict[str, Any]]:
    commands = list(manifest["commands"])
    if not include_hidden:
        commands = [command for command in commands if _is_command_visible(command)]
    if host_platform is None:
        return commands
    return [command for command in commands if host_platform in command["platforms"]]


def list_commands_by_group(
    manifest: dict[str, Any],
    group_title: str,
    host_platform: str | None = None,
    include_hidden: bool = False,
) -> list[dict[str, Any]]:
    commands = list_commands(manifest, host_platform, include_hidden=include_hidden)
    return [command for command in commands if command["group"] == group_title]


def find_command(
    manifest: dict[str, Any],
    command_id: str,
    host_platform: str | None = None,
    include_hidden: bool = False,
) -> dict[str, Any] | None:
    for command in list_commands(manifest, host_platform, include_hidden=include_hidden):
        if command["id"] == command_id:
            return command
    return None


def command_argv(command: dict[str, Any]) -> list[str]:
    argv = list(command.get("tokens", [command["id"]]))
    for name, value in command.get("options", {}).items():
        argv.extend([f"--{name}", str(value)])
    return argv


def command_syntax(command: dict[str, Any]) -> str:
    return command.get("syntax") or " ".join(command_argv(command))


def detect_host_platform_family(platform_id: str) -> str:
    if platform_id.startswith("windows"):
        return "windows"
    if platform_id.startswith("macos"):
        return "macos"
    if platform_id.startswith("linux"):
        return "linux"
    return platform_id


def _stream_is_tty(name: str) -> bool:
    stream = getattr(sys, name, None)
    isatty = getattr(stream, "isatty", None)
    if not callable(isatty):
        return False

    try:
        return bool(isatty())
    except OSError:
        return False


def is_interactive_session() -> bool:
    forced = os.environ.get("BOOM_RUN_FORCE_INTERACTIVE")
    if forced == "1":
        return True
    if forced == "0":
        return False

    if os.environ.get("CI") or os.environ.get("GITHUB_ACTIONS") or os.environ.get("TF_BUILD"):
        return False

    if _stream_is_tty("stdin") and _stream_is_tty("stdout"):
        return True

    return bool(os.environ.get("TERM")) or bool(os.environ.get("WT_SESSION"))


def _split_cli(argv: list[str]) -> tuple[list[str], dict[str, Any], bool]:
    json_output = False
    positional: list[str] = []
    options: dict[str, Any] = {}
    index = 0

    while index < len(argv):
        argument = argv[index]
        if argument == "--json":
            json_output = True
            index += 1
            continue

        if argument.startswith("--"):
            option_name = argument[2:]
            if index + 1 < len(argv) and not argv[index + 1].startswith("--"):
                options[option_name] = argv[index + 1]
                index += 2
            else:
                options[option_name] = True
                index += 1
            continue

        positional.append(argument)
        index += 1

    return positional, options, json_output


def resolve_cli_command(
    manifest: dict[str, Any],
    positional: list[str],
    options: dict[str, Any],
    host_platform: str,
) -> dict[str, Any] | None:
    candidates = sorted(
        list_commands(manifest, host_platform, include_hidden=True),
        key=lambda item: (len(item.get("tokens", [])), len(item.get("options", {}))),
        reverse=True,
    )
    for command in candidates:
        tokens = command.get("tokens", [])
        if positional[: len(tokens)] != tokens:
            continue

        if command["id"] == "capability":
            return command

        if len(positional) != len(tokens):
            continue

        required_options = command.get("options", {})
        if any(options.get(name) != value for name, value in required_options.items()):
            continue

        return command

    return None


def resolve_dynamic_test_command(
    manifest: dict[str, Any],
    positional: list[str],
    options: dict[str, Any],
    host_platform: str,
) -> tuple[dict[str, Any] | None, str | None, dict[str, Any]]:
    if not positional or positional[0] != "test":
        return None, None, options

    merged_options = dict(options)

    if len(positional) >= 2 and positional[1] == "list":
        if len(positional) > 3:
            return None, None, merged_options

        family = positional[2] if len(positional) == 3 else None
        if family is not None:
            merged_options["family"] = family
        command = find_command(manifest, "test-list", host_platform, include_hidden=True)
        return command, family, merged_options

    if len(positional) == 2 and positional[1] == "all":
        command = find_command(manifest, "test-all", host_platform, include_hidden=True)
        return command, "all", merged_options

    if len(positional) == 3 and positional[2] == "all":
        merged_options["family"] = positional[1]
        command = find_command(manifest, "test-family-all", host_platform, include_hidden=True)
        return command, positional[1], merged_options

    if len(positional) == 3:
        merged_options["family"] = positional[1]
        merged_options["suite"] = positional[2]
        command = find_command(manifest, "test-family-suite", host_platform, include_hidden=True)
        return command, f"{positional[1]}/{positional[2]}", merged_options

    return None, None, merged_options


def parse_cli(argv: list[str], interactive: bool, manifest: dict[str, Any], host_platform: str) -> dict[str, Any]:
    positional, options, json_output = _split_cli(argv)

    if not positional:
        fallback_id = "menu" if interactive else "help"
        return {
            "command": find_command(manifest, fallback_id, host_platform),
            "command_text": fallback_id,
            "target": None,
            "json": json_output,
            "options": options,
        }

    command, target, merged_options = resolve_dynamic_test_command(manifest, positional, options, host_platform)
    if command is None:
        command = resolve_cli_command(manifest, positional, options, host_platform)
        target = None
        merged_options = options
    if command is None:
        return {
            "command": None,
            "command_text": " ".join(positional),
            "target": None,
            "json": json_output,
            "options": merged_options,
        }

    if command["id"] == "capability":
        target = positional[1] if len(positional) > 1 else None
        command_text = "capability"
    else:
        target = target if target is not None else command.get("target")
        command_text = " ".join(positional)
        for name, value in command.get("options", {}).items():
            command_text += f" --{name} {value}"

    return {
        "command": command,
        "command_text": command_text,
        "target": target,
        "json": json_output,
        "options": merged_options,
    }
