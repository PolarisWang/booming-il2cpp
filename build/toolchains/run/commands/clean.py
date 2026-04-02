from __future__ import annotations

import shutil
import sys
from pathlib import Path

try:
    from ..result import CommandResult
    from . import prepare as prepare_module
except ImportError:
    root = Path(__file__).resolve().parents[1]
    sys.path.insert(0, str(root))
    from result import CommandResult
    from commands import prepare as prepare_module


def handle(command: dict, repo_root: Path, host_platform: str, command_text: str) -> CommandResult:
    scope = command.get("clean_scope", "global")
    removed: list[str] = []

    for path in prepare_module.resolve_clean_paths(repo_root, scope):
        if not path.exists():
            continue
        if path.is_dir():
            shutil.rmtree(path, ignore_errors=True)
        else:
            path.unlink(missing_ok=True)
        removed.append(str(path))

    return CommandResult.success(
        command=command_text,
        host_platform=host_platform,
        target=scope,
        payload={"cleanScope": scope, "removedPaths": removed},
        text=f"clean scope '{scope}' completed\n",
    )
