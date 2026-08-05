#!/usr/bin/env python3
"""Generate skill index and domain registry pages from library/ and runtime/ skills."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from pathlib import Path


GENERATED_NOTICE = (
    "> 本页由 `.ai/skills/tooling/catalog/generate_skill_catalog.py` 自动生成。"
    "修改 `skill.manifest.json` 或 `discovery/domain-catalog.json` 后重新生成。"
)


def resolve_repo_root(repo_root_arg: str | None) -> Path:
    if repo_root_arg:
        return Path(repo_root_arg).resolve()
    script_dir = Path(__file__).resolve().parent
    output = subprocess.check_output(
        ["git", "-C", str(script_dir), "rev-parse", "--show-toplevel"],
        text=True,
    )
    return Path(output.strip()).resolve()


def load_json(path: Path) -> dict:
    return json.loads(path.read_text(encoding="utf-8-sig"))


def write_if_changed(path: Path, content: str) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    if path.exists() and path.read_text(encoding="utf-8") == content:
        return
    path.write_text(content, encoding="utf-8", newline="\n")


def find_skill_file(skill_dir: Path) -> Path:
    for child in skill_dir.iterdir():
        if child.is_file() and child.name.lower() == "skill.md":
            return child
    raise RuntimeError(f"Missing SKILL.md in skill directory: {skill_dir}")


def parse_frontmatter(skill_path: Path) -> dict[str, str]:
    lines = skill_path.read_text(encoding="utf-8").splitlines()
    if len(lines) < 3 or lines[0].lstrip("﻿").strip() != "---":
        raise RuntimeError(f"Skill file missing YAML frontmatter: {skill_path}")

    result: dict[str, str] = {}
    for line in lines[1:]:
        if line.strip() == "---":
            break
        if ":" not in line:
            continue
        key, value = line.split(":", 1)
        result[key.strip()] = value.strip()

    if "name" not in result or "description" not in result:
        raise RuntimeError(
            f"Skill frontmatter must contain name and description: {skill_path}"
        )
    return result


def inline_code(value: str) -> str:
    return f"`{value}`"


def markdown_link(label: str, target: str) -> str:
    return f"[{label}]({target})"


def markdown_table_line(columns: list[str]) -> str:
    return "| " + " | ".join(columns) + " |"


def trigger_text(signals: list[str]) -> str:
    return "；".join(signal for signal in signals if signal.strip())


def load_skill_entry(skill_dir: Path, kind: str, known_domains: dict[str, dict]) -> dict:
    manifest_path = skill_dir / "skill.manifest.json"
    if not manifest_path.exists():
        raise RuntimeError(f"Missing skill.manifest.json: {skill_dir}")

    manifest = load_json(manifest_path)
    required_fields = [
        "kind", "domain", "summary", "trigger_signals",
        "owner", "status", "startup_visible", "token_budget",
    ]
    for field in required_fields:
        if field not in manifest:
            raise RuntimeError(f"Manifest missing '{field}': {manifest_path}")

    if manifest["kind"] != kind:
        raise RuntimeError(
            f"Manifest kind '{manifest['kind']}' does not match expected '{kind}': {manifest_path}"
        )

    if kind == "project" and manifest["domain"] not in known_domains:
        raise RuntimeError(
            f"Unknown domain '{manifest['domain']}' in manifest: {manifest_path}"
        )

    skill_path = find_skill_file(skill_dir)
    frontmatter = parse_frontmatter(skill_path)

    return {
        "name": frontmatter["name"],
        "description": frontmatter["description"],
        "kind": kind,
        "domain": manifest["domain"],
        "directory_name": skill_dir.name,
        "summary": manifest["summary"],
        "trigger_signals": list(manifest["trigger_signals"]),
        "owner": manifest["owner"],
        "status": manifest["status"],
        "token_budget": manifest["token_budget"],
        "startup_visible": bool(manifest["startup_visible"]),
        "skill_path": f"library/skills/{skill_dir.name}/{skill_path.name}",
        "version": manifest.get("version", "1.0.0"),
    }


def is_publishable_entry(entry: dict) -> bool:
    return entry["status"].lower() != "draft"


def generate_skill_index(domains: list[dict], project_entries: list[dict]) -> str:
    lines = [
        "# Skill Index",
        "",
        GENERATED_NOTICE,
        "",
        "## 加载说明",
        "",
        "1. 先按「领域索引」选择一个最相关的 registry 页面。",
        "2. 只读取命中的 `SKILL.md`；确有需要时再读取 `references/` 或 `scripts/`。",
        "3. 启动阶段不加载全量 catalog。",
        "",
        "## 领域索引",
        "",
        markdown_table_line(["领域", "何时读取", "registry", "skill 数量"]),
        markdown_table_line(["----", "--------", "--------", "---------"]),
    ]

    for domain in domains:
        count = sum(1 for entry in project_entries if entry["domain"] == domain["id"])
        lines.append(
            markdown_table_line([
                inline_code(domain["title"]),
                domain["selection_hint"],
                markdown_link(domain["title"], f"./registries/{domain['id']}.md"),
                str(count),
            ])
        )

    lines.extend([
        "",
        "## 维护规则",
        "",
        "- 新增 skill 时，必须先创建 `skill.manifest.json` 并选择唯一 domain。",
        "- formal `library/skills/` 只保留正式内容；draft 应停留在 `lifecycle/incubator/`。",
        "- registry 由生成脚本刷新，不手工维护长表。",
        "- 所有修改需在 `.ai/skills/library/skills/` 中完成，`.claude/skills/` 和 `.codex/skills/` 由工具自动同步。",
        "",
    ])
    return "\n".join(lines)


def generate_registry_index(domains: list[dict], project_entries: list[dict]) -> str:
    lines = [
        "# Skill Registries",
        "",
        GENERATED_NOTICE,
        "",
        markdown_table_line(["领域", "范围", "页面", "skill 数量"]),
        markdown_table_line(["----", "----", "----", "---------"]),
    ]

    for domain in domains:
        count = sum(1 for entry in project_entries if entry["domain"] == domain["id"])
        lines.append(
            markdown_table_line([
                inline_code(domain["title"]),
                domain["summary"],
                markdown_link(domain["title"], f"./{domain['id']}.md"),
                str(count),
            ])
        )

    lines.append("")
    return "\n".join(lines)


def generate_domain_registry(domain: dict, entries: list[dict]) -> str:
    lines = [
        f"# {domain['title']} registry",
        "",
        GENERATED_NOTICE,
        "",
        "## 何时读取本域",
        "",
        domain["selection_hint"],
        "",
        "## 技能清单",
        "",
        markdown_table_line(["skill.name", "触发信号", "摘要", "version", "owner", "budget", "status"]),
        markdown_table_line(["-----------", "--------", "----", "-------", "-----", "------", "------"]),
    ]

    for entry in entries:
        lines.append(
            markdown_table_line([
                markdown_link(inline_code(entry["name"]), "../../" + entry["skill_path"]),
                trigger_text(entry["trigger_signals"]),
                entry["summary"],
                entry.get("version", "1.0.0"),
                inline_code(entry["owner"]),
                entry["token_budget"],
                entry["status"],
            ])
        )

    lines.append("")
    return "\n".join(lines)


def build_outputs(
    chapter_root: Path, domains: list[dict], project_entries: list[dict]
) -> dict[Path, str]:
    outputs: dict[Path, str] = {}
    discovery_root = chapter_root / "discovery"

    outputs[discovery_root / "skill-index.md"] = generate_skill_index(domains, project_entries)

    registries_dir = discovery_root / "registries"
    outputs[registries_dir / "INDEX.md"] = generate_registry_index(domains, project_entries)

    for domain in domains:
        domain_entries = sorted(
            (entry for entry in project_entries if entry["domain"] == domain["id"]),
            key=lambda item: item["name"],
        )
        outputs[registries_dir / f"{domain['id']}.md"] = generate_domain_registry(
            domain, domain_entries
        )

    return outputs


def verify_outputs(outputs: dict[Path, str]) -> None:
    issues: list[str] = []
    for path, expected in outputs.items():
        if not path.exists():
            issues.append(f"missing: {path}")
            continue
        actual = path.read_text(encoding="utf-8")
        if actual != expected:
            issues.append(f"stale: {path}")
    if issues:
        raise RuntimeError("Catalog outputs are not up to date:\n" + "\n".join(issues))


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--repo-root")
    parser.add_argument("--check", action="store_true")
    args = parser.parse_args()

    repo_root = resolve_repo_root(args.repo_root)
    chapter_root = repo_root / ".ai" / "skills"

    domain_catalog = load_json(chapter_root / "discovery" / "domain-catalog.json")
    domains = sorted(domain_catalog["domains"], key=lambda item: item["order"])
    known_domains = {domain["id"]: domain for domain in domains}

    project_skill_dirs = sorted(
        entry
        for entry in (chapter_root / "library" / "skills").iterdir()
        if entry.is_dir()
    )

    project_entries = [
        load_skill_entry(skill_dir, "project", known_domains)
        for skill_dir in project_skill_dirs
    ]
    published_entries = [
        entry for entry in project_entries if is_publishable_entry(entry)
    ]

    all_names = [entry["name"] for entry in published_entries]
    duplicates = sorted({name for name in all_names if all_names.count(name) > 1})
    if duplicates:
        raise RuntimeError("Duplicate skill names found: " + ", ".join(duplicates))

    outputs = build_outputs(chapter_root, domains, published_entries)
    if args.check:
        verify_outputs(outputs)
        print("[skill-catalog] Catalog outputs are up to date.")
        return 0

    for path, content in outputs.items():
        write_if_changed(path, content)

    print("[skill-catalog] Updated skill index and registry pages.")
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except Exception as exc:
        print(f"[skill-catalog] {exc}", file=sys.stderr)
        raise SystemExit(1)
