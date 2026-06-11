#!/usr/bin/env python3
"""Cross-platform skill pipeline verification.

Equivalent to verify-skill-pipeline.ps1 — pure Python, no PowerShell required.
Used by skill_learn.py evolve-promote as fallback when pwsh is unavailable.

Usage:
    python skills/tooling/verification/verify_skill_pipeline.py          # all checks
    python skills/tooling/verification/verify_skill_pipeline.py --bootstrap  # also run bootstrap
"""

from __future__ import annotations

import json
import os
import subprocess
import sys
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


def has_todo(text: str) -> bool:
    return "todo" in text.lower() if text else False


def has_scaffold_placeholder(text: str) -> bool:
    if not text:
        return False
    placeholders = ["TODO: 用 1-2 句话描述该 skill 的核心职责、输入信号和输出边界。"]
    return any(p in text for p in placeholders)


def check_junction(path: Path, expected_target: Path) -> bool:
    """Check if path exists and is a junction pointing to expected_target."""
    if not path.exists():
        return False
    try:
        resolved = path.resolve()
        expected = expected_target.resolve()
        return resolved == expected
    except (OSError, ValueError):
        return False


def main() -> int:
    repo_root = resolve_repo_root()
    if not repo_root:
        print("[verify] ERROR: not in a git repository", file=sys.stderr)
        return 1

    errors: list[str] = []
    chapter_root = repo_root / "skills"
    formal_skills_dir = chapter_root / "library" / "skills"
    bootstrap_script = chapter_root / "runtime" / "bootstrap-skills.ps1"
    catalog_script = chapter_root / "tooling" / "catalog" / "generate_skill_catalog.py"

    do_bootstrap = "--bootstrap" in sys.argv

    # ── Check 1: Scripts exist ──────────────────────────────────────────────
    if not bootstrap_script.exists():
        errors.append(f"Missing bootstrap script: {bootstrap_script}")
    if not catalog_script.exists():
        errors.append(f"Missing catalog script: {catalog_script}")
    if errors:
        for e in errors:
            print(f"[verify] FAIL: {e}")
        return 1

    # ── Check 2: Run bootstrap (optional) ──────────────────────────────────
    if do_bootstrap:
        print("[verify] Running bootstrap...")
        result = subprocess.run(
            ["pwsh", "-File", str(bootstrap_script)] if sys.platform == "win32"
            else ["pwsh", str(bootstrap_script)],
            capture_output=True, text=True, timeout=120,
        )
        if result.returncode != 0:
            print(f"[verify] Bootstrap failed:\n{result.stderr[:500]}", file=sys.stderr)
            return result.returncode

    # ── Check 3: Catalog check ─────────────────────────────────────────────
    print("[verify] Checking catalog...")
    result = subprocess.run(
        [sys.executable, str(catalog_script), "--repo-root", str(repo_root), "--check"],
        capture_output=True, text=True, timeout=60,
    )
    if result.returncode != 0:
        print(f"[verify] Catalog check failed:\n{result.stdout[:500]}\n{result.stderr[:500]}", file=sys.stderr)
        return result.returncode

    # ── Check 4: Junctions ─────────────────────────────────────────────────
    print("[verify] Checking junctions...")
    claude_skills = repo_root / ".claude" / "skills"
    codex_skills = repo_root / ".codex" / "skills"
    library_skills = formal_skills_dir

    if not check_junction(claude_skills, library_skills):
        errors.append(f".claude/skills junction mismatch or missing (expected -> {library_skills})")
    if not check_junction(codex_skills, library_skills):
        errors.append(f".codex/skills junction mismatch or missing (expected -> {library_skills})")

    if errors:
        # Junctions are nice-to-have on some setups; warn but don't fail
        for e in errors:
            print(f"[verify] WARN: {e}")
        errors.clear()

    # ── Check 5: Formal skills ─────────────────────────────────────────────
    print("[verify] Checking formal skills...")
    if formal_skills_dir.exists():
        for skill_dir in sorted(formal_skills_dir.iterdir()):
            if not skill_dir.is_dir():
                continue
            name = skill_dir.name
            manifest_path = skill_dir / "skill.manifest.json"
            skill_file = skill_dir / "SKILL.md"
            metadata_path = skill_dir / "scaffold.meta.json"
            candidate_notes_path = skill_dir / "candidate-notes.md"

            if metadata_path.exists():
                errors.append(f"{name}: contains scaffold.meta.json")
            if candidate_notes_path.exists():
                errors.append(f"{name}: contains candidate-notes.md")
            if not manifest_path.exists():
                errors.append(f"{name}: missing skill.manifest.json")
                continue
            if not skill_file.exists():
                errors.append(f"{name}: missing SKILL.md")
                continue

            try:
                manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
            except (json.JSONDecodeError, OSError) as e:
                errors.append(f"{name}: manifest parse error: {e}")
                continue

            skill_content = skill_file.read_text(encoding="utf-8")

            # description from SKILL.md frontmatter
            description = None
            lines = skill_content.splitlines()
            if len(lines) >= 3 and lines[0].strip() == "---":
                for i in range(1, len(lines)):
                    if lines[i].strip() == "---":
                        break
                    if lines[i].startswith("description:"):
                        description = lines[i][len("description:"):].strip()

            summary = str(manifest.get("summary", ""))
            status = str(manifest.get("status", ""))

            if status == "draft":
                errors.append(f"{name}: status must not be draft")
            if has_todo(summary):
                errors.append(f"{name}: summary contains TODO")
            if description and has_todo(description):
                errors.append(f"{name}: description contains TODO")

            trigger_count = len(manifest.get("trigger_signals", []))
            if trigger_count < 1 or trigger_count > 5:
                errors.append(f"{name}: trigger_signals count {trigger_count} out of range (1-5)")

            if has_scaffold_placeholder(skill_content):
                errors.append(f"{name}: SKILL.md body contains scaffold placeholder")

    # ── Check 6: Evolution infrastructure ──────────────────────────────────
    print("[verify] Checking evolution infrastructure...")
    evolution_dirs = [
        "lifecycle/evolution/proposals",
        "lifecycle/evolution/lineage",
        "lifecycle/telemetry/health",
    ]
    for rel_dir in evolution_dirs:
        full_path = chapter_root / rel_dir
        if not full_path.exists():
            errors.append(f"Missing evolution directory: {rel_dir}")

    # ── Check 7: Manifest evolution fields ─────────────────────────────────
    print("[verify] Checking manifest evolution fields...")
    if formal_skills_dir.exists():
        for skill_dir in sorted(formal_skills_dir.iterdir()):
            if not skill_dir.is_dir():
                continue
            manifest_path = skill_dir / "skill.manifest.json"
            if not manifest_path.exists():
                continue
            try:
                manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
            except (json.JSONDecodeError, OSError):
                continue
            if not manifest.get("version_history"):
                errors.append(f"{skill_dir.name}: missing version_history")
            if not manifest.get("evolution_lineage"):
                errors.append(f"{skill_dir.name}: missing evolution_lineage")

    # ── Summary ────────────────────────────────────────────────────────────
    if errors:
        print(f"[verify] {len(errors)} issue(s) found:")
        for e in errors:
            print(f"  - {e}")
        return 1

    print("[verify] All checks passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
