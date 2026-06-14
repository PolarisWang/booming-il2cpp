#!/usr/bin/env python3
"""Stop hook: evaluates session quality using telemetry data.

Replaces the previous transcript-text-parsing approach which was broken
because session JSON files only contain metadata, not conversation text.
Now reads telemetry JSONL files to compute quality metrics:
  - tool_success_rate from tool_outcomes.jsonl
  - edit intensity from tool_outcomes (Edit/Write calls)
  - completion signal from session_outcomes.jsonl
  - recently used skills from usage.jsonl
"""

from __future__ import annotations

import json
import subprocess
import sys
from datetime import datetime, timezone
from pathlib import Path

# R8: JSONL 文件大小上限 10MB
_MAX_JSONL_BYTES = 10 * 1024 * 1024


def _rotate_jsonl_if_needed(path: Path) -> None:
    if path.exists() and path.stat().st_size > _MAX_JSONL_BYTES:
        timestamp = datetime.now(timezone.utc).strftime("%Y%m%d_%H%M%S")
        rotated = path.with_name(f"{path.stem}.{timestamp}.jsonl")
        try:
            path.rename(rotated)
        except OSError:
            pass


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


def estimate_quality_from_telemetry(repo_root: Path) -> dict:
    """Compute quality score from telemetry data instead of session transcript."""
    telemetry_dir = repo_root / "skills" / "lifecycle" / "telemetry"
    usage_path = telemetry_dir / "usage.jsonl"
    tool_path = telemetry_dir / "tool_outcomes.jsonl"
    session_path = telemetry_dir / "session_outcomes.jsonl"

    # Load recent records (last 500 to keep it fast)
    tools = load_jsonl(tool_path, 500)
    sessions = load_jsonl(session_path, 500)
    usage = load_jsonl(usage_path, 500)

    if not tools and not sessions and not usage:
        return {"score": 0.0, "reason": "no telemetry data"}

    # 1. tool_success_rate
    total_tools = len(tools)
    successful_tools = sum(1 for t in tools if t.get("success", True))
    tool_success_rate = successful_tools / max(total_tools, 1)

    # 2. edit intensity (Edit/Write calls as proxy for real work)
    edit_tools = [t for t in tools if t.get("tool_name") in ("Edit", "Write")]
    edit_count = len(edit_tools)
    edit_ratio = edit_count / max(total_tools, 1)

    # 3. completion signal from session records
    completed_sessions = sum(1 for s in sessions if s.get("completed", False))
    total_sessions = len(sessions)
    completion_rate = completed_sessions / max(total_sessions, 1)

    # 4. skill usage breadth
    unique_skills = len(set(
        u.get("skill_path", "") for u in usage if u.get("skill_path")
    ))

    # Compute composite quality score (0.0 ~ 1.0)
    score = 0.0
    details = []

    # Tool success (max 0.30)
    if tool_success_rate >= 0.95:
        score += 0.30
        details.append(f"tool_ok={tool_success_rate:.2f}")
    elif tool_success_rate >= 0.8:
        score += 0.15
        details.append(f"tool_ok={tool_success_rate:.2f}")

    # Edit intensity (max 0.30)
    if edit_ratio >= 0.15:
        score += 0.30
        details.append(f"edit_ratio={edit_ratio:.2f}")
    elif edit_ratio >= 0.08:
        score += 0.15
        details.append(f"edit_ratio={edit_ratio:.2f}")

    # Completion (max 0.25)
    if completion_rate >= 0.8:
        score += 0.25
        details.append(f"complete={completion_rate:.2f}")
    elif completion_rate >= 0.5:
        score += 0.10
        details.append(f"complete={completion_rate:.2f}")

    # Skill breadth bonus (max 0.15)
    if unique_skills >= 3:
        score += 0.15
        details.append(f"skills={unique_skills}")
    elif unique_skills >= 2:
        score += 0.08
        details.append(f"skills={unique_skills}")

    # Detect domain from skill paths
    domain_hint = _detect_domain_from_skills(usage)
    action_hint = _detect_action_from_telemetry(completion_rate, tool_success_rate, edit_count)

    return {
        "score": round(min(1.0, score), 2),
        "tool_success_rate": round(tool_success_rate, 4),
        "edit_count": edit_count,
        "completion_rate": round(completion_rate, 4),
        "unique_skills": unique_skills,
        "domain_hint": domain_hint,
        "action_hint": action_hint,
        "details": "; ".join(details) if details else "no significant signals",
    }


def _detect_domain_from_skills(usage: list[dict]) -> str:
    """Detect domain from recently used skill paths."""
    domain_skills = {
        "workflow": ["dev-brainstorm", "dev-roadmap", "dev-writing-plans", "dev-executing-plans", "dev-dispatching"],
        "quality": ["dev-verification", "dev-receiving-code-review", "dev-requesting-code-review", "dev-trace"],
        "testing": ["dev-foundation-dll", "dev-test-driven-development", "dev-project-test-governance"],
        "il2cpp": ["dev-architecture-first-development", "dev-il2cpp"],
        "knowledge": ["dev-project-wiki"],
        "skilling": ["dev-skill-evolution", "dev-writing-skills"],
    }
    for u in reversed(usage):
        path = u.get("skill_path", "")
        for domain, skills in domain_skills.items():
            if any(s in path for s in skills):
                return domain
    return "engineering"


def _detect_action_from_telemetry(
    completion_rate: float, tool_success_rate: float, edit_count: int
) -> str:
    """Detect action type from telemetry patterns."""
    if completion_rate >= 0.8 and edit_count >= 5:
        return "feature"
    if completion_rate < 0.3 and tool_success_rate < 0.8:
        return "bugfix"
    if edit_count >= 3:
        return "refine"
    return "feature"


def main() -> int:
    repo_root = resolve_repo_root()
    if not repo_root:
        return 0

    quality = estimate_quality_from_telemetry(repo_root)
    if quality["score"] < 0.5:
        return 0  # Not a high-quality enough period to record

    signals_dir = repo_root / "skills" / "lifecycle" / "learning" / "signals"
    signals_dir.mkdir(parents=True, exist_ok=True)

    record = {
        "timestamp": datetime.now(timezone.utc).isoformat(),
        "quality_score": quality["score"],
        "domain_hint": quality["domain_hint"],
        "action_hint": quality["action_hint"],
        "metrics": {
            "tool_calls": 0,  # no longer track from transcript
            "tool_success_rate": quality["tool_success_rate"],
            "edit_count": quality["edit_count"],
            "completion_rate": quality["completion_rate"],
            "unique_skills": quality["unique_skills"],
        },
        "details": quality["details"],
        "source": "telemetry",
    }

    month_file = signals_dir / f"{datetime.now(timezone.utc).strftime('%Y-%m')}.jsonl"
    _rotate_jsonl_if_needed(month_file)
    with open(month_file, "a", encoding="utf-8") as f:
        f.write(json.dumps(record, ensure_ascii=False) + "\n")

    # R5: 提高触发阈值 — 只有重度编辑会话才触发自动演化
    if quality["edit_count"] >= 10 or quality["unique_skills"] >= 3:
        try:
            # Generate proposals from latest telemetry
            subprocess.run(
                [sys.executable or "python",
                 str(repo_root / "skills" / "tooling" / "learning" / "evolve.py"),
                 "auto-evolve"],
                cwd=repo_root, capture_output=True, timeout=25,
            )
        except Exception:
            pass  # Best-effort; never fail the hook

    return 0


if __name__ == "__main__":
    raise SystemExit(main())