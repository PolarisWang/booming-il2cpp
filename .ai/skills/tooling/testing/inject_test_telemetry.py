#!/usr/bin/env python3
"""Inject test telemetry data to verify the skill self-evolution pipeline end-to-end.

Usage:
  python skills/tooling/testing/inject_test_telemetry.py

This generates synthetic data for 3 evolution scenarios:
  - FIX:     dev-brainstorm (completion_rate=0, sessions>=3)
  - DERIVE:  dev-architecture-first-development (context_switch high, sessions>=10)
  - CAPTURE: novel domain signals (metal-shader-compilation, texture-streaming-pipeline)

After injection, run these commands in order:

  # Step 1: health check
  python -m skills.tooling.learning.health_engine compute --all

  # Step 2: evolution proposals
  python -m skills.tooling.learning.evolve auto-evolve

  # Step 3: review + promote (e.g. FIX proposal)
  python -m skills.tooling.learning.skill_learn evolve-benchmark fix-dev-brainstorm-v1.0.1
  python -m skills.tooling.learning.skill_learn evolve-review fix-dev-brainstorm-v1.0.1
  python -m skills.tooling.learning.skill_learn evolve-promote fix-dev-brainstorm-v1.0.1

  # Step 4: verify
  python -m skills.tooling.learning.evolve status

To clean up test artifacts after verification:
  python skills/tooling/testing/inject_test_telemetry.py --clean
"""

from __future__ import annotations

import json
import os
import shutil
import sys
from datetime import datetime, timedelta, timezone
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parents[3]

TELEMETRY_DIR = REPO_ROOT / "skills" / "lifecycle" / "telemetry"
SIGNALS_DIR = REPO_ROOT / "skills" / "lifecycle" / "learning" / "signals"
PROPOSALS_DIR = REPO_ROOT / "skills" / "lifecycle" / "evolution" / "proposals"
LINEAGE_DIR = REPO_ROOT / "skills" / "lifecycle" / "evolution" / "lineage"
HEALTH_DIR = TELEMETRY_DIR / "health"
PROMOTION_LOG = REPO_ROOT / "skills" / "lifecycle" / "governance" / "promotion-log.md"
PENDING_REPORT = REPO_ROOT / "skills" / "lifecycle" / "evolution" / "pending-report.json"
BRAINSTORM_MANIFEST = REPO_ROOT / "skills" / "library" / "skills" / "dev-brainstorm" / "skill.manifest.json"


def ts(days_ago: int) -> str:
    dt = datetime.now(timezone.utc).replace(hour=9, minute=0, second=0, microsecond=0)
    dt -= timedelta(days=days_ago)
    return dt.isoformat()


def write_jsonl(path: Path, records: list[dict]) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    with path.open("w", encoding="utf-8") as f:
        for r in records:
            f.write(json.dumps(r, ensure_ascii=False) + "\n")
    rel = path.relative_to(REPO_ROOT)
    print(f"  wrote {len(records)} records -> {rel}")


