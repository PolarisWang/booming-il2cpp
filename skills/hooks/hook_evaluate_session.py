#!/usr/bin/env python3
"""Stop hook: evaluates session quality and generates learning signals.

Reads Claude Code session transcript, computes heuristic quality score,
and writes signals to lifecycle/learning/signals/ when score >= threshold.
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


def find_transcript() -> str | None:
    """Find the most recent Claude Code transcript."""
    # Cross-platform: check common transcript directories
    candidate_dirs = []
    home = Path.home()

    # Linux/macOS
    candidate_dirs.append(home / ".claude" / "claude-code" / "transcripts")
    # Windows
    local_app_data = Path.home() / "AppData" / "Local"
    candidate_dirs.append(local_app_data / "Claude" / "claude-code" / "transcripts")

    for claude_dir in candidate_dirs:
        if claude_dir.exists():
            transcripts = sorted(claude_dir.glob("*.md"), key=lambda p: p.stat().st_mtime, reverse=True)
            if transcripts:
                return str(transcripts[0])

    return None


def estimate_quality(transcript_path: str) -> dict:
    """Heuristic quality scoring of a session transcript."""
    text = Path(transcript_path).read_text(encoding="utf-8", errors="replace")

    tool_call_count = len(re.findall(r"^- .+ \(", text))
    skill_refs = len(re.findall(r"dev-[a-z-]+", text))
    edit_count = len(re.findall(r"(?:Edit|Write)\s+`[^`]+`", text))

    score = 0.0
    details = []

    if edit_count > 5:
        score += 0.35
        details.append(f"edits={edit_count}")
    if skill_refs >= 2:
        score += 0.30
        details.append(f"skill_refs={skill_refs}")
    if tool_call_count > 30:
        score += 0.25
        details.append(f"tool_calls={tool_call_count}")
    elif tool_call_count > 10:
        score += 0.10
        details.append(f"tool_calls={tool_call_count}")

    domain_hint = _detect_domain(text)
    action_hint = _detect_action(text)

    return {
        "score": round(score, 2),
        "tool_call_count": tool_call_count,
        "skill_refs": skill_refs,
        "edit_count": edit_count,
        "domain_hint": domain_hint,
        "action_hint": action_hint,
        "details": "; ".join(details),
    }


def _detect_domain(text: str) -> str:
    domain_keywords = {
        "workflow": ["brainstorm", "roadmap", "writing-plans", "executing-plans", "TDD", "debug"],
        "quality": ["code review", "verification", "trace", "completion"],
        "testing": ["test governance", "foundation-dll", "test generation", "subject"],
        "il2cpp": ["il2cpp", "translation", "emission", "planning", "opcode"],
        "knowledge": ["wiki", "knowledge", "documentation"],
        "skilling": ["skill", "SKILL.md", "manifest"],
    }
    text_lower = text.lower()
    scores = {}
    for domain, keywords in domain_keywords.items():
        scores[domain] = sum(1 for kw in keywords if kw.lower() in text_lower)
    if max(scores.values()) == 0:
        return "engineering"
    return max(scores, key=scores.get)


def _detect_action(text: str) -> str:
    if re.search(r"(?:create|new|add).*(?:skill|SKILL\.md)", text, re.IGNORECASE):
        return "new-skill"
    if re.search(r"(?:update|modify|edit|improve).*(?:skill|SKILL\.md)", text, re.IGNORECASE):
        return "refine"
    if re.search(r"(?:bug|fix|error|fail|crash)", text, re.IGNORECASE):
        return "bugfix"
    return "feature"


def main() -> int:
    repo_root = resolve_repo_root()
    if not repo_root:
        return 0

    transcript = find_transcript()
    if not transcript:
        return 0

    quality = estimate_quality(transcript)
    if quality["score"] < 0.7:
        return 0  # Not a high-quality enough session to record

    signals_dir = repo_root / "skills" / "lifecycle" / "learning" / "signals"
    signals_dir.mkdir(parents=True, exist_ok=True)

    record = {
        "timestamp": datetime.now(timezone.utc).isoformat(),
        "quality_score": quality["score"],
        "domain_hint": quality["domain_hint"],
        "action_hint": quality["action_hint"],
        "metrics": {
            "tool_calls": quality["tool_call_count"],
            "skill_refs": quality["skill_refs"],
            "edits": quality["edit_count"],
        },
        "transcript": transcript,
    }

    month_file = signals_dir / f"{datetime.now().strftime('%Y-%m')}.jsonl"
    with open(month_file, "a", encoding="utf-8") as f:
        f.write(json.dumps(record, ensure_ascii=False) + "\n")

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
