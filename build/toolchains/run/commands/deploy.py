from __future__ import annotations

import sys
from pathlib import Path

try:
    from ..subject import deploy_layout as deploy_layout_module
    from ..core.result import CommandResult
except ImportError:
    root = Path(__file__).resolve().parents[1]
    sys.path.insert(0, str(root))
    from subject import deploy_layout as deploy_layout_module
    from core.result import CommandResult


def handle(
    command: dict,
    repo_root: Path,
    host_platform: str,
    command_text: str,
    options: dict[str, object] | None = None,
) -> CommandResult:
    try:
        kind = str(command.get("kind") or "")
        if kind != "core-deploy":
            raise RuntimeError(f"unsupported deploy kind: {kind}")
        outcome = deploy_layout_module.deploy_core_outputs(repo_root, host_platform, dict(options or {}))
    except Exception as error:
        return CommandResult.failure(
            command=command_text,
            host_platform=host_platform,
            target=str(dict(options or {}).get("target") or "") or None,
            errors=[str(error)],
            payload={
                "artifacts": [],
                "importantOutputs": [],
                "consoleText": str(error),
            },
            text=f"Run failed: {command_text}\n- {error}\n",
        )

    return CommandResult.success(
        command=command_text,
        host_platform=host_platform,
        target=str(dict(options or {}).get("target") or "") or None,
        payload={
            "artifacts": list(outcome.get("artifacts") or []),
            "importantOutputs": list(outcome.get("importantOutputs") or []),
            "consoleText": str(outcome.get("consoleText") or ""),
        },
        text=f"Run completed: {command_text}\n",
    )
