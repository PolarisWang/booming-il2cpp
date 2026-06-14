#!/usr/bin/env python3
"""Skill self-learning pipeline: collect -> cluster -> draft -> benchmark -> review -> promote -> retire.

Usage:
  python skill_learn.py collect <signal-text> [--domain-hint <domain>] [--action-hint <action>]
  python skill_learn.py cluster
  python skill_learn.py draft <candidate-id>
  python skill_learn.py benchmark <candidate-id>
  python skill_learn.py review <candidate-id>
  python skill_learn.py promote <candidate-id> [--dry-run]
  python skill_learn.py promote-manual <scaffold-id> [--dry-run]
  python skill_learn.py observe
  python skill_learn.py retire <skill-name> [--dry-run] [--reason <reason>]
  python skill_learn.py auto-collect [--min-uses 3]
  python skill_learn.py auto-assess
  python skill_learn.py auto-report [--output json|md|both]
  python skill_learn.py auto-cleanup [--lookback-days 90] [--dry-run] [--health-engine/--no-health-engine]
  python skill_learn.py evolve-benchmark <proposal-id>
  python skill_learn.py evolve-review <proposal-id>
  python skill_learn.py evolve-promote <proposal-id> [--dry-run]
  python skill_learn.py evolve-history [--skill <name>]
"""

from __future__ import annotations

import argparse
import json
import math
import os
import re
import shutil
import stat
import subprocess
import sys
from collections import Counter, defaultdict
from datetime import datetime, timedelta, timezone
from pathlib import Path
from typing import Any


def now_iso() -> str:
    return datetime.now(timezone.utc).astimezone().replace(microsecond=0).isoformat()


def slugify(value: str) -> str:
    lowered = value.strip().lower()
    lowered = re.sub(r"[^a-z0-9一-鿿]+", "-", lowered)
    lowered = re.sub(r"-{2,}", "-", lowered).strip("-")
    if not lowered:
        return "item"
    ascii_only = lowered.encode("ascii", "ignore").decode("ascii")
    ascii_only = re.sub(r"-{2,}", "-", ascii_only).strip("-")
    return ascii_only or lowered


def tokenize(text: str) -> set[str]:
    tokens = set(re.findall(r"[a-z0-9][a-z0-9_-]*", text.lower()))
    for segment in re.findall(r"[一-鿿]+", text):
        if len(segment) == 1:
            tokens.add(segment)
            continue
        tokens.add(segment)
        for index in range(len(segment) - 1):
            tokens.add(segment[index: index + 2])
    return tokens


def overlap_score(left: str, right: str) -> float:
    l_tokens = tokenize(left)
    r_tokens = tokenize(right)
    if not l_tokens or not r_tokens:
        return 0.0
    return len(l_tokens & r_tokens) / math.sqrt(len(l_tokens) * len(r_tokens))


def read_json(path: Path, default: Any | None = None) -> Any:
    if not path.exists():
        if default is not None:
            return default
        raise FileNotFoundError(path)
    return json.loads(path.read_text(encoding="utf-8-sig"))


