#!/usr/bin/env python3
"""PostToolUse hook: records tool call outcomes (success/failure) per skill.

Tracks which tools succeed or fail during skill-invoked sessions.
Writes to lifecycle/telemetry/tool_outcomes.jsonl.
"""

from __future__ import annotations

import json
import subprocess
import sys
from datetime import datetime, timezone
from pathlib import Path


def resolve_repo_root() -> Path | None:
    try:
        script_dir = Path(__file__).resolve().parent
        output = subprocess.run(
            ["git", "-C", str(script_dir), "rev-parse", "--show-toplevel"],
            capture_output=True, text=True, timeout=10,
        )
        root = output.stdout.strip()
        return Path(root).resolve() if root else None
    except Exception:
        return None


def get_active_skill_path(repo_root: Path) -> str | None:
    """Read the most recently read SKILL.md from usage.jsonl as active skill context."""
    usage_path = repo_root / "skills" / "lifecycle" / "telemetry" / "usage.jsonl"
    if not usage_path.exists():
        return None
    try:
        lines = usage_path.read_text(encoding="utf-8").strip().splitlines()
        if not lines:
            return None
        last = json.loads(lines[-1])
        return last.get("skill_path", None)
    except (json.JSONDecodeError, OSError):
        return None


def main() -> int:
    repo_root = resolve_repo_root()
    if not repo_root:
        return 0

    stdin_data = sys.stdin.read().strip() if not sys.stdin.isatty() else ""
    if not stdin_data:
        return 0

    try:
        tool_call = json.loads(stdin_data)
    except json.JSONDecodeError:
        return 0

    # Extract tool name: from 'tool' field or infer from the tool_call structure
    tool_name = ""
    if isinstance(tool_call, dict):
        tool_name = tool_call.get("tool", tool_call.get("name", ""))

    if not tool_name:
        return 0

    # Determine success: presence of 'error' key or exception indicates failure
    has_error = False
    error_type = ""
    if isinstance(tool_call, dict):
        if tool_call.get("error"):
            has_error = True
            error_type = str(tool_call["error"])[:100]
        if tool_call.get("exception"):
            has_error = True
            error_type = str(tool_call["exception"])[:100]

    # Map to active skill context
    skill_path = get_active_skill_path(repo_root)

    telemetry_dir = repo_root / "skills" / "lifecycle" / "telemetry"
    telemetry_dir.mkdir(parents=True, exist_ok=True)

    record = {
        "timestamp": datetime.now(timezone.utc).isoformat(),
        "tool_name": tool_name,
        "success": not has_error,
        "error_type": error_type if has_error else None,
        "skill_path": skill_path,
        "event": "tool_outcome",
    }

    log_path = telemetry_dir / "tool_outcomes.jsonl"
    with open(log_path, "a", encoding="utf-8") as f:
        f.write(json.dumps(record, ensure_ascii=False) + "\n")

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
