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


def _derive_repo_root() -> Path | None:
    """Derive repo root from script location — no git subprocess fork.

    This file lives at <repo>/.ai/skills/hooks/... (4 levels below repo root),
    so the root is 4 `.parent` hops up. Resolution uses `.resolve()` (realpath)
    so it is robust to symlinked deployments.
    """
    p = Path(__file__).resolve()
    for _ in range(4):
        p = p.parent
        if (p / ".git").exists() or (p / ".gitignore").exists():
            return p
    # Fallback: legacy git query (rare — only if hooks are not under repo root)
    try:
        script_dir = Path(__file__).resolve().parent
        output = subprocess.run(
            ["git", "-C", str(script_dir), "rev-parse", "--show-toplevel"],
            capture_output=True, text=True, timeout=5,
        )
        root = output.stdout.strip()
        return Path(root).resolve() if root else None
    except Exception:
        return None


def resolve_repo_root() -> Path | None:
    """"""  # noqa: D204 — A1: no git fork on the hot path; cached at module scope.
    global RESOLVED_REPO_ROOT
    if RESOLVED_REPO_ROOT is None:
        RESOLVED_REPO_ROOT = _derive_repo_root()
    return RESOLVED_REPO_ROOT


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


def extract_skill_name(file_path: str) -> str | None:
    m = re.search(r"skills/library/skills/([^/]+)/SKILL\.md", file_path.replace("\\", "/"))
    return m.group(1) if m else None


def detect_expert_from_input(raw: str) -> str | None:
    names = EXPERT_PATTERN.findall(raw)
    return names[-1] if names else None


def _extract_declared_expert(class_file: Path) -> str | None:
    """Read the loaded_expert line from .classified and return the expert name.

    Format: `loaded_expert:dev-il2cpp→dev-il2cpp-codegen-expert`
    Returns the expert after the arrow (or after the colon if no arrow).
    """
    try:
        for line in class_file.read_text(encoding="utf-8", errors="replace").splitlines():
            if "loaded_expert:" not in line:
                continue
            val = line.split("loaded_expert:", 1)[-1].strip()
            # Split on arrow (UTF-8 or ASCII variants) or comma; take the LAST
            # entry — the most-specific routed expert (mirrors check_classification.py).
            parts = [p.strip() for p in re.split(r"→|->|=>|,", val) if p.strip()]
            return parts[-1] if parts else None
        return None
    except (OSError, UnicodeDecodeError):
        return None


def record_skill_usage(repo_root: Path, skill_path: str, source: str) -> None:
    telemetry_dir = repo_root / ".ai" / "skills" / "lifecycle" / "telemetry"
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


def record_tool_outcome(repo_root: Path, tool_call: dict, skill_path: str | None = None) -> None:
    telemetry_dir = repo_root / ".ai" / "skills" / "lifecycle" / "telemetry"
    telemetry_dir.mkdir(parents=True, exist_ok=True)

    tool_name = tool_call.get("tool", tool_call.get("tool_name", "unknown"))
    success = tool_call.get("success", True)
    error_msg = tool_call.get("error", "") or tool_call.get("stderr", "")

    # A2: drop the noisy majority — pure-success records with no skill attribution
    # add ~0 signal (the evolution pipeline filters tool outcomes by skill_path,
    # see extract_success_pattern). Failures and skill-attributed successes stay.
    if success and not skill_path:
        return 0

    record = {
        "timestamp": datetime.now(timezone.utc).isoformat(),
        "tool_name": tool_name,
        "success": success,
        "error_preview": error_msg[:200] if error_msg else "",
        "event": "tool_outcome",
    }
    if skill_path:
        record["skill_path"] = skill_path
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
    return 0


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

    # ── Detect skill path first (shared between usage + tool outcome) ──
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

    # Method 4: Classification marker fallback — read the ACTUAL declared expert
    # from .classified instead of a generic "engineering" tag, so per-skill
    # attribution stays meaningful for the evolution/retirement pipeline.
    if not skill_path:
        class_file = repo_root / ".claude" / ".classified"
        if class_file.exists():
            declared = _extract_declared_expert(class_file)
            skill_path = declared if declared else "engineering"
            source = "classification"

    # ── Always record tool outcome (with skill_path if detected) ──
    record_tool_outcome(repo_root, tool_call, skill_path=skill_path)

    # ── Record skill usage ──
    if skill_path and source:
        record_skill_usage(repo_root, skill_path, source)

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
