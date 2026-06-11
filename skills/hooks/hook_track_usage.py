#!/usr/bin/env python3
"""PostToolUse hook: records skill/expert-agent usage to telemetry.

Triggers on:
1. Read tool accessing skills/library/skills/*/SKILL.md (direct file read)
2. Skill tool invocation (dev-il2cpp-*-expert, dev-project-*, etc.)
3. Any tool call mentioning an expert agent pattern in its input/output
"""

from __future__ import annotations

import json
import re
import subprocess
import sys
from datetime import datetime, timezone
from pathlib import Path


SKILL_PATH_PATTERN = re.compile(r"skills/library/skills/[^/]+/SKILL\.md", re.IGNORECASE)
EXPERT_PATTERN = re.compile(r"dev-il2cpp-[-a-z]+|dev-project-[-a-z]+", re.IGNORECASE)


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


def extract_skill_name(file_path: str) -> str | None:
    """Extract skill name from SKILL.md path like '.../dev-il2cpp-codegen-expert/SKILL.md'."""
    m = re.search(r"skills/library/skills/([^/]+)/SKILL\.md", file_path.replace("\\", "/"))
    return m.group(1) if m else None


def detect_expert_from_input(tool_call: dict) -> str | None:
    """Detect if this tool call invokes an Expert Agent, return skill name."""
    raw = json.dumps(tool_call)
    names = EXPERT_PATTERN.findall(raw)
    if names:
        return names[-1]
    return None


def is_skill_tool(tool_call: dict) -> str | None:
    """Check if this is a Skill tool invocation, return the skill name."""
    tool = tool_call.get("tool", tool_call.get("name", ""))
    if tool == "Skill":
        return tool_call.get("skill", "") or tool_call.get("input", {}).get("skill", "")
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

    skill_path = None
    source = None

    # Method 1: Direct SKILL.md file read via Read tool
    fp = tool_call.get("file_path", "") if isinstance(tool_call, dict) else ""
    if fp and SKILL_PATH_PATTERN.search(fp):
        skill_path = extract_skill_name(fp)
        source = "read_tool"

    # Method 2: Skill tool invocation
    if not skill_path:
        skill_name = is_skill_tool(tool_call)
        if skill_name:
            skill_path = str(skill_name)
            source = "skill_tool"

    # Method 3: Any tool call mentioning an expert agent
    if not skill_path:
        skill_path = detect_expert_from_input(tool_call)
        if skill_path:
            source = "expert_detected"

    # Method 4: Classification marker — use as fallback for all non-Skill tools
    if not skill_path:
        ckass_dir = repo_root / ".claude" if repo_root else Path(".claude")
        ckass_file = ckass_dir / ".classified"
        if ckass_file.exists():
            skill_path = "engineering"
            source = "classification"

    if not skill_path:
        return 0

    telemetry_dir = repo_root / "skills" / "lifecycle" / "telemetry"
    telemetry_dir.mkdir(parents=True, exist_ok=True)

    record = {
        "timestamp": datetime.now(timezone.utc).isoformat(),
        "skill_path": str(skill_path),
        "event": "skill_activated",
        "source": source,
    }

    log_path = telemetry_dir / "usage.jsonl"
    with open(log_path, "a", encoding="utf-8") as f:
        f.write(json.dumps(record, ensure_ascii=False) + "\n")

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
