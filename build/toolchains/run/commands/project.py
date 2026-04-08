from __future__ import annotations

import sys
from pathlib import Path

try:
    from .. import project_workspace as workspace_module
    from ..result import CommandResult
except ImportError:
    root = Path(__file__).resolve().parents[1]
    sys.path.insert(0, str(root))
    import project_workspace as workspace_module
    from result import CommandResult


def handle(
    command: dict,
    repo_root: Path,
    host_platform: str,
    command_text: str,
    options: dict[str, object] | None = None,
) -> CommandResult:
    try:
        kind = str(command.get("kind") or "")
        if kind == "subject-workspace":
            outcome = workspace_module.generate_subject_workspace(repo_root, host_platform, dict(options or {}))
        elif kind == "core-workspace":
            outcome = workspace_module.generate_core_workspace(repo_root, host_platform, dict(options or {}))
        else:
            raise RuntimeError(f"unsupported project kind: {kind}")
    except Exception as error:
        return CommandResult.failure(
            command=command_text,
            host_platform=host_platform,
            target=str(dict(options or {}).get("id") or dict(options or {}).get("target") or "") or None,
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
        target=str(dict(options or {}).get("id") or dict(options or {}).get("target") or "") or None,
        payload={
            "artifacts": list(outcome.get("artifacts") or []),
            "importantOutputs": list(outcome.get("importantOutputs") or []),
            "consoleText": str(outcome.get("consoleText") or ""),
            "manifestPath": outcome.get("manifestPath"),
        },
        text=f"Run completed: {command_text}\n",
    )
