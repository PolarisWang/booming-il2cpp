#!/usr/bin/env python3
"""PostToolUse hook: records tool outcomes + skill usage in one invocation.

Merges the functionality of hook_track_usage.py and hook_track_tool_outcomes.py
to reduce hook overhead from two Python invocations to one.

Writes to:
  - lifecycle/telemetry/usage.jsonl (skill activation events)
  - lifecycle/telemetry/tool_outcomes.jsonl (tool call results)
  - lifecycle/telemetry/failure-patterns.jsonl (on failure)
"""

from __future__ import annotations

import json
import re
import subprocess
import sys
from datetime import datetime, timezone
from pathlib import Path


RESOLVED_REPO_ROOT: Path | None = None
SKILL_PATH_PATTERN = re.compile(r"skills/library/skills/[^/]+/SKILL\.md", re.IGNORECASE)
EXPERT_PATTERN = re.compile(r"dev-il2cpp-[-a-z]+|dev-project-[-a-z]+", re.IGNORECASE)

# R8: JSONL 文件大小上限 10MB，超出后自动轮转
_MAX_JSONL_BYTES = 10 * 1024 * 1024


def _rotate_jsonl_if_needed(path: Path) -> None:
    """Rotate JSONL file if it exceeds size limit."""
    if path.exists() and path.stat().st_size > _MAX_JSONL_BYTES:
        timestamp = datetime.now(timezone.utc).strftime("%Y%m%d_%H%M%S")
        rotated = path.with_name(f"{path.stem}.{timestamp}.jsonl")
        try:
            path.rename(rotated)
        except OSError:
            pass  # Best-effort; swallow to never break the hook


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


def extract_skill_name(file_path: str) -> str | None:
    m = re.search(r"skills/library/skills/([^/]+)/SKILL\.md", file_path.replace("\\", "/"))
    return m.group(1) if m else None


def detect_expert_from_input(raw: str) -> str | None:
    names = EXPERT_PATTERN.findall(raw)
    return names[-1] if names else None


def record_skill_usage(repo_root: Path, skill_path: str, source: str) -> None:
    telemetry_dir = repo_root / "skills" / "lifecycle" / "telemetry"
    telemetry_dir.mkdir(parents=True, exist_ok=True)
    record = {
        "timestamp": datetime.now(timezone.utc).isoformat(),
        "skill_path": skill_path,
        "event": "skill_activated",
        "source": source,
    }
    log_path = telemetry_dir / "usage.jsonl"
    _rotate_jsonl_if_needed(log_path)
    with open(log_path, "a", encoding="utf-8") as f:
        f.write(json.dumps(record, ensure_ascii=False) + "\n")


def record_tool_outcome(repo_root: Path, tool_call: dict) -> None:
    telemetry_dir = repo_root / "skills" / "lifecycle" / "telemetry"
    telemetry_dir.mkdir(parents=True, exist_ok=True)

    tool_name = tool_call.get("tool", tool_call.get("tool_name", "unknown"))
    success = tool_call.get("success", True)
    error_msg = tool_call.get("error", "") or tool_call.get("stderr", "")

    record = {
        "timestamp": datetime.now(timezone.utc).isoformat(),
        "tool_name": tool_name,
        "success": success,
        "error_preview": error_msg[:200] if error_msg else "",
        "event": "tool_outcome",
    }
    log_path = telemetry_dir / "tool_outcomes.jsonl"
    _rotate_jsonl_if_needed(log_path)
    with open(log_path, "a", encoding="utf-8") as f:
        f.write(json.dumps(record, ensure_ascii=False) + "\n")

    # On failure, write enriched error context for evolution pipeline
    if not success and error_msg:
        failure = dict(record)
        failure["event"] = "failure_pattern"
        failure["error_snippet"] = error_msg[:500]
        failure_path = telemetry_dir / "failure-patterns.jsonl"
        _rotate_jsonl_if_needed(failure_path)
        with open(failure_path, "a", encoding="utf-8") as f:
            f.write(json.dumps(failure, ensure_ascii=False) + "\n")


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

    # ── Always record tool outcome ──
    record_tool_outcome(repo_root, tool_call)

    # ── Record skill usage ──
    skill_path = None
    source = None

    # Method 1: Direct SKILL.md file read via Read tool
    fp = tool_call.get("file_path", "") if isinstance(tool_call, dict) else ""
    if fp and SKILL_PATH_PATTERN.search(fp):
        skill_path = extract_skill_name(fp)
        source = "read_tool"

    # Method 2: Skill tool invocation
    if not skill_path:
        tool = tool_call.get("tool", tool_call.get("name", ""))
        if tool == "Skill":
            skill_path = tool_call.get("skill", "") or tool_call.get("input", {}).get("skill", "")
            source = "skill_tool"

    # Method 3: Expert detected in input
    if not skill_path:
        raw = json.dumps(tool_call)
        skill_path = detect_expert_from_input(raw)
        if skill_path:
            source = "expert_detected"

    # Method 4: Classification marker fallback
    if not skill_path:
        class_file = repo_root / ".claude" / ".classified"
        if class_file.exists():
            skill_path = "engineering"
            source = "classification"

    if skill_path and source:
        record_skill_usage(repo_root, skill_path, source)

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