def write_json(path: Path, payload: Any) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(payload, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")


def append_jsonl(path: Path, payload: dict[str, Any]) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    with path.open("a", encoding="utf-8", newline="\n") as f:
        f.write(json.dumps(payload, ensure_ascii=False) + "\n")


def read_jsonl(path: Path) -> list[dict[str, Any]]:
    if not path.exists():
        return []
    records: list[dict[str, Any]] = []
    for line in path.read_text(encoding="utf-8").splitlines():
        if line.strip():
            records.append(json.loads(line))
    return records


def write_text(path: Path, content: str) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(content, encoding="utf-8", newline="\n")


def append_text(path: Path, content: str) -> None:
    """Append content to an existing file (typically Markdown log)."""
    path.parent.mkdir(parents=True, exist_ok=True)
    existing = path.read_text(encoding="utf-8") if path.exists() else ""
    path.write_text(existing + content, encoding="utf-8", newline="\n")


def parse_frontmatter(skill_path: Path) -> dict[str, str]:
    lines = skill_path.read_text(encoding="utf-8").splitlines()
    if len(lines) < 3 or lines[0].lstrip("﻿").strip() != "---":
        raise RuntimeError(f"Missing YAML frontmatter: {skill_path}")
    result: dict[str, str] = {}
    for line in lines[1:]:
        if line.strip() == "---":
            break
        if ":" not in line:
            continue
        key, value = line.split(":", 1)
        result[key.strip()] = value.strip()
    return result


def find_skill_file(skill_dir: Path) -> Path:
    for child in skill_dir.iterdir():
        if child.is_file() and child.name.lower() == "skill.md":
            return child
    raise RuntimeError(f"Missing SKILL.md in {skill_dir}")


def _remove_readonly(func, path, exc_info) -> None:
    try:
        os.chmod(path, stat.S_IWRITE)
    except OSError:
        pass
    func(path)


def remove_tree(path: Path) -> None:
    if path.exists():
        shutil.rmtree(path, onerror=_remove_readonly)


# ---------------------------------------------------------------------------
# Path resolution
# ---------------------------------------------------------------------------

class Paths:
    def __init__(self, repo_root: Path) -> None:
        self.root = repo_root / "skills"
        self.library = self.root / "library" / "skills"
        self.signals_dir = self.root / "lifecycle" / "learning" / "signals"
        self.candidates_dir = self.root / "lifecycle" / "learning" / "candidates"
        self.incubator = self.root / "lifecycle" / "incubator"
        self.telemetry = self.root / "lifecycle" / "telemetry"
        self.governance = self.root / "lifecycle" / "governance"
        self.promotion_log = self.governance / "promotion-log.md"


def resolve_paths() -> Paths:
    script_dir = Path(__file__).resolve().parent
    output = subprocess.check_output(
        ["git", "-C", str(script_dir), "rev-parse", "--show-toplevel"], text=True
    )
    return Paths(Path(output.strip()))


# ---------------------------------------------------------------------------
# Subcommand: collect
# ---------------------------------------------------------------------------

def collect_signals(paths: Paths, args: argparse.Namespace) -> int:
    record = {
        "timestamp": now_iso(),
        "summary": args.text,
        "domain_hint": args.domain_hint or "engineering",
        "action_hint": args.action_hint or "feature",
        "source": "manual",
    }
    month_file = paths.signals_dir / f"{datetime.now().strftime('%Y-%m')}.jsonl"
    append_jsonl(month_file, record)
    print(f"[skill-learn] Signal recorded to {month_file.name}")
    return 0


# ---------------------------------------------------------------------------
# Subcommand: cluster
# ---------------------------------------------------------------------------

def cluster_signals(paths: Paths, args: argparse.Namespace) -> int:
    all_signals: list[dict] = []
    for f in sorted(paths.signals_dir.glob("*.jsonl")):
        all_signals.extend(read_jsonl(f))

    if not all_signals:
        print("[skill-learn] No signals to cluster.")
        return 0

    groups: dict[str, list[dict]] = defaultdict(list)
    for s in all_signals:
        key = slugify(s.get("summary", ""))[:60] or "unclustered"
        groups[key].append(s)

    for group_id, items in groups.items():
        if len(items) < args.min_evidence:
            continue
        candidate_dir = paths.candidates_dir / group_id
        candidate_dir.mkdir(parents=True, exist_ok=True)

        candidate = {
            "id": group_id,
            "created": now_iso(),
            "evidence_count": len(items),
            "proposal": items[0].get("summary", ""),
            "domain_hint": max(items, key=lambda x: x.get("evidence_count", 1)).get("domain_hint", "engineering"),
            "evidence": items[:10],
        }
        write_json(candidate_dir / "candidate.json", candidate)

        evidence_lines = []
        for item in items[:10]:
            evidence_lines.append(f"- {item.get('summary', '')}")
        write_text(candidate_dir / "evidence.md", "\n".join(evidence_lines))

        print(f"[skill-learn] Candidate '{group_id}' created with {len(items)} evidence(s).")

    if not groups:
        print("[skill-learn] No groups formed (min_evidence threshold not met).")
    return 0


# ---------------------------------------------------------------------------
# Subcommand: draft
# ---------------------------------------------------------------------------

def draft_candidate(paths: Paths, args: argparse.Namespace) -> int:
    candidate_dir = paths.candidates_dir / args.candidate_id
    if not candidate_dir.exists():
        print(f"[skill-learn] Candidate '{args.candidate_id}' not found.")
        return 1

    candidate = read_json(candidate_dir / "candidate.json")
    skill_name = slugify(candidate.get("proposal", ""))[:40] or "new-skill"
    incubator_dir = paths.incubator / candidate["id"]
    incubator_dir.mkdir(parents=True, exist_ok=True)

    skill_md = (
        f"---\nname: {skill_name}\ndescription: 由学习信号自动生成的草稿 skill，用于 {candidate['proposal']}\n---\n\n"
        f"# {skill_name}\n\n## 概述\n\n该草稿来自 `{candidate['id']}`，用于把重复出现的经验信号沉淀成可维护的 project skill。\n\n"
        f"## 学习来源\n- candidate: `{candidate['id']}`\n- proposal: `{candidate['proposal']}`\n"
        f"- evidence count: `{candidate['evidence_count']}`\n\n## 当前问题\n{candidate.get('proposal', '')}\n\n"
        f"## 使用流程\n1. 根据触发信号判断是否应该进入本 skill。\n2. 完成任务后把新增经验继续回写到 learning 闭环。\n\n"
        f"## 红旗\n- 不要把一次性任务记录伪装成长期 skill。\n- 不要在没有 benchmark 和 promote 审核前把草稿移入正式 skills。\n\n"
        f"## 下一步\n1. 收敛 description、summary 和 trigger_signals。\n2. 补齐 benchmark cases。\n"
        f"3. 通过 review 后再 promote 到正式 skills。\n"
    )
    write_text(incubator_dir / "SKILL.md", skill_md)

    manifest = {
        "kind": "project",
        "domain": candidate.get("domain_hint", "workflow"),
        "summary": f"TODO: 补充 {skill_name} 的一句话摘要",
        "trigger_signals": [f"TODO: 补充 {skill_name} 的触发信号"],
        "owner": "00-skills-maintainers",
        "status": "draft",
        "startup_visible": False,
        "token_budget": "small",
    }
    write_json(incubator_dir / "skill.manifest.json", manifest)
    write_text(incubator_dir / "candidate-notes.md", f"Candidate ID: {candidate['id']}\nEvidence count: {candidate['evidence_count']}\n")

    print(f"[skill-learn] Draft created in lifecycle/incubator/{candidate['id']}/")
    return 0


# ---------------------------------------------------------------------------
# Subcommand: benchmark
# ---------------------------------------------------------------------------

def benchmark_candidate(paths: Paths, args: argparse.Namespace) -> int:
    incubator_dir = paths.incubator / args.candidate_id
    if not incubator_dir.exists():
        print(f"[skill-learn] Incubator '{args.candidate_id}' not found.")
        return 1

    skill_file = incubator_dir / "SKILL.md"
    manifest_file = incubator_dir / "skill.manifest.json"
    if not skill_file.exists() or not manifest_file.exists():
        print(f"[skill-learn] Incubator missing SKILL.md or manifest.json.")
        return 1

    manifest = read_json(manifest_file)
    frontmatter = parse_frontmatter(skill_file)
    skill_body = skill_file.read_text(encoding="utf-8")

    issues: list[str] = []

    domain_catalog = read_json(paths.root / "discovery" / "domain-catalog.json", {"domains": []})
    known_domains = {d["id"] for d in domain_catalog.get("domains", [])}
    if manifest.get("domain") not in known_domains:
        issues.append(f"Unknown domain '{manifest.get('domain')}'")

    summary = manifest.get("summary", "")
    if "TODO" in summary or "todo" in summary.lower():
        issues.append("Manifest summary has TODO placeholder")

    desc = frontmatter.get("description", "")
    if "TODO" in desc or "todo" in desc.lower():
        issues.append("Frontmatter description has TODO placeholder")

    signals = manifest.get("trigger_signals", [])
    signal_count = len(signals)
    if signal_count < 1:
        issues.append("No trigger_signals defined")
    if any("TODO" in str(s) for s in signals):
        issues.append("trigger_signals contains TODO placeholder")

    body = skill_body.split("---", 2)[-1] if skill_body.count("---") >= 2 else skill_body
    if "TODO" in body:
        issues.append("SKILL.md body contains TODO placeholder")

    # Check overlap with formal skills
    if paths.library.exists():
        for formal_dir in sorted(paths.library.iterdir()):
            if not formal_dir.is_dir():
                continue
            formal_manifest_file = formal_dir / "skill.manifest.json"
            if not formal_manifest_file.exists():
                continue
            formal_manifest = read_json(formal_manifest_file)
            formal_summary = formal_manifest.get("summary", "")
            score = overlap_score(summary, formal_summary)
            if score > 0.78:
                issues.append(f"High overlap ({score:.2f}) with formal skill '{formal_dir.name}'")

    result = {
        "candidate_id": args.candidate_id,
        "timestamp": now_iso(),
        "passed": len(issues) == 0,
        "issues": issues,
        "overlap_checked": True,
    }
    write_json(incubator_dir / "benchmark.json", result)

    if result["passed"]:
        print(f"[skill-learn] Benchmark PASSED for '{args.candidate_id}'")
    else:
        print(f"[skill-learn] Benchmark FAILED for '{args.candidate_id}':")
        for issue in issues:
            print(f"  - {issue}")

    return 0 if result["passed"] else 1


# ---------------------------------------------------------------------------
# Subcommand: review
# ---------------------------------------------------------------------------

def review_candidate(paths: Paths, args: argparse.Namespace) -> int:
    incubator_dir = paths.incubator / args.candidate_id
    if not incubator_dir.exists():
        print(f"[skill-learn] Incubator '{args.candidate_id}' not found.")
        return 1

    benchmark = read_json(incubator_dir / "benchmark.json", {})
    issues = benchmark.get("issues", [])

    review_result = {
        "candidate_id": args.candidate_id,
        "timestamp": now_iso(),
        "benchmark_passed": benchmark.get("passed", False),
        "blocking_issues": issues,
        "approved": benchmark.get("passed", False) and len(issues) == 0,
    }
    write_json(incubator_dir / "review.json", review_result)

    if review_result["approved"]:
        print(f"[skill-learn] Review APPROVED for '{args.candidate_id}'")
    else:
        print(f"[skill-learn] Review DENIED for '{args.candidate_id}'. Blocking issues:")
        for issue in issues:
            print(f"  - {issue}")
    return 0


# ---------------------------------------------------------------------------
# Subcommand: promote
# ---------------------------------------------------------------------------

def promote_candidate(paths: Paths, args: argparse.Namespace) -> int:
    incubator_dir = paths.incubator / args.candidate_id
    if not incubator_dir.exists():
        print(f"[skill-learn] Incubator '{args.candidate_id}' not found.")
        return 1

    review = read_json(incubator_dir / "review.json", {})
    if not review.get("approved", False):
        print(f"[skill-learn] Cannot promote '{args.candidate_id}': not approved.")
        return 1

    skill_file = incubator_dir / "SKILL.md"
    manifest_file = incubator_dir / "skill.manifest.json"
    if not skill_file.exists() or not manifest_file.exists():
        print(f"[skill-learn] Missing SKILL.md or manifest.json.")
        return 1

    frontmatter = parse_frontmatter(skill_file)
    skill_name = frontmatter.get("name", "new-skill")
    manifest = read_json(manifest_file)
    directory_name = f"dev-{skill_name}"
    target_dir = paths.library / directory_name

    if target_dir.exists():
        print(f"[skill-learn] Target already exists: {target_dir}")
        return 1

    if args.dry_run:
        print(f"[skill-learn] DRY RUN: Would promote '{args.candidate_id}' -> library/skills/{directory_name}")
        return 0

    # Backup old version if exists
    if target_dir.exists():
        backup_dir = paths.governance / "backups" / f"{directory_name}-{datetime.now().strftime('%Y%m%d-%H%M%S')}"
        shutil.copytree(target_dir, backup_dir)
        remove_tree(target_dir)

    shutil.copytree(incubator_dir, target_dir)

    # Clean sidecar files
    for sidecar in ["candidate-notes.md", "scaffold.meta.json", "benchmark.json", "review.json"]:
        (target_dir / sidecar).unlink(missing_ok=True)

    # Update manifest status
    manifest["status"] = "active"
    write_json(target_dir / "skill.manifest.json", manifest)

    # Log promotion
    log_line = f"| {datetime.now().strftime('%Y-%m-%d')} | {directory_name} | incubator/{args.candidate_id} | 自动学习晋升 |\n"
    append_text(paths.promotion_log, log_line)

    # Refresh catalog
    _refresh_catalog()

    print(f"[skill-learn] Promoted '{args.candidate_id}' -> library/skills/{directory_name}")
    return 0


def _refresh_catalog() -> None:
    """Refresh the skill catalog after promotion."""
    script = Path(__file__).resolve().parent.parent / "catalog" / "generate_skill_catalog.py"
    if script.exists():
        try:
            subprocess.run([sys.executable, str(script)], check=True, capture_output=True, text=True)
            print(f"[skill-learn] Catalog refreshed.")
        except subprocess.CalledProcessError as e:
            print(f"[skill-learn] Catalog refresh failed: {e.stderr}")

    # Also run verification if available
    verify_ps1 = Path(__file__).resolve().parent.parent / "verification" / "verify-skill-pipeline.ps1"
    verify_py = Path(__file__).resolve().parent.parent / "verification" / "verify_skill_pipeline.py"
    if verify_ps1.exists():
        try:
            pwsh_path = shutil.which("pwsh")
            if not pwsh_path:
                print("[skill-learn] pwsh not found, skipping pipeline verification.")
            else:
                result = subprocess.run(
                    [pwsh_path, str(verify_ps1)],
                    check=False, capture_output=True, text=True, timeout=60,
                )
                if result.returncode == 0:
                    print(f"[skill-learn] Pipeline verification passed.")
                else:
                    print(f"[skill-learn] Pipeline verification reported issues:\n{result.stderr[:500]}")
        except (FileNotFoundError, OSError):
            # Fallback to Python verify script
            if verify_py.exists():
                try:
                    result = subprocess.run(
                        [sys.executable, str(verify_py)],
                        check=False, capture_output=True, text=True, timeout=60,
                    )
                    if result.returncode == 0:
                        print(f"[skill-learn] Pipeline verification passed (python).")
                    else:
                        print(f"[skill-learn] Pipeline verification reported issues:\n{result.stdout[:500]}")
                except subprocess.TimeoutExpired:
                    print(f"[skill-learn] Pipeline verification timed out (python).")
            else:
                print(f"[skill-learn] Skipping pipeline verification (no pwsh or python verify script).")
        except subprocess.TimeoutExpired:
            print(f"[skill-learn] Pipeline verification timed out.")


# ---------------------------------------------------------------------------
# Subcommand: promote-manual
# ---------------------------------------------------------------------------

def promote_manual_scaffold(paths: Paths, args: argparse.Namespace) -> int:
    scaffold_dir = paths.incubator / args.scaffold_id
    if not scaffold_dir.exists():
        print(f"[skill-learn] Scaffold '{args.scaffold_id}' not found.")
        return 1

    metadata = read_json(scaffold_dir / "scaffold.meta.json", {})
    directory_name = metadata.get("target_directory_name", args.scaffold_id.replace("manual-", ""))
    target_dir = paths.library / directory_name

    if target_dir.exists():
        print(f"[skill-learn] Target already exists: {target_dir}")
        return 1

    if args.dry_run:
        print(f"[skill-learn] DRY RUN: Would promote '{args.scaffold_id}' -> library/skills/{directory_name}")
        return 0

    shutil.copytree(scaffold_dir, target_dir)

    # Clean sidecar
    for sidecar in ["scaffold.meta.json", "benchmark.json", "review.json", "candidate-notes.md"]:
        (target_dir / sidecar).unlink(missing_ok=True)

    # Update manifest
    manifest = read_json(target_dir / "skill.manifest.json")
    manifest["status"] = "active"
    write_json(target_dir / "skill.manifest.json", manifest)

    log_line = f"| {datetime.now().strftime('%Y-%m-%d')} | {directory_name} | incubator/{args.scaffold_id} | 手动 scaffold 晋升 |\n"
    append_text(paths.promotion_log, log_line)

    _refresh_catalog()
    print(f"[skill-learn] Promoted manual scaffold '{args.scaffold_id}' -> library/skills/{directory_name}")
    return 0


# ---------------------------------------------------------------------------
# Subcommand: observe
# ---------------------------------------------------------------------------

def observe_skills(paths: Paths, args: argparse.Namespace) -> int:
    observation: dict[str, Any] = {
        "timestamp": now_iso(),
        "formal_skills": [],
        "incubator_skills": [],
        "candidates": [],
    }

    if paths.library.exists():
        for skill_dir in sorted(paths.library.iterdir()):
            if not skill_dir.is_dir():
                continue
            manifest = read_json(skill_dir / "skill.manifest.json", {})
            observation["formal_skills"].append({
                "name": skill_dir.name,
                "domain": manifest.get("domain", "unknown"),
                "status": manifest.get("status", "unknown"),
                "budget": manifest.get("token_budget", "small"),
            })

    if paths.incubator.exists():
        for d in sorted(paths.incubator.iterdir()):
            if d.is_dir():
                observation["incubator_skills"].append(d.name)

    if paths.candidates_dir.exists():
        for d in sorted(paths.candidates_dir.iterdir()):
            if d.is_dir():
                observation["candidates"].append(d.name)

    write_json(paths.telemetry / f"observe-{datetime.now().strftime('%Y%m%d-%H%M%S')}.json", observation)

    print(f"[skill-learn] Observe snapshot written.")
    print(f"  Formal skills: {len(observation['formal_skills'])}")
    print(f"  Incubator drafts: {len(observation['incubator_skills'])}")
    print(f"  Candidates: {len(observation['candidates'])}")
    return 0


# ---------------------------------------------------------------------------
# Subcommand: retire
# ---------------------------------------------------------------------------

def retire_skill(paths: Paths, args: argparse.Namespace) -> int:
    target_dir = paths.library / args.skill_name
    if not target_dir.exists():
        print(f"[skill-learn] Skill '{args.skill_name}' not found in library.")
        return 1

    manifest = read_json(target_dir / "skill.manifest.json", {})
    if manifest.get("status") == "deprecated":
        print(f"[skill-learn] Skill '{args.skill_name}' already deprecated.")
        return 0

    if args.dry_run:
        print(f"[skill-learn] DRY RUN: Would retire '{args.skill_name}' (reason: {args.reason})")
        return 0

    # Backup
    backup_dir = paths.governance / "backups" / f"{args.skill_name}-{datetime.now().strftime('%Y%m%d-%H%M%S')}"
    shutil.copytree(target_dir, backup_dir)

    # Mark deprecated
    manifest["status"] = "deprecated"
    manifest["retire_reason"] = args.reason or "not specified"
    manifest["retired_at"] = now_iso()
    write_json(target_dir / "skill.manifest.json", manifest)

    # Log
    log_line = f"| {datetime.now().strftime('%Y-%m-%d')} | {args.skill_name} | - | 退役: {args.reason} |\n"
    append_text(paths.promotion_log, log_line)

    _refresh_catalog()
    print(f"[skill-learn] Retired '{args.skill_name}' (reason: {args.reason})")
    return 0


# ---------------------------------------------------------------------------
# Subcommand: auto-collect
# ---------------------------------------------------------------------------

def auto_collect(paths: Paths, args: argparse.Namespace) -> int:
    usage_file = paths.telemetry / "usage.jsonl"
    if not usage_file.exists():
        print("[skill-learn] No usage data to auto-collect.")
        return 0

    records = read_jsonl(usage_file)
    # Group by skill_path
    skill_uses = Counter(r.get("skill_path", "") for r in records)
    month_file = paths.signals_dir / f"{datetime.now().strftime('%Y-%m')}.jsonl"

    signals_generated = 0
    for skill_path, count in skill_uses.items():
        if count >= args.min_uses:
            signal = {
                "timestamp": now_iso(),
                "summary": f"Skill '{skill_path}' used {count} times in this period",
                "domain_hint": "skilling",
                "action_hint": "refine",
                "source": "auto-collect",
                "usage_count": count,
            }
            append_jsonl(month_file, signal)
            signals_generated += 1

    print(f"[skill-learn] Auto-collect: {signals_generated} signal(s) from usage data.")
    return 0


# ---------------------------------------------------------------------------
# Subcommand: auto-assess
# ---------------------------------------------------------------------------

def auto_assess(paths: Paths, args: argparse.Namespace) -> int:
    # Load formal skills
    formal_skills = []
    if paths.library.exists():
        for d in sorted(paths.library.iterdir()):
            if not d.is_dir():
                continue
            m = read_json(d / "skill.manifest.json", {})
            formal_skills.append({
                "name": d.name,
                "domain": m.get("domain", "unknown"),
                "status": m.get("status", "unknown"),
                "budget": m.get("token_budget", "small"),
            })

    # Load candidates
    candidates = []
    if paths.candidates_dir.exists():
        for d in sorted(paths.candidates_dir.iterdir()):
            if d.is_dir():
                c = read_json(d / "candidate.json", {})
                candidates.append({"id": d.name, "evidence": c.get("evidence_count", 0)})

    # Load incubator
    incubator_count = len([d for d in paths.incubator.iterdir() if d.is_dir()]) if paths.incubator.exists() else 0

    # Domain distribution
    domain_dist = Counter(s["domain"] for s in formal_skills)
    status_dist = Counter(s["status"] for s in formal_skills)

    assessment = {
        "timestamp": now_iso(),
        "formal_count": len(formal_skills),
        "incubator_count": incubator_count,
        "candidate_count": len(candidates),
        "domain_distribution": dict(domain_dist),
        "status_distribution": dict(status_dist),
        "stale_candidates": [c for c in candidates if c["evidence"] < 2],
    }
    write_json(paths.telemetry / f"assess-{datetime.now().strftime('%Y%m%d-%H%M%S')}.json", assessment)

    print(f"[skill-learn] Assessment snapshot written.")
    print(f"  Formal: {assessment['formal_count']}, Incubator: {assessment['incubator_count']}, Candidates: {assessment['candidate_count']}")
    return 0


# ---------------------------------------------------------------------------
# Subcommand: auto-report
# ---------------------------------------------------------------------------

def auto_report(paths: Paths, args: argparse.Namespace) -> int:
    # Load latest assessment
    assess_files = sorted(paths.telemetry.glob("assess-*.json"))
    assessment = read_json(assess_files[-1]) if assess_files else {}

    report = {
        "timestamp": now_iso(),
        "period": f"{datetime.now().strftime('%Y-%m')}",
        "formal_count": assessment.get("formal_count", 0),
        "incubator_count": assessment.get("incubator_count", 0),
        "candidate_count": assessment.get("candidate_count", 0),
        "domain_distribution": assessment.get("domain_distribution", {}),
    }

    if args.output in ("json", "both"):
        report_path = paths.telemetry / f"report-{datetime.now().strftime('%Y%m%d')}.json"
        write_json(report_path, report)
        print(f"[skill-learn] Report written to {report_path}")

    if args.output in ("md", "both"):
        md = (
            f"# Skill System Report ({report['period']})\n\n"
            f"| Metric | Value |\n|--------|-------|\n"
            f"| Formal skills | {report['formal_count']} |\n"
            f"| Incubator drafts | {report['incubator_count']} |\n"
            f"| Candidates | {report['candidate_count']} |\n\n"
            f"## Domain Distribution\n\n| Domain | Count |\n|--------|-------|\n"
        )
        for domain, count in sorted(report["domain_distribution"].items()):
            md += f"| {domain} | {count} |\n"
        report_md = paths.telemetry / f"report-{datetime.now().strftime('%Y%m%d')}.md"
        write_text(report_md, md)
        print(f"[skill-learn] Report written to {report_md}")

    return 0


# ---------------------------------------------------------------------------
# Subcommand: auto-cleanup
# ---------------------------------------------------------------------------

def auto_cleanup(paths: Paths, args: argparse.Namespace) -> int:
    cutoff = datetime.now(timezone.utc).astimezone() - timedelta(days=args.lookback_days)

    # Find zero-usage skills by checking telemetry
    usage_file = paths.telemetry / "usage.jsonl"
    used_skills: set[str] = set()
    if usage_file.exists():
        for r in read_jsonl(usage_file):
            sp = r.get("skill_path", "")
            if sp:
                used_skills.add(sp)

    stale: list[str] = []
    if paths.library.exists():
        for d in sorted(paths.library.iterdir()):
            if not d.is_dir():
                continue
            if d.name not in used_skills:
                stale.append(d.name)

    # Optional: also flag LOW-USE skills from health engine
    health_low_use: list[str] = []
    if args.health_engine:
        health_dir = paths.telemetry / "health"
        if health_dir.exists():
            snapshots = sorted(health_dir.glob("health-snapshot-*.json"))
            if snapshots:
                try:
                    snap = json.loads(snapshots[-1].read_text(encoding="utf-8"))
                    for name, data in snap.get("skills", {}).items():
                        m = data.get("metrics", {})
                        raw = data.get("raw_counts", {})
                        if m.get("applied_rate", 1.0) < 0.4 and raw.get("sessions", 0) >= 5:
                            if name not in stale and name not in health_low_use:
                                health_low_use.append(name)
                except (json.JSONDecodeError, OSError):
                    pass

    if args.dry_run:
        print(f"[skill-learn] DRY RUN: {len(stale)} stale skill(s) would be retired:")
        for s in stale:
            print(f"  - {s}")
        if health_low_use:
            print(f"\n[skill-learn] {len(health_low_use)} LOW-USE skill(s) from health snapshot:")
            for s in health_low_use:
                print(f"  - {s} (LOW-USE)")
        return 0

    for skill_name in stale:
        # Retire without backup for auto-cleanup
        target_dir = paths.library / skill_name
        manifest = read_json(target_dir / "skill.manifest.json", {})
        manifest["status"] = "deprecated"
        manifest["retire_reason"] = "auto-cleanup: no usage in lookback period"
        manifest["retired_at"] = now_iso()
        write_json(target_dir / "skill.manifest.json", manifest)
        log_line = f"| {datetime.now().strftime('%Y-%m-%d')} | {skill_name} | - | 自动退役: 无使用记录 |\n"
        append_text(paths.promotion_log, log_line)

    for skill_name in health_low_use:
        if skill_name in stale:
            continue
        target_dir = paths.library / skill_name
        manifest = read_json(target_dir / "skill.manifest.json", {})
        manifest["status"] = "deprecated"
        manifest["retire_reason"] = "auto-cleanup: LOW-USE from health engine"
        manifest["retired_at"] = now_iso()
        write_json(target_dir / "skill.manifest.json", manifest)
        log_line = f"| {datetime.now().strftime('%Y-%m-%d')} | {skill_name} | - | 自动退役: LOW-USE |\n"
        append_text(paths.promotion_log, log_line)

    _refresh_catalog()
    total = len(stale) + len(health_low_use)
    print(f"[skill-learn] Auto-cleanup: {len(stale)} zero-usage + {len(health_low_use)} LOW-USE = {total} skill(s) retired.")
    return 0


# ---------------------------------------------------------------------------
# Evolution subcommands: benchmark/review/promote proposals from evolve.py
# ---------------------------------------------------------------------------

def evolve_benchmark(paths: Paths, args: argparse.Namespace) -> int:
    """Benchmark an evolution proposal (from lifecycle/evolution/proposals/)."""
    proposals_dir = paths.root / "lifecycle" / "evolution" / "proposals"
    proposal_dir = proposals_dir / args.proposal_id
    if not proposal_dir.exists():
        print(f"[skill-learn] Evolution proposal '{args.proposal_id}' not found.")
        return 1

    prop = read_json(proposal_dir / "evolution-proposal.json", {})
    manifest = read_json(proposal_dir / "skill.manifest.json", {})
    issues: list[str] = []

    # 1. Version lineage integrity
    evo_type = prop.get("type", "")
    version_from = prop.get("version_from", "")
    version_to = prop.get("version_to", prop.get("version", ""))
    if evo_type == "fix":
        if not version_to or not version_from:
            issues.append("FIX proposal missing version_from or version_to")
        elif version_to <= version_from:
            issues.append(f"Version must increase: {version_from} -> {version_to}")

    # 2. Manifest has version
    if not manifest.get("version"):
        issues.append("Manifest missing version field")

    # 3. SKILL.md exists and has frontmatter
    skill_file = proposal_dir / "SKILL.md"
    if not skill_file.exists():
        issues.append("Missing SKILL.md in proposal")
    else:
        try:
            fm = parse_frontmatter(skill_file)
            if not fm.get("name"):
                issues.append("SKILL.md frontmatter missing name")
        except RuntimeError:
            issues.append("SKILL.md missing frontmatter")

    # 4. Check overlap with formal skills for capture/derive
    if evo_type in ("capture", "derive"):
        summary = manifest.get("summary", "")
        if paths.library.exists():
            for formal_dir in sorted(paths.library.iterdir()):
                if not formal_dir.is_dir():
                    continue
                fm_file = formal_dir / "skill.manifest.json"
                if not fm_file.exists():
                    continue
                fm_manifest = read_json(fm_file)
                fm_summary = fm_manifest.get("summary", "")
                score = overlap_score(summary, fm_summary)
                if score > 0.78:
                    if fm_manifest.get("name") != prop.get("parent_skill"):
                        issues.append(f"High overlap ({score:.2f}) with formal skill '{formal_dir.name}'")

    result = {
        "proposal_id": args.proposal_id,
        "type": evo_type,
        "timestamp": now_iso(),
        "passed": len(issues) == 0,
        "issues": issues,
    }
    write_json(proposal_dir / "evolve-benchmark.json", result)

    if result["passed"]:
        print(f"[skill-learn] Evolve benchmark PASSED for '{args.proposal_id}'")
    else:
        print(f"[skill-learn] Evolve benchmark FAILED for '{args.proposal_id}':")
        for issue in issues:
            print(f"  - {issue}")
    return 0 if result["passed"] else 1


def evolve_review(paths: Paths, args: argparse.Namespace) -> int:
    """Review an evolution proposal benchmark."""
    proposals_dir = paths.root / "lifecycle" / "evolution" / "proposals"
    proposal_dir = proposals_dir / args.proposal_id
    if not proposal_dir.exists():
        print(f"[skill-learn] Evolution proposal '{args.proposal_id}' not found.")
        return 1

    benchmark = read_json(proposal_dir / "evolve-benchmark.json", {})
    issues = benchmark.get("issues", [])

    review_result = {
        "proposal_id": args.proposal_id,
        "timestamp": now_iso(),
        "benchmark_passed": benchmark.get("passed", False),
        "blocking_issues": issues,
        "approved": benchmark.get("passed", False) and len(issues) == 0,
    }
    write_json(proposal_dir / "evolve-review.json", review_result)

    if review_result["approved"]:
        print(f"[skill-learn] Evolve review APPROVED for '{args.proposal_id}'")
    else:
        print(f"[skill-learn] Evolve review DENIED for '{args.proposal_id}':")
        for issue in issues:
            print(f"  - {issue}")
    return 0


def evolve_promote(paths: Paths, args: argparse.Namespace) -> int:
    """Promote an approved evolution proposal to formal library."""
    proposals_dir = paths.root / "lifecycle" / "evolution" / "proposals"
    proposal_dir = proposals_dir / args.proposal_id
    if not proposal_dir.exists():
        print(f"[skill-learn] Evolution proposal '{args.proposal_id}' not found.")
        return 1

    review = read_json(proposal_dir / "evolve-review.json", {})
    if not review.get("approved", False):
        print(f"[skill-learn] Cannot promote '{args.proposal_id}': not approved.")
        return 1

    prop = read_json(proposal_dir / "evolution-proposal.json", {})
    manifest = read_json(proposal_dir / "skill.manifest.json", {})
    evo_type = prop.get("type", "")
    skill_name_from_prop = prop.get("skill", "")

    if args.dry_run:
        if evo_type == "fix":
            print(f"[skill-learn] DRY RUN: Would FIX '{skill_name_from_prop}' -> library/skills/{skill_name_from_prop}")
        elif evo_type == "derive":
            print(f"[skill-learn] DRY RUN: Would DERIVE '{skill_name_from_prop}' -> library/skills/{skill_name_from_prop}")
        elif evo_type == "capture":
            print(f"[skill-learn] DRY RUN: Would promote CAPTURED '{skill_name_from_prop}' -> library/skills/{skill_name_from_prop}")
        return 0

    if evo_type == "fix":
        target_dir = paths.library / skill_name_from_prop
        if not target_dir.exists():
            print(f"[skill-learn] Target skill '{skill_name_from_prop}' not found in library.")
            return 1

        backup_dir = paths.governance / "backups" / f"{skill_name_from_prop}-{datetime.now().strftime('%Y%m%d-%H%M%S')}"
        shutil.copytree(target_dir, backup_dir)
        remove_tree(target_dir)
        shutil.copytree(proposal_dir, target_dir)

        for sidecar in ["evolution-proposal.json", "evolve-benchmark.json", "evolve-review.json"]:
            (target_dir / sidecar).unlink(missing_ok=True)

        tgt_manifest = read_json(target_dir / "skill.manifest.json")
        tgt_manifest["status"] = "active"
        write_json(target_dir / "skill.manifest.json", tgt_manifest)

        log_line = f"| {datetime.now().strftime('%Y-%m-%d')} | {skill_name_from_prop} | evolution/{args.proposal_id} | FIX: {prop.get('version_from', '')} -> {prop.get('version_to', '')} |\n"
        append_text(paths.promotion_log, log_line)
        print(f"[skill-learn] FIX promoted: {skill_name_from_prop} ({prop.get('version_from', '')} -> {prop.get('version_to', '')})")

    elif evo_type == "derive":
        target_dir = paths.library / skill_name_from_prop
        if target_dir.exists():
            print(f"[skill-learn] Target '{skill_name_from_prop}' already exists.")
            return 1

        shutil.copytree(proposal_dir, target_dir)
        for sidecar in ["evolution-proposal.json", "evolve-benchmark.json", "evolve-review.json"]:
            (target_dir / sidecar).unlink(missing_ok=True)

        tgt_manifest = read_json(target_dir / "skill.manifest.json")
        tgt_manifest["status"] = "active"
        write_json(target_dir / "skill.manifest.json", tgt_manifest)

        parent_name = prop.get("parent_skill", "")
        if parent_name:
            parent_dir = paths.library / parent_name
            if parent_dir.exists():
                parent_manifest = read_json(parent_dir / "skill.manifest.json")
                if "evolution_lineage" not in parent_manifest:
                    parent_manifest["evolution_lineage"] = {"parent": None, "derived_from": None, "derived_to": [], "fixes": [], "captured_from": None}
                if "derived_to" not in parent_manifest["evolution_lineage"]:
                    parent_manifest["evolution_lineage"]["derived_to"] = []
                if skill_name_from_prop not in parent_manifest["evolution_lineage"]["derived_to"]:
                    parent_manifest["evolution_lineage"]["derived_to"].append(skill_name_from_prop)
                write_json(parent_dir / "skill.manifest.json", parent_manifest)

        log_line = f"| {datetime.now().strftime('%Y-%m-%d')} | {skill_name_from_prop} | evolution/{args.proposal_id} | DERIVE from {parent_name} |\n"
        append_text(paths.promotion_log, log_line)
        print(f"[skill-learn] DERIVE promoted: {skill_name_from_prop} (derived from {parent_name})")

    elif evo_type == "capture":
        target_dir = paths.library / skill_name_from_prop
        if target_dir.exists():
            print(f"[skill-learn] Target '{skill_name_from_prop}' already exists.")
            return 1

        shutil.copytree(proposal_dir, target_dir)
        for sidecar in ["evolution-proposal.json", "evolve-benchmark.json", "evolve-review.json"]:
            (target_dir / sidecar).unlink(missing_ok=True)

        tgt_manifest = read_json(target_dir / "skill.manifest.json")
        tgt_manifest["status"] = "active"
        tgt_manifest["version"] = "1.0.0"
        if "version_history" not in tgt_manifest:
            tgt_manifest["version_history"] = []
        tgt_manifest["version_history"].append({
            "version": "1.0.0",
            "date": now_iso()[:10],
            "type": "captured",
            "description": "Promoted from 0.1.0 to 1.0.0 (formal release)"
        })
        write_json(target_dir / "skill.manifest.json", tgt_manifest)

        log_line = f"| {datetime.now().strftime('%Y-%m-%d')} | {skill_name_from_prop} | evolution/{args.proposal_id} | CAPTURE: 0.1.0 -> 1.0.0 |\n"
        append_text(paths.promotion_log, log_line)
        print(f"[skill-learn] CAPTURE promoted: {skill_name_from_prop} (0.1.0 -> 1.0.0)")

    _refresh_catalog()
    return 0


def evolve_history(paths: Paths, args: argparse.Namespace) -> int:
    """Show evolution history for a skill."""
    lineage_dir = paths.root / "lifecycle" / "evolution" / "lineage"

    if args.skill:
        lineage_file = lineage_dir / f"{args.skill}.jsonl"
        if not lineage_file.exists():
            print(f"[skill-learn] No evolution history for '{args.skill}'.")
            return 0
        records = read_jsonl(lineage_file)
        print(f"=== Evolution History: {args.skill} ===\n")
        for r in records:
            evo_type = r.get("type", "unknown").upper()
            v_from = r.get("version_from", r.get("version", ""))
            v_to = r.get("version_to", "")
            ts = r.get("timestamp", "")[:19]
            reason = r.get("reason", "")[:80]
            if v_to:
                print(f"  [{evo_type}] {ts}  {v_from} -> {v_to}")
            else:
                print(f"  [{evo_type}] {ts}  v{r.get('version', '?')}")
            if reason:
                print(f"       {reason}")
            print()
    else:
        if not lineage_dir.exists():
            print("[skill-learn] No evolution lineage data found.")
            return 0
        files = sorted(lineage_dir.glob("*.jsonl"))
        if not files:
            print("[skill-learn] No lineage files found.")
            return 0
        print("=== Skills with Evolution History ===\n")
        for f in files:
            records = read_jsonl(f)
            count = len(records)
            last = records[-1] if records else {}
            evo_type = last.get("type", "initial").upper()
            version = last.get("version_to", last.get("version", "?"))
            print(f"  {f.stem}: {count} change(s), latest={version} ({evo_type})")
        print()

    return 0


# ---------------------------------------------------------------------------
# Parser
# ---------------------------------------------------------------------------

def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description="Skill self-learning pipeline")
    subparsers = parser.add_subparsers(dest="command")

    # collect
    cp = subparsers.add_parser("collect", help="Record a learning signal")
    cp.add_argument("text", help="Signal description")
    cp.add_argument("--domain-hint", default="engineering")
    cp.add_argument("--action-hint", default="feature")

    # cluster
    cp = subparsers.add_parser("cluster", help="Cluster signals into candidates")
    cp.add_argument("--min-evidence", type=int, default=2)

    # draft
    cp = subparsers.add_parser("draft", help="Generate incubator draft from candidate")
    cp.add_argument("candidate_id")

    # benchmark
    cp = subparsers.add_parser("benchmark", help="Benchmark an incubator candidate")
    cp.add_argument("candidate_id")

    # review
    cp = subparsers.add_parser("review", help="Review benchmark results")
    cp.add_argument("candidate_id")

    # promote
    cp = subparsers.add_parser("promote", help="Promote candidate to formal library")
    cp.add_argument("candidate_id")
    cp.add_argument("--dry-run", action="store_true")

    # promote-manual
    cp = subparsers.add_parser("promote-manual", help="Promote manual scaffold to formal library")
    cp.add_argument("scaffold_id")
    cp.add_argument("--dry-run", action="store_true")

    # observe
    subparsers.add_parser("observe", help="Generate telemetry snapshot")

    # retire
    cp = subparsers.add_parser("retire", help="Retire a formal skill")
    cp.add_argument("skill_name")
    cp.add_argument("--dry-run", action="store_true")
    cp.add_argument("--reason", default="not specified")

    # auto-collect
    cp = subparsers.add_parser("auto-collect", help="Auto-collect signals from usage data")
    cp.add_argument("--min-uses", type=int, default=3)

    # auto-assess
    subparsers.add_parser("auto-assess", help="Multi-factor assessment")

    # auto-report
    cp = subparsers.add_parser("auto-report", help="Generate comprehensive report")
    cp.add_argument("--output", choices=["json", "md", "both"], default="both")

    # auto-cleanup
    cp = subparsers.add_parser("auto-cleanup", help="Auto-retire stale skills")
    cp.add_argument("--lookback-days", type=int, default=90)
    cp.add_argument("--dry-run", action="store_true", default=True)
    cp.add_argument("--no-dry-run", action="store_false", dest="dry_run")
    cp.add_argument("--health-engine", action="store_true", default=True,
                    help="Also check health snapshots for LOW-USE skills (default: True)")
    cp.add_argument("--no-health-engine", action="store_false", dest="health_engine")

    # evolve-benchmark
    cp = subparsers.add_parser("evolve-benchmark", help="Benchmark an evolution proposal")
    cp.add_argument("proposal_id")

    # evolve-review
    cp = subparsers.add_parser("evolve-review", help="Review evolution benchmark")
    cp.add_argument("proposal_id")

    # evolve-promote
    cp = subparsers.add_parser("evolve-promote", help="Promote evolution proposal to library")
    cp.add_argument("proposal_id")
    cp.add_argument("--dry-run", action="store_true")

    # evolve-history
    cp = subparsers.add_parser("evolve-history", help="Show evolution history for a skill")
    cp.add_argument("--skill", type=str, default=None)

    return parser


def main() -> int:
    parser = build_parser()
    args = parser.parse_args()
    paths = resolve_paths()

    handlers = {
        "collect": collect_signals,
        "cluster": cluster_signals,
        "draft": draft_candidate,
        "benchmark": benchmark_candidate,
        "review": review_candidate,
        "promote": promote_candidate,
        "promote-manual": promote_manual_scaffold,
        "observe": observe_skills,
        "retire": retire_skill,
        "auto-collect": auto_collect,
        "auto-assess": auto_assess,
        "auto-report": auto_report,
        "auto-cleanup": auto_cleanup,
        "evolve-benchmark": evolve_benchmark,
        "evolve-review": evolve_review,
        "evolve-promote": evolve_promote,
        "evolve-history": evolve_history,
    }

    handler = handlers.get(args.command)
    if not handler:
        parser.print_help()
        return 1

    try:
        return int(handler(paths, args))
    except Exception as e:
        print(f"[skill-learn] {e}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    raise SystemExit(main())
