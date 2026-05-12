#!/usr/bin/env python3
"""PostToolUse hook: records tool call outcomes and captures failure patterns.

Tracks which tools succeed or fail during skill-invoked sessions.
Writes to lifecycle/telemetry/tool_outcomes.jsonl.
On failure, also writes enriched error context to lifecycle/telemetry/failure-patterns.jsonl
for the evolution pipeline to learn from.
"""

from __future__ import annotations

import json
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


def extract_error_context(tool_call: dict) -> dict:
    """Extract enriched error context for failure pattern learning."""
    error_msg = tool_call.get("error", "") or tool_call.get("exception", "") or ""
    tool_name = tool_call.get("tool", tool_call.get("name", ""))
    file_path = tool_call.get("file_path", tool_call.get("path", ""))

    # Classify error type for pattern matching
    error_class = "unknown"
    error_lower = str(error_msg).lower()

    if "timeout" in error_lower:
        error_class = "timeout"
    elif "permission" in error_lower or "denied" in error_lower or "unauthorized" in error_lower:
        error_class = "permission"
    elif "not found" in error_lower or "does not exist" in error_lower or "enoent" in error_lower:
        error_class = "not_found"
    elif "syntax" in error_lower or "parse" in error_lower:
        error_class = "syntax_error"
    elif "compile" in error_lower or "build" in error_lower:
        error_class = "build_failure"
    elif "import" in error_lower or "module" in error_lower:
        error_class = "import_error"
    elif "connect" in error_lower or "refused" in error_lower or "network" in error_lower:
        error_class = "network_error"
    elif "memory" in error_lower or "alloc" in error_lower:
        error_class = "memory_error"

    # Truncate long error messages
    if len(error_msg) > 200:
        error_msg = error_msg[:200] + "..."

    return {
        "error_class": error_class,
        "error_preview": error_msg[:120],
        "tool_name": tool_name,
        "file_path": file_path,
    }


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

    # Extract tool name
    tool_name = ""
    if isinstance(tool_call, dict):
        tool_name = tool_call.get("tool", tool_call.get("name", ""))

    if not tool_name:
        return 0

    # Determine success/failure
    has_error = False
    error_type = ""
    if isinstance(tool_call, dict):
        if tool_call.get("error"):
            has_error = True
            error_type = str(tool_call["error"])[:100]
        if tool_call.get("exception"):
            has_error = True
            error_type = str(tool_call["exception"])[:100]

    skill_path = get_active_skill_path(repo_root)

    telemetry_dir = repo_root / "skills" / "lifecycle" / "telemetry"

    # Always write tool_outcomes.jsonl
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

    # On failure, write enriched failure pattern
    if has_error and isinstance(tool_call, dict):
        error_ctx = extract_error_context(tool_call)
        failure_record = {
            "timestamp": datetime.now(timezone.utc).isoformat(),
            "error_class": error_ctx["error_class"],
            "error_preview": error_ctx["error_preview"],
            "tool_name": tool_name,
            "file_path": error_ctx["file_path"],
            "skill_path": skill_path,
            "error_type": error_type,
            "event": "failure_pattern",
        }
        failure_path = telemetry_dir / "failure-patterns.jsonl"
        with open(failure_path, "a", encoding="utf-8") as f:
            f.write(json.dumps(failure_record, ensure_ascii=False) + "\n")

    return 0


if __name__ == "__main__":
    raise SystemExit(main())