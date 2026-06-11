#!/usr/bin/env python3
"""Evolution Trigger Engine: FIX / DERIVED / CAPTURED skill evolution.

Reads health metrics from health_engine.py and generates evolution proposals.

Subcommands:
  auto-evolve                 Full evolution pass (FIX + DERIVED + CAPTURED)
  auto-evolve-fix --skill     FIX one skill
  auto-evolve-derive --skill  DERIVE from one skill
  auto-evolve-capture         CAPTURE new skills from signals
  propose --dry-run           Enumerate what would happen (no actions)
  status                      Show pending evolution proposals
"""

from __future__ import annotations

import argparse
import json
import re
import shutil
import subprocess
import sys
from collections import defaultdict
from datetime import datetime, timezone
from pathlib import Path
from typing import Any


# ── Thresholds (match health_engine.py) ─────────────────────────────────────
COMPLETION_RATE_LOW = 0.6
APPLIED_RATE_MIN_FOR_FIX = 3
CONTEXT_SWITCH_HIGH = 0.5
APPLIED_RATE_MIN_FOR_DERIVE = 10
QUALITY_CAPTURE_THRESHOLD = 0.8


def now_iso() -> str:
    return datetime.now(timezone.utc).astimezone().replace(microsecond=0).isoformat()


def resolve_repo_root() -> Path | None:
    try:
        script_dir = Path(__file__).resolve().parent
        for parent in [script_dir, script_dir.parent, script_dir.parent.parent]:
            git_root = subprocess.run(
                ["git", "-C", str(parent), "rev-parse", "--show-toplevel"],
                capture_output=True, text=True, timeout=10,
            )
            root = git_root.stdout.strip()
            if root:
                return Path(root).resolve()
        return None
    except Exception:
        return None


def load_json(path: Path, default: Any | None = None) -> Any:
    if not path.exists():
        if default is not None:
            return default
        raise FileNotFoundError(str(path))
    return json.loads(path.read_text(encoding="utf-8"))


