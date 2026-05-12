#!/usr/bin/env python3
"""Stop hook: records session completion outcomes and extracts success patterns.

Evaluates whether a session completed its stated goal using heuristics
(edits, commits, final tool calls). Writes to lifecycle/telemetry/session_outcomes.jsonl.
On successful completion, also extracts and records the tool-use pattern
to lifecycle/telemetry/success-patterns.jsonl for the evolution pipeline.
"""

from __future__ import annotations

import json
import re
import subprocess
import sys
from datetime import datetime, timezone
from pathlib import Path


RESOLVED_REPO_ROOT: Path | None = None


def resolve_repo_root() -> Path | None:
    global RESOLVED_REPO_ROOT
    if RESOLVED_REPO_ROOT is not None:
        return RESOLVED_REPO_ROOT
    try:
        script_dir = Path(__file__).resolve().parent
        output = subprocess.run(
            ["git", "-C", str(script_dir), "rev-parse", "--show-toplevel"],
            capture_output=True, text=True, timeout=10,
        )
        root = output.stdout.strip()
        RESOLVED_REPO_ROOT = Path(root).resolve() if root else None
        return RESOLVED_REPO_ROOT
    except Exception:
        return None


def load_jsonl(path: Path, max_records: int = 0) -> list[dict]:
    if not path.exists() or path.stat().st_size == 0:
        return []
    records = []
    try:
        for line in path.read_text(encoding="utf-8").strip().splitlines():
            if line.strip():
                records.append(json.loads(line))
                if max_records > 0 and len(records) >= max_records:
                    break
    except (json.JSONDecodeError, OSError):
        return records
    return records


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
    """Heuristic: check for completion signals from recent git and telemetry activity."""
    has_recent_commit = False
    try:
        output = subprocess.run(
            ["git", "-C", str(repo_root), "log", "--oneline", "-5", "--since=1 hour ago"],
            capture_output=True, text=True, timeout=10,
        )
        has_recent_commit = bool(output.stdout.strip())
    except Exception:
        pass

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

    completed = has_recent_commit or has_skill_edit

    return {
        "completed": completed,
        "has_recent_commit": has_recent_commit,
        "has_skill_edit": has_skill_edit,
    }


def extract_success_pattern(repo_root: Path, skill_path: str | None) -> dict | None:
    """Extract the tool-use pattern that led to success from recent telemetry.

    Reads the last N tool outcomes for this skill and identifies the
    sequence of tools used, the ratio of edits to reads, and common file targets.
    This becomes the 'success pattern' record that FIX/CAPTURE can learn from.
    """
    telemetry_dir = repo_root / "skills" / "lifecycle" / "telemetry"
    tool_path = telemetry_dir / "tool_outcomes.jsonl"

    tools = load_jsonl(tool_path, 200)

    # Filter to current skill
    if skill_path:
        skill_tools = [t for t in tools if t.get("skill_path") == skill_path]
    else:
        skill_tools = tools

    if not skill_tools:
        return None

    # Count tool types in the sequence
    tool_counts: dict[str, int] = {}
    file_targets: list[str] = []
    successful_count = 0
    failed_count = 0

    for t in skill_tools:
        name = t.get("tool_name", "unknown")
        tool_counts[name] = tool_counts.get(name, 0) + 1
        if t.get("success", True):
            successful_count += 1
        else:
            failed_count += 1

    # Determine the dominant pattern: Read-heavy, Edit-heavy, or Balanced
    reads = tool_counts.get("Read", 0)
    writes = tool_counts.get("Write", 0) + tool_counts.get("Edit", 0)
    bash = tool_counts.get("Bash", 0)
    total = len(skill_tools)

    if reads > writes + bash and reads > total * 0.5:
        pattern_type = "investigation"
    elif writes > reads and writes > total * 0.3:
        pattern_type = "implementation"
    elif bash > reads + writes:
        pattern_type = "execution"
    else:
        pattern_type = "mixed"

    success_rate = successful_count / max(total, 1)

    return {
        "timestamp": datetime.now(timezone.utc).isoformat(),
        "skill_path": skill_path,
        "pattern_type": pattern_type,
        "tool_sequence_summary": tool_counts,
        "total_tool_calls": total,
        "success_rate": round(success_rate, 4),
        "pattern_description": _describe_pattern(pattern_type, tool_counts, total),
        "event": "success_pattern",
    }


def _describe_pattern(pattern_type: str, tool_counts: dict, total: int) -> str:
    reads = tool_counts.get("Read", 0)
    writes = tool_counts.get("Write", 0) + tool_counts.get("Edit", 0)
    bash_calls = tool_counts.get("Bash", 0)

    if pattern_type == "investigation":
        return f"查探主导: {reads}次Read, {bash_calls}次执行验证"
    elif pattern_type == "implementation":
        return f"实现主导: {writes}次Edit/Write, {reads}次参考阅读"
    elif pattern_type == "execution":
        return f"执行主导: {bash_calls}次命令, {writes}次修改"
    else:
        return f"混合模式: {total}次工具调用, Read={reads}, Write/Edit={writes}, Bash={bash_calls}"


def main() -> int:
    repo_root = resolve_repo_root()
    if not repo_root:
        return 0

    skill_path = get_active_skill_path(repo_root)
    outcome = estimate_session_completed(repo_root)

    telemetry_dir = repo_root / "skills" / "lifecycle" / "telemetry"
    telemetry_dir.mkdir(parents=True, exist_ok=True)

    # Always write session outcome
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

    # On successful completion, extract and record success pattern
    if outcome["completed"]:
        pattern = extract_success_pattern(repo_root, skill_path)
        if pattern:
            pattern_path = telemetry_dir / "success-patterns.jsonl"
            with open(pattern_path, "a", encoding="utf-8") as f:
                f.write(json.dumps(pattern, ensure_ascii=False) + "\n")

    return 0


if __name__ == "__main__":
    raise SystemExit(main())