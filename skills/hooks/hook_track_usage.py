#!/usr/bin/env python3
"""PostToolUse hook: records skill SKILL.md reads to telemetry/usage.jsonl.

Triggered when Claude Code reads a SKILL.md file from skills/library/skills/.
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


def is_skill_read(file_path: str) -> bool:
    """Check if the Read tool is accessing a SKILL.md in library/."""
    normalized = file_path.replace("\\", "/")
    return "skills/library/skills/" in normalized and normalized.endswith(
        ("/SKILL.md", "/skill.md")
    )


def main() -> int:
    repo_root = resolve_repo_root()
    if not repo_root:
        return 0  # Not in a git repo, silently skip

    # Read file path from stdin (Claude Code passes tool-call info)
    stdin_data = sys.stdin.read().strip() if not sys.stdin.isatty() else ""
    if not stdin_data:
        return 0

    try:
        tool_call = json.loads(stdin_data)
    except json.JSONDecodeError:
        return 0

    file_path = ""
    if isinstance(tool_call, dict):
        file_path = tool_call.get("file_path", "")

    if not file_path or not is_skill_read(file_path):
        return 0

    telemetry_dir = repo_root / "skills" / "lifecycle" / "telemetry"
    telemetry_dir.mkdir(parents=True, exist_ok=True)

    record = {
        "timestamp": datetime.now(timezone.utc).isoformat(),
        "skill_path": file_path,
        "event": "skill_read",
    }

    log_path = telemetry_dir / "usage.jsonl"
    with open(log_path, "a", encoding="utf-8") as f:
        f.write(json.dumps(record, ensure_ascii=False) + "\n")

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
