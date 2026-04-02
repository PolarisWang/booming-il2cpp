from __future__ import annotations

from typing import Any


def render_menu(manifest: dict[str, Any], visible_commands_by_group: dict[str, list[dict[str, Any]]]) -> str:
    lines = ["Unified Run Menu", ""]

    for group in manifest["groups"]:
        title = group["title"]
        commands = visible_commands_by_group.get(title, [])
        if not commands:
            continue

        lines.append(title)
        for command in commands:
            lines.append(f"  {command['id']}: {command['title']}")
        lines.append("")

    return "\n".join(lines).rstrip() + "\n"