def inject() -> None:
    # ── usage.jsonl ──────────────────────────────────────────────────────────
    # dev-brainstorm: 5 reads (triggers FIX)
    # dev-architecture-first-development: 15 reads across 8 days (triggers DERIVE)
    usage = []
    for i in range(5):
        usage.append({
            "timestamp": ts(i),
            "skill_path": "skills/library/skills/dev-brainstorm/SKILL.md",
            "event": "skill_read",
        })
    for i in range(15):
        usage.append({
            "timestamp": ts(i // 2),
            "skill_path": "skills/library/skills/dev-architecture-first-development/SKILL.md",
            "event": "skill_read",
        })
    write_jsonl(TELEMETRY_DIR / "usage.jsonl", usage)

    # ── tool_outcomes.jsonl ──────────────────────────────────────────────────
    # dev-brainstorm: 15 calls, all successful
    # dev-architecture: 40 calls, 38 successful (2 failures)
    tools = []
    for i in range(15):
        tools.append({
            "timestamp": ts(i // 3),
            "tool_name": "Read" if i % 2 == 0 else "Edit",
            "success": True,
            "error_type": None,
            "skill_path": "skills/library/skills/dev-brainstorm/SKILL.md",
            "event": "tool_outcome",
        })
    for i in range(40):
        tools.append({
            "timestamp": ts(i // 5),
            "tool_name": ["Read", "Edit", "Bash"][i % 3],
            "success": i < 38,
            "error_type": None if i < 38 else "TimeoutError",
            "skill_path": "skills/library/skills/dev-architecture-first-development/SKILL.md",
            "event": "tool_outcome",
        })
    write_jsonl(TELEMETRY_DIR / "tool_outcomes.jsonl", tools)

    # ── session_outcomes.jsonl ───────────────────────────────────────────────
    # dev-brainstorm: 5 sessions, 0 completed (completion_rate=0 -> triggers FIX)
    # dev-architecture: 12 sessions, 10 completed
    sessions = []
    for i in range(5):
        sessions.append({
            "timestamp": ts(i),
            "skill_path": "skills/library/skills/dev-brainstorm/SKILL.md",
            "completed": False,
            "signals": {"has_recent_commit": False, "has_skill_edit": False},
            "event": "session_outcome",
        })
    for i in range(12):
        sessions.append({
            "timestamp": ts(i // 2),
            "skill_path": "skills/library/skills/dev-architecture-first-development/SKILL.md",
            "completed": i < 10,
            "signals": {"has_recent_commit": i < 10, "has_skill_edit": False},
            "event": "session_outcome",
        })
    write_jsonl(TELEMETRY_DIR / "session_outcomes.jsonl", sessions)

    # ── learning/signals/{YYYY-MM}.jsonl ─────────────────────────────────────
    # Novel domains not matching existing skills -> triggers CAPTURE
    signals = [
        {
            "timestamp": ts(0),
            "quality_score": 0.85,
            "domain_hint": "metal-shader-compilation",
            "action_hint": "new-skill",
            "metrics": {"tool_calls": 45, "skill_refs": 0, "edits": 12},
            "transcript": "/tmp/fake-transcript-metal-shader.md",
        },
        {
            "timestamp": ts(1),
            "quality_score": 0.92,
            "domain_hint": "texture-streaming-pipeline",
            "action_hint": "feature",
            "metrics": {"tool_calls": 60, "skill_refs": 0, "edits": 18},
            "transcript": "/tmp/fake-transcript-texture.md",
        },
    ]
    write_jsonl(SIGNALS_DIR / f"{datetime.now(timezone.utc).strftime('%Y-%m')}.jsonl", signals)

    print()
    print("Test telemetry injected. Run health check + evolution pipeline:")
    print()
    print("  python -m skills.tooling.learning.health_engine compute --all")
    print("  python -m skills.tooling.learning.evolve auto-evolve")
    print()


def clean() -> None:
    """Remove all test artifacts."""
    # Telemetry files
    for f in ["usage.jsonl", "tool_outcomes.jsonl", "session_outcomes.jsonl"]:
        p = TELEMETRY_DIR / f
        if p.exists():
            p.unlink()
            print(f"  removed {p.relative_to(REPO_ROOT)}")
    # Health snapshots
    if HEALTH_DIR.exists():
        for f in HEALTH_DIR.glob("*.json"):
            f.unlink()
        print(f"  cleared {HEALTH_DIR.relative_to(REPO_ROOT)}/")
    # Signal files
    if SIGNALS_DIR.exists():
        for f in SIGNALS_DIR.glob("*.jsonl"):
            f.unlink()
        print(f"  cleared {SIGNALS_DIR.relative_to(REPO_ROOT)}/")
    # Evolution proposals
    if PROPOSALS_DIR.exists():
        for d in PROPOSALS_DIR.iterdir():
            if d.is_dir():
                shutil.rmtree(d)
        print(f"  cleared {PROPOSALS_DIR.relative_to(REPO_ROOT)}/")
    # Lineage records
    if LINEAGE_DIR.exists():
        for f in LINEAGE_DIR.glob("*.jsonl"):
            f.unlink()
        print(f"  cleared {LINEAGE_DIR.relative_to(REPO_ROOT)}/")
    # Pending report
    if PENDING_REPORT.exists():
        PENDING_REPORT.unlink()
        print(f"  removed {PENDING_REPORT.relative_to(REPO_ROOT)}")
    # dev-brainstorm manifest revert
    if BRAINSTORM_MANIFEST.exists():
        import json as j
        with open(BRAINSTORM_MANIFEST, encoding="utf-8") as f:
            m = j.load(f)
        if m.get("version") != "1.0.0":
            m["version"] = "1.0.0"
            if m.get("version_history") and m["version_history"][-1].get("type") == "fix":
                m["version_history"].pop()
            with open(BRAINSTORM_MANIFEST, "w", encoding="utf-8") as f:
                j.dump(m, f, indent=2, ensure_ascii=False)
                f.write("\n")
            print("  reverted dev-brainstorm manifest to 1.0.0")
    # Promoted skills
    for promoted in [
        "auto-metal-shader-compilation-new-skill",
        "dev-architecture-first-development-planning",
    ]:
        p = REPO_ROOT / "skills" / "library" / "skills" / promoted
        if p.exists():
            shutil.rmtree(p)
            print(f"  removed promoted skill: {promoted}/")
    # Promotion log cleanup (remove lines added by test)
    if PROMOTION_LOG.exists():
        content = PROMOTION_LOG.read_text(encoding="utf-8")
        lines = content.splitlines()
        # Keep only lines up to the ghost-skill annotation (the baseline)
        cleaned = []
        for line in lines:
            if "2026-05-12" in line and ("FIX: 1.0.0 -> 1.0.1" in line or "CAPTURE" in line or "DERIVE" in line):
                continue
            cleaned.append(line)
        PROMOTION_LOG.write_text("\n".join(cleaned) + "\n", encoding="utf-8")
        print("  cleaned promotion-log.md of test entries")

    print()
    print("Test artifacts cleaned.")


def main() -> int:
    if len(sys.argv) > 1 and sys.argv[1] == "--clean":
        clean()
    else:
        inject()
    return 0


if __name__ == "__main__":
    raise SystemExit(main())