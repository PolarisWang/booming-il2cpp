from __future__ import annotations

import sys
from pathlib import Path

try:
    from ..result import CommandResult
except ImportError:
    root = Path(__file__).resolve().parents[1]
    sys.path.insert(0, str(root))
    from result import CommandResult


def handle(host_platform: str) -> CommandResult:
    return CommandResult.success(
        command="bootstrap",
        host_platform=host_platform,
        target=None,
        text="bootstrap is handled by the wrapper. Use run bootstrap --yes.\n",
    )
