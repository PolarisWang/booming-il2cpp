#!/usr/bin/env python3
"""Stop hook: records session completion outcomes per skill.

Evaluates whether a session completed its stated goal using heuristics
(edits, commits, final tool calls). Writes to lifecycle/telemetry/session_outcomes.jsonl.
"""

from __future__ import annotations

import json
import re
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
    """Read the most recent skill from usage.jsonl."""
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


def estimate_session_completed(repo_root: Path) -> dict:
    """Heuristic: check for completion signals from recent git and transcript activity."""
    # Signal 1: recent git commit
    has_recent_commit = False
    try:
        output = subprocess.run(
            ["git", "-C", str(repo_root), "log", "--oneline", "-5", "--since=1 hour ago"],
            capture_output=True, text=True, timeout=10,
        )
        has_recent_commit = bool(output.stdout.strip())
    except Exception:
        pass

    # Signal 2: check for files edited in skills/library recently
    has_skill_edit = False
    skill_library = repo_root / "skills" / "library" / "skills"
    if skill_library.exists():
        try:
            output = subprocess.run(
                ["git", "-C", str(repo_root), "diff", "--name-only", "--since=1 hour ago"],
                capture_output=True, text=True, timeout=10,
            )
            has_skill_edit = "skills/library/skills/" in output.stdout
        except Exception:
            pass

    # Signal 3: look for session transcript with completion patterns
    completed = has_recent_commit or has_skill_edit

    return {
        "completed": completed,
        "has_recent_commit": has_recent_commit,
        "has_skill_edit": has_skill_edit,
    }


def main() -> int:
    repo_root = resolve_repo_root()
    if not repo_root:
        return 0

    skill_path = get_active_skill_path(repo_root)
    outcome = estimate_session_completed(repo_root)

    telemetry_dir = repo_root / "skills" / "lifecycle" / "telemetry"
    telemetry_dir.mkdir(parents=True, exist_ok=True)

    record = {
        "timestamp": datetime.now(timezone.utc).isoformat(),
        "skill_path": skill_path,
        "completed": outcome["completed"],
        "signals": {
            "has_recent_commit": outcome["has_recent_commit"],
            "has_skill_edit": outcome["has_skill_edit"],
        },
        "event": "session_outcome",
    }

    log_path = telemetry_dir / "session_outcomes.jsonl"
    with open(log_path, "a", encoding="utf-8") as f:
        f.write(json.dumps(record, ensure_ascii=False) + "\n")

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
