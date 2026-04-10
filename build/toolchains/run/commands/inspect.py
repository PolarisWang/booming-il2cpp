from __future__ import annotations

import sys
from pathlib import Path
from typing import Any

try:
    from ..core import manifest as manifest_module
    from .. import menu as menu_module
    from ..core.result import CommandResult
except ImportError:
    root = Path(__file__).resolve().parents[1]
    sys.path.insert(0, str(root))
    from core import manifest as manifest_module
    import menu as menu_module
    from core.result import CommandResult


def render_help(manifest: dict[str, Any], host_platform: str) -> str:
    lines = ["Available commands", ""]
    for command in manifest_module.list_commands(manifest, host_platform):
        lines.append(f"{manifest_module.command_syntax(command)}: {command['title']}")
    return "\n".join(lines) + "\n"


def handle_menu(manifest: dict[str, Any], host_platform: str) -> CommandResult:
    commands_by_group: dict[str, list[dict[str, Any]]] = {}
    for group in manifest_module.list_group_titles(manifest):
        visible = [
            command
            for command in manifest_module.list_commands_by_group(manifest, group, host_platform)
            if command.get("show_in_menu", True)
        ]
        if visible:
            commands_by_group[group] = visible

    text = menu_module.render_menu(manifest, commands_by_group)
    return CommandResult.success(
        command="menu",
        host_platform=host_platform,
        target=None,
        payload={"items": commands_by_group},
        text=text,
    )


def handle_help(manifest: dict[str, Any], host_platform: str) -> CommandResult:
    text = render_help(manifest, host_platform)
    return CommandResult.success(
        command="help",
        host_platform=host_platform,
        target=None,
        payload={"items": manifest_module.list_commands(manifest, host_platform)},
        text=text,
    )


def handle_list(manifest: dict[str, Any], host_platform: str) -> CommandResult:
    items = manifest_module.list_commands(manifest, host_platform)
    return CommandResult.success(
        command="list",
        host_platform=host_platform,
        target=None,
        payload={"items": items},
        text=render_help(manifest, host_platform),
    )


def handle_capability(manifest: dict[str, Any], host_platform: str, target: str | None) -> CommandResult:
    if not target:
        return CommandResult.failure(
            command="capability",
            host_platform=host_platform,
            target=None,
            errors=["capability id is required"],
            text="capability id is required\n",
        )

    command = manifest_module.find_command(manifest, target, host_platform)
    if command is None:
        return CommandResult.failure(
            command="capability",
            host_platform=host_platform,
            target=target,
            errors=[f"unknown capability: {target}"],
            text=f"unknown capability: {target}\n",
        )

    lines = [
        f"Capability: {command['id']}",
        f"Title: {command['title']}",
        f"Group: {command['group']}",
        f"Handler: {command['handler']}",
    ]
    lines.append(f"Syntax: {manifest_module.command_syntax(command)}")
    for example in command.get("examples", []):
        lines.append(f"Example: {example}")

    return CommandResult.success(
        command="capability",
        host_platform=host_platform,
        target=target,
        payload={"item": command},
        text="\n".join(lines) + "\n",
    )