def write_json(path: Path, payload: Any) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(payload, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")


def append_jsonl(path: Path, payload: dict) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    with path.open("a", encoding="utf-8") as f:
        f.write(json.dumps(payload, ensure_ascii=False) + "\n")


def load_jsonl(path: Path) -> list[dict]:
    if not path.exists() or path.stat().st_size == 0:
        return []
    return [json.loads(line) for line in path.read_text(encoding="utf-8").strip().splitlines() if line.strip()]


def load_failure_patterns(repo_root: Path, skill_name: str) -> list[dict]:
    """Load failure patterns for a specific skill from telemetry."""
    fp_path = repo_root / "skills" / "lifecycle" / "telemetry" / "failure-patterns.jsonl"
    all_failures = load_jsonl(fp_path)
    return [f for f in all_failures if skill_name in (f.get("skill_path") or "")]


def load_success_patterns(repo_root: Path, skill_name: str) -> list[dict]:
    """Load success patterns for a specific skill from telemetry."""
    sp_path = repo_root / "skills" / "lifecycle" / "telemetry" / "success-patterns.jsonl"
    all_success = load_jsonl(sp_path)
    return [s for s in all_success if skill_name in (s.get("skill_path") or "")]


def summarize_failures(failures: list[dict]) -> str:
    """Generate a readable summary of failure patterns."""
    if not failures:
        return ""
    classes: dict[str, int] = {}
    for f in failures:
        cls = f.get("error_class", "unknown")
        classes[cls] = classes.get(cls, 0) + 1
    total = len(failures)
    summary = f"该技能的历史使用中记录了 {total} 次工具调用失败:\n"
    for cls, count in sorted(classes.items(), key=lambda x: -x[1]):
        pct = count / total * 100
        examples = [f.get("error_preview", "") for f in failures if f.get("error_class") == cls][:3]
        summary += f"- {cls} ({count}次, {pct:.0f}%): "
        if examples:
            summary += f"例如 \"{examples[0]}\"\n"
        else:
            summary += "\n"
    return summary


def summarize_successes(successes: list[dict]) -> str:
    """Generate a readable summary of success patterns."""
    if not successes:
        return ""
    pattern_types: dict[str, int] = {}
    for s in successes:
        pt = s.get("pattern_type", "mixed")
        pattern_types[pt] = pattern_types.get(pt, 0) + 1
    dominant = max(pattern_types, key=pattern_types.get) if pattern_types else "mixed"
    desc = successes[-1].get("pattern_description", "")

    summary = (
        f"该技能的成功工作模式以「{_pattern_label(dominant)}」为主:\n"
        f"- {desc}\n"
        f"- 平均工具调用成功率: {successes[-1].get('success_rate', 1.0)*100:.0f}%\n"
    )
    # Extract tool sequence hints
    last = successes[-1]
    tool_seq = last.get("tool_sequence_summary", {})
    if tool_seq:
        top_tools = sorted(tool_seq.items(), key=lambda x: -x[1])[:5]
        tools_str = ", ".join(f"{t}({c}次)" for t, c in top_tools)
        summary += f"- 常用工具序列: {tools_str}\n"
    return summary


def _pattern_label(pt: str) -> str:
    labels = {
        "investigation": "查探型",
        "implementation": "实现型",
        "execution": "执行型",
        "mixed": "混合型",
    }
    return labels.get(pt, pt)


def parse_frontmatter(skill_path: Path) -> dict[str, str]:
    lines = skill_path.read_text(encoding="utf-8").splitlines()
    if len(lines) < 3 or lines[0].lstrip("﻿").strip() != "---":
        return {}
    result: dict[str, str] = {}
    for line in lines[1:]:
        if line.strip() == "---":
            break
        if ":" not in line:
            continue
        key, value = line.split(":", 1)
        result[key.strip()] = value.strip()
    return result


def find_latest_health_snapshot(repo_root: Path) -> dict | None:
    health_dir = repo_root / "skills" / "lifecycle" / "telemetry" / "health"
    if not health_dir.exists():
        return None
    snapshots = sorted(health_dir.glob("health-snapshot-*.json"))
    if not snapshots:
        return None
    return load_json(snapshots[-1])


def find_signals_not_matching_skills(repo_root: Path) -> list[dict]:
    """Find high-quality signals that don't match any existing skill."""
    signals_dir = repo_root / "skills" / "lifecycle" / "learning" / "signals"
    if not signals_dir.exists():
        return []

    library_dir = repo_root / "skills" / "library" / "skills"

    # Load all known skill summaries
    known_summaries: list[str] = []
    if library_dir.exists():
        for d in sorted(library_dir.iterdir()):
            if not d.is_dir():
                continue
            mf = d / "skill.manifest.json"
            if mf.exists():
                m = load_json(mf, {})
                known_summaries.append(m.get("summary", ""))

    # Find high-quality signals
    candidates = []
    for f in sorted(signals_dir.glob("*.jsonl")):
        for s in load_jsonl(f):
            score = s.get("quality_score", 0) or 0
            action = s.get("action_hint", "")
            if score >= QUALITY_CAPTURE_THRESHOLD and action in ("new-skill", "feature"):
                # Check overlap with existing skills
                summary = s.get("domain_hint", "")
                has_overlap = any(
                    len(set(summary.lower().split()) & set(ks.lower().split())) > 3
                    for ks in known_summaries
                )
                if not has_overlap:
                    candidates.append(s)
    return candidates


def get_next_version(current: str, evo_type: str) -> str:
    """Calculate next version based on evolution type.
    FIX = patch bump (1.0.0 -> 1.0.1)
    CAPTURED pre-release = 0.1.0, promoted to 1.0.0
    """
    try:
        parts = current.split(".")
        major, minor, patch = int(parts[0]), int(parts[1]), int(parts[2])
    except (ValueError, IndexError):
        return "1.0.0"

    if evo_type == "fix":
        return f"{major}.{minor}.{patch + 1}"
    elif evo_type == "capture":
        if major == 0:
            return "0.1.0"
        return f"{major + 1}.0.0"
    return current


# ── Proposal generation ─────────────────────────────────────────────────────

def generate_fix_proposal(
    repo_root: Path, skill_name: str, health_data: dict
) -> dict | None:
    """Generate a FIX evolution proposal for a skill."""
    proposals_dir = repo_root / "skills" / "lifecycle" / "evolution" / "proposals"
    lineage_dir = repo_root / "skills" / "lifecycle" / "evolution" / "lineage"
    library_dir = repo_root / "skills" / "library" / "skills" / skill_name

    if not library_dir.exists():
        print(f"  [FIX] Skill directory not found: {skill_name}")
        return None

    # Read current manifest
    manifest_file = library_dir / "skill.manifest.json"
    if not manifest_file.exists():
        print(f"  [FIX] No manifest for: {skill_name}")
        return None

    manifest = load_json(manifest_file)
    current_version = manifest.get("version", "1.0.0")
    next_version = get_next_version(current_version, "fix")

    proposal_id = f"fix-{skill_name}-v{next_version}"
    proposal_dir = proposals_dir / proposal_id

    if proposal_dir.exists():
        print(f"  [FIX] Proposal already exists: {proposal_id}")
        return None

    # Load failure and success patterns for this skill
    failures = load_failure_patterns(repo_root, skill_name)
    successes = load_success_patterns(repo_root, skill_name)

    # Copy current skill as baseline for evolution
    shutil.copytree(library_dir, proposal_dir)

    # Write proposal metadata
    metrics = health_data.get("metrics", {})
    raw = health_data.get("raw_counts", {})
    proposal = {
        "proposal_id": proposal_id,
        "type": "fix",
        "skill": skill_name,
        "version_from": current_version,
        "version_to": next_version,
        "timestamp": now_iso(),
        "reason": f"completion_rate={metrics.get('completion_rate', 0):.2f} "
                  f"(threshold: {COMPLETION_RATE_LOW}), "
                  f"sessions={raw.get('sessions', 0)}",
        "health_evidence": {
            "completion_rate": metrics.get("completion_rate", 0),
            "tool_success_rate": metrics.get("tool_success_rate", 1.0),
            "fallback_rate": metrics.get("fallback_rate", 0),
            "applied_rate": metrics.get("applied_rate", 0),
        },
        "failure_patterns": {
            "total_failures": len(failures),
            "error_classes": dict(sorted(
                {f.get("error_class", "unknown"): sum(1 for x in failures if x.get("error_class") == f.get("error_class"))
                 for f in failures}.items(),
                key=lambda x: -x[1]
            )) if failures else {},
        },
        "success_patterns": {
            "total_successes": len(successes),
            "dominant_pattern": successes[-1].get("pattern_type", "mixed") if successes else None,
        },
        "status": "proposed",
    }
    write_json(proposal_dir / "evolution-proposal.json", proposal)

    # Update manifest in proposal with new version
    manifest["version"] = next_version
    if "version_history" not in manifest:
        manifest["version_history"] = [{"version": current_version, "date": now_iso()[:10], "type": "initial"}]
    manifest["version_history"].append({
        "version": next_version,
        "date": now_iso()[:10],
        "type": "fix",
        "reason": proposal["reason"],
    })
    write_json(proposal_dir / "skill.manifest.json", manifest)

    # Update SKILL.md with FIX annotation AND failure/success insights
    skill_file = proposal_dir / "SKILL.md"
    if skill_file.exists():
        content = skill_file.read_text(encoding="utf-8")

        # Build experience-based sections
        insight_sections = []

        # Failure patterns section
        if failures:
            fail_summary = summarize_failures(failures)
            insight_sections.append(
                "## 已知错误模式 (From Failure Pattern Library)\n\n"
                f"{fail_summary}\n"
            )

        # Success patterns section
        if successes:
            succ_summary = summarize_successes(successes)
            insight_sections.append(
                "## 推荐工作模式 (From Success Pattern Library)\n\n"
                f"{succ_summary}\n"
            )

        # The evolution annotation always goes first
        annotation = (
            f"\n\n---\n<!-- FIX evolution: {current_version} -> {next_version} -->\n"
            f"**Evolution Note**: This skill was auto-evolved (FIX) on {now_iso()[:10]}. "
            f"Reason: {proposal['reason']}. "
        )
        if not failures and not successes:
            annotation += "Review and update the content below to address the identified issues.\n"
        else:
            annotation += "Experience patterns have been appended below for review.\n"

        # Append annotation and insight sections
        new_content = content + annotation
        for section in insight_sections:
            new_content += "\n" + section
        skill_file.write_text(new_content, encoding="utf-8")

    # Write lineage record
    lineage = {
        "skill": skill_name,
        "version_from": current_version,
        "version_to": next_version,
        "type": "fix",
        "timestamp": now_iso(),
        "proposal_id": proposal_id,
        "reason": proposal["reason"],
    }
    lineage_file = lineage_dir / f"{skill_name}.jsonl"
    append_jsonl(lineage_file, lineage)

    print(f"  [FIX] Proposal created: {proposal_id} ({current_version} -> {next_version})")
    return proposal


def generate_derive_proposal(
    repo_root: Path, skill_name: str, health_data: dict
) -> list[dict]:
    """Generate DERIVED evolution proposals for a skill."""
    proposals_dir = repo_root / "skills" / "lifecycle" / "evolution" / "proposals"
    lineage_dir = repo_root / "skills" / "lifecycle" / "evolution" / "lineage"
    library_dir = repo_root / "skills" / "library" / "skills" / skill_name

    if not library_dir.exists():
        return []

    manifest_file = library_dir / "skill.manifest.json"
    if not manifest_file.exists():
        return []

    manifest = load_json(manifest_file)
    summary = manifest.get("summary", "")

    # Generate variants based on context-switching patterns
    # Heuristic: generate 1-2 derived skills for common split patterns
    variants = []
    split_patterns = [
        ("planning", f"{skill_name}-planning", "专注于规划阶段的子技能"),
        ("execution", f"{skill_name}-execution", "专注于执行阶段的子技能"),
    ]

    proposals = []
    for variant_suffix, variant_name, variant_desc in split_patterns:
        proposal_id = f"derive-{skill_name}-{variant_suffix}-v1"
        proposal_dir = proposals_dir / proposal_id

        if proposal_dir.exists():
            continue

        proposal_dir.mkdir(parents=True, exist_ok=True)

        # Create derived SKILL.md
        derived_skill_md = (
            f"---\nname: {variant_name}\ndescription: {variant_desc}\n---\n\n"
            f"# {variant_name}\n\n"
            f"## Overview\n\n"
            f"Derived from [{skill_name}](../{skill_name}/SKILL.md) to specialize in {variant_suffix}.\n\n"
            f"## Usage\n\n"
            f"1. TODO: Define specific trigger signals for this variant.\n"
            f"2. TODO: Define variant-specific workflow.\n"
            f"3. TODO: Define completion criteria.\n\n"
            f"## Parent Skill\n\n"
            f"- Derived from: `{skill_name}` (v{manifest.get('version', '1.0.0')})\n"
            f"- Evolution date: {now_iso()[:10]}\n"
            f"- Reason: Context-switching ratio exceeded threshold\n\n"
            f"## Red Flags\n\n"
            f"- Do not use when the parent skill's full scope is needed.\n"
            f"- Ensure this variant covers all {variant_suffix}-specific edge cases.\n"
        )
        write_json(proposal_dir / "SKILL.md", {"_note": "Write file manually", "content": derived_skill_md})
        # Actually write the SKILL.md
        (proposal_dir / "SKILL.md").write_text(derived_skill_md, encoding="utf-8")

        # Create derived manifest
        derived_manifest = {
            "kind": "project",
            "domain": manifest.get("domain", "workflow"),
            "name": variant_name,
            "summary": f"Derived from {skill_name} — {variant_desc}",
            "trigger_signals": [f"TODO: Define trigger signals for {variant_name}"],
            "owner": "00-skills-maintainers",
            "status": "draft",
            "version": "1.0.0",
            "version_history": [
                {"version": "1.0.0", "date": now_iso()[:10], "type": "derived",
                 "parent": skill_name, "description": f"Derived from {skill_name}"}
            ],
            "evolution_lineage": {
                "parent": skill_name,
                "derived_from": skill_name,
                "derived_to": [],
                "fixes": [],
                "captured_from": None,
            },
            "startup_visible": False,
            "token_budget": "small",
        }
        write_json(proposal_dir / "skill.manifest.json", derived_manifest)

        # Proposal metadata
        proposal = {
            "proposal_id": proposal_id,
            "type": "derive",
            "skill": variant_name,
            "parent_skill": skill_name,
            "version": "1.0.0",
            "timestamp": now_iso(),
            "reason": f"Context-switching ratio triggered, derived variant for {variant_suffix}",
            "status": "proposed",
        }
        write_json(proposal_dir / "evolution-proposal.json", proposal)

        # Lineage
        lineage = {
            "skill": variant_name,
            "version": "1.0.0",
            "type": "derived",
            "parent_skill": skill_name,
            "timestamp": now_iso(),
            "proposal_id": proposal_id,
            "reason": proposal["reason"],
        }
        lineage_file = lineage_dir / f"{variant_name}.jsonl"
        append_jsonl(lineage_file, lineage)

        print(f"  [DERIVE] Proposal created: {proposal_id}")
        proposals.append(proposal)

    return proposals


def generate_capture_proposal(
    repo_root: Path, signal: dict
) -> dict | None:
    """Generate a CAPTURED evolution proposal from a high-quality signal."""
    proposals_dir = repo_root / "skills" / "lifecycle" / "evolution" / "proposals"
    lineage_dir = repo_root / "skills" / "lifecycle" / "evolution" / "lineage"

    domain_hint = signal.get("domain_hint", "workflow")
    action_hint = signal.get("action_hint", "feature")
    timestamp = signal.get("timestamp", now_iso())[:10].replace("-", "")

    # Create a unique ID
    auto_id = f"capture-auto-{timestamp}-{domain_hint}"
    proposal_id = f"capture-{auto_id}-v1"
    proposal_dir = proposals_dir / proposal_id

    if proposal_dir.exists():
        return None

    proposal_dir.mkdir(parents=True, exist_ok=True)

    skill_name = f"auto-{domain_hint}-{action_hint}"
    captured_skill_md = (
        f"---\nname: {skill_name}\ndescription: 从高质量会话捕获的自动生成 skill\n---\n\n"
        f"# {skill_name}\n\n"
        f"## Overview\n\n"
        f"Captured from a high-quality session (score: {signal.get('quality_score', 0)}).\n"
        f"Domain: {domain_hint}, Action: {action_hint}\n\n"
        f"## Source Signal\n\n"
        f"- Timestamp: {signal.get('timestamp', 'unknown')}\n"
        f"- Domain: {domain_hint}\n"
        f"- Action: {action_hint}\n"
        f"- Quality: {signal.get('quality_score', 0)}\n\n"
        f"## Usage\n\n"
        f"1. TODO: Review and refine trigger signals.\n"
        f"2. TODO: Define step-by-step workflow.\n"
        f"3. TODO: Define completion criteria.\n\n"
        f"## Red Flags\n\n"
        f"- This is a CAPTURED skill — review thoroughly before promoting to formal.\n"
        f"- Ensure it does not overlap with existing skills.\n"
        f"- Version 0.1.0 until promoted to formal library.\n"
    )
    (proposal_dir / "SKILL.md").write_text(captured_skill_md, encoding="utf-8")

    # Manifest (pre-release version)
    captured_manifest = {
        "kind": "project",
        "domain": domain_hint,
        "name": skill_name,
        "summary": f"Captured from high-quality session ({domain_hint}/{action_hint})",
        "trigger_signals": [f"TODO: Define trigger signals for {skill_name}"],
        "owner": "00-skills-maintainers",
        "status": "draft",
        "version": "0.1.0",
        "version_history": [
            {"version": "0.1.0", "date": now_iso()[:10], "type": "captured",
             "description": f"Captured from {domain_hint} session"}
        ],
        "evolution_lineage": {
            "parent": None,
            "derived_from": None,
            "derived_to": [],
            "fixes": [],
            "captured_from": signal.get("timestamp", "unknown"),
        },
        "startup_visible": False,
        "token_budget": "small",
    }
    write_json(proposal_dir / "skill.manifest.json", captured_manifest)

    proposal = {
        "proposal_id": proposal_id,
        "type": "capture",
        "skill": skill_name,
        "version": "0.1.0",
        "timestamp": now_iso(),
        "reason": f"High-quality signal ({signal.get('quality_score', 0)}) in domain {domain_hint}",
        "signal_evidence": signal,
        "status": "proposed",
    }
    write_json(proposal_dir / "evolution-proposal.json", proposal)

    # Lineage
    lineage = {
        "skill": skill_name,
        "version": "0.1.0",
        "type": "captured",
        "timestamp": now_iso(),
        "proposal_id": proposal_id,
        "reason": proposal["reason"],
    }
    lineage_file = lineage_dir / f"{skill_name}.jsonl"
    append_jsonl(lineage_file, lineage)

    print(f"  [CAPTURE] Proposal created: {proposal_id} (v0.1.0)")
    return proposal


# ── Main commands ───────────────────────────────────────────────────────────

def cmd_propose_dry_run(repo_root: Path) -> int:
    """Dry-run: enumerate what evolution would happen without acting."""
    snapshot = find_latest_health_snapshot(repo_root)
    if not snapshot:
        print("[evolve] No health snapshot found. Run 'health_engine.py compute --all' first.")
        return 0

    skills = snapshot.get("skills", {})
    print("=== Evolution Proposal (DRY RUN) ===\n")

    fix_candidates = []
    derive_candidates = []
    for name, data in sorted(skills.items()):
        m = data.get("metrics", {})
        raw = data.get("raw_counts", {})

        if m.get("completion_rate", 1.0) < COMPLETION_RATE_LOW and raw.get("sessions", 0) >= APPLIED_RATE_MIN_FOR_FIX:
            fix_candidates.append((name, data))
        if m.get("context_switching_ratio", 0) > CONTEXT_SWITCH_HIGH and raw.get("sessions", 0) >= APPLIED_RATE_MIN_FOR_DERIVE:
            derive_candidates.append((name, data))

    signals = find_signals_not_matching_skills(repo_root)

    print(f"FIX candidates ({len(fix_candidates)}):")
    for name, data in fix_candidates:
        m = data.get("metrics", {})
        print(f"  - {name}: completion={m.get('completion_rate', 0):.2f}, sessions={data.get('raw_counts', {}).get('sessions', 0)}")

    print(f"\nDERIVE candidates ({len(derive_candidates)}):")
    for name, data in derive_candidates:
        m = data.get("metrics", {})
        print(f"  - {name}: switch_ratio={m.get('context_switching_ratio', 0):.2f}, sessions={data.get('raw_counts', {}).get('sessions', 0)}")

    print(f"\nCAPTURE candidates ({len(signals)}):")
    for s in signals:
        print(f"  - domain={s.get('domain_hint', '?')}, action={s.get('action_hint', '?')}, score={s.get('quality_score', 0)}")

    # Write pending report
    report = {
        "timestamp": now_iso(),
        "fix_candidates": len(fix_candidates),
        "derive_candidates": len(derive_candidates),
        "capture_candidates": len(signals),
        "details": {
            "fix": [{"skill": n} for n, _ in fix_candidates],
            "derive": [{"skill": n} for n, _ in derive_candidates],
            "capture": len(signals),
        }
    }
    evolution_dir = repo_root / "skills" / "lifecycle" / "evolution"
    evolution_dir.mkdir(parents=True, exist_ok=True)
    write_json(evolution_dir / "pending-report.json", report)

    print(f"\nPending report written.")
    return 0


def cmd_auto_evolve(repo_root: Path, dry_run: bool = False) -> int:
    """Execute full evolution pass."""
    if dry_run:
        return cmd_propose_dry_run(repo_root)

    snapshot = find_latest_health_snapshot(repo_root)
    if not snapshot:
        print("[evolve] No health snapshot found. Run compute first.")
        return 0

    skills = snapshot.get("skills", {})
    results = {"fix": [], "derive": [], "capture": []}

    print("=== Running Auto-Evolution ===\n")

    # FIX
    print("--- FIX Pass ---")
    for name, data in sorted(skills.items()):
        m = data.get("metrics", {})
        raw = data.get("raw_counts", {})
        if m.get("completion_rate", 1.0) < COMPLETION_RATE_LOW and raw.get("sessions", 0) >= APPLIED_RATE_MIN_FOR_FIX:
            prop = generate_fix_proposal(repo_root, name, data)
            if prop:
                results["fix"].append(prop)

    # DERIVE
    print("--- DERIVE Pass ---")
    for name, data in sorted(skills.items()):
        m = data.get("metrics", {})
        raw = data.get("raw_counts", {})
        if m.get("context_switching_ratio", 0) > CONTEXT_SWITCH_HIGH and raw.get("sessions", 0) >= APPLIED_RATE_MIN_FOR_DERIVE:
            props = generate_derive_proposal(repo_root, name, data)
            results["derive"].extend(props)

    # CAPTURE
    print("--- CAPTURE Pass ---")
    capture_signals = find_signals_not_matching_skills(repo_root)
    for s in capture_signals:
        prop = generate_capture_proposal(repo_root, s)
        if prop:
            results["capture"].append(prop)

    # Summary
    print(f"\n=== Evolution Summary ===")
    print(f"  FIX proposals:     {len(results['fix'])}")
    print(f"  DERIVE proposals:  {len(results['derive'])}")
    print(f"  CAPTURE proposals: {len(results['capture'])}")
    print(f"  Total:             {sum(len(v) for v in results.values())}")

    return 0


def cmd_auto_evolve_fix(repo_root: Path, skill_name: str) -> int:
    """FIX a specific skill."""
    snapshot = find_latest_health_snapshot(repo_root)
    if not snapshot:
        print("[evolve] No health snapshot found.")
        return 0

    skill_data = snapshot.get("skills", {}).get(skill_name)
    if not skill_data:
        print(f"[evolve] No health data for '{skill_name}'. Run compute first.")
        return 0

    print(f"--- FIX: {skill_name} ---")
    prop = generate_fix_proposal(repo_root, skill_name, skill_data)
    if prop:
        print(f"  Proposal: {prop['proposal_id']} ({prop['version_from']} -> {prop['version_to']})")
    return 0


def cmd_auto_evolve_derive(repo_root: Path, skill_name: str) -> int:
    """DERIVE from a specific skill."""
    snapshot = find_latest_health_snapshot(repo_root)
    if not snapshot:
        print("[evolve] No health snapshot found.")
        return 0

    skill_data = snapshot.get("skills", {}).get(skill_name)
    if not skill_data:
        print(f"[evolve] No health data for '{skill_name}'.")
        return 0

    print(f"--- DERIVE: {skill_name} ---")
    props = generate_derive_proposal(repo_root, skill_name, skill_data)
    print(f"  Proposals generated: {len(props)}")
    return 0


def cmd_auto_evolve_capture(repo_root: Path) -> int:
    """CAPTURE new skills from signals."""
    signals = find_signals_not_matching_skills(repo_root)
    if not signals:
        print("[evolve] No capture candidates found.")
        return 0

    print(f"--- CAPTURE ({len(signals)} candidates) ---")
    count = 0
    for s in signals:
        prop = generate_capture_proposal(repo_root, s)
        if prop:
            count += 1

    print(f"  Proposals generated: {count}")
    return 0


def cmd_status(repo_root: Path) -> int:
    """Show pending evolution proposals."""
    proposals_dir = repo_root / "skills" / "lifecycle" / "evolution" / "proposals"
    if not proposals_dir.exists():
        print("[evolve] No evolution proposals directory.")
        return 0

    proposals = sorted(proposals_dir.iterdir()) if proposals_dir.exists() else []
    if not proposals:
        print("[evolve] No pending evolution proposals.")
        return 0

    print(f"=== Pending Proposals ({len(proposals)}) ===\n")
    for prop_dir in proposals:
        prop_file = prop_dir / "evolution-proposal.json"
        if prop_file.exists():
            prop = load_json(prop_file)
            print(f"  [{prop.get('type', '?').upper():>7}] {prop.get('proposal_id', prop_dir.name)}")
            print(f"         skill: {prop.get('skill', '?')}, version: {prop.get('version_from', prop.get('version', '?'))} -> {prop.get('version_to', prop.get('version', '?'))}")
            print(f"         status: {prop.get('status', '?')}, reason: {prop.get('reason', '?')[:80]}")
            print()
    return 0


# ── CLI ─────────────────────────────────────────────────────────────────────

def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description="Skill Evolution Trigger Engine")
    sub = parser.add_subparsers(dest="command", required=True)

    # auto-evolve
    p = sub.add_parser("auto-evolve", help="Full evolution pass")
    p.add_argument("--dry-run", action="store_true", help="Only enumerate, no actions")

    # auto-evolve-fix
    p = sub.add_parser("auto-evolve-fix", help="FIX a specific skill")
    p.add_argument("--skill", required=True, help="Skill name to fix")

    # auto-evolve-derive
    p = sub.add_parser("auto-evolve-derive", help="DERIVE from a specific skill")
    p.add_argument("--skill", required=True, help="Skill name to derive from")

    # auto-evolve-capture
    sub.add_parser("auto-evolve-capture", help="CAPTURE new skills from signals")

    # propose
    sub.add_parser("propose", help="Dry-run enumeration of evolution candidates")

    # status
    sub.add_parser("status", help="Show pending evolution proposals")

    return parser


def main() -> int:
    parser = build_parser()
    args = parser.parse_args()

    repo_root = resolve_repo_root()
    if not repo_root:
        print("Error: not in a git repository", file=sys.stderr)
        return 1

    handlers = {
        "auto-evolve": lambda: cmd_auto_evolve(repo_root, args.dry_run),
        "auto-evolve-fix": lambda: cmd_auto_evolve_fix(repo_root, args.skill),
        "auto-evolve-derive": lambda: cmd_auto_evolve_derive(repo_root, args.skill),
        "auto-evolve-capture": lambda: cmd_auto_evolve_capture(repo_root),
        "propose": lambda: cmd_propose_dry_run(repo_root),
        "status": lambda: cmd_status(repo_root),
    }

    handler = handlers.get(args.command)
    if not handler:
        parser.print_help()
        return 1

    try:
        return int(handler())
    except Exception as e:
        print(f"[evolve] Error: {e}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    raise SystemExit(main())
