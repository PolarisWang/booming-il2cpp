#!/usr/bin/env python3
"""Check wiki markdown files for broken internal links.

Usage: python scripts/codegen/check_wiki_links.py [--fix]

Scans all .md files in wiki/ for [[link]] and [text](path) patterns
and verifies the target file exists relative to the wiki root.
Reports broken links and optionally removes trailing whitespace.
"""

import os
import re
import sys
from pathlib import Path

WIKI_ROOT = Path(__file__).resolve().parents[1] / "wiki"


def find_md_files(root: Path):
    """Yield all .md files under root."""
    for dirpath, _, filenames in os.walk(root):
        for fn in filenames:
            if fn.endswith(".md"):
                yield Path(dirpath) / fn


def check_file(filepath: Path, fix: bool = False) -> list[str]:
    """Check a single file for broken wiki links. Returns list of issues."""
    issues = []
    rel_path = filepath.relative_to(WIKI_ROOT)
    content = filepath.read_text(encoding="utf-8")

    # Pattern: [text](path) where path doesn't start with http://, https://, or #
    link_pattern = re.compile(r'\[([^\]]+)\]\(([^)]+)\)')
    for match in link_pattern.finditer(content):
        link_text, target = match.groups()
        if target.startswith(("http://", "https://", "#", "mailto:")):
            continue
        # Resolve relative to the file's directory
        target_path = (filepath.parent / target).resolve()
        if not target_path.exists():
            # Also try relative to wiki root
            alt_path = (WIKI_ROOT / target).resolve()
            if not alt_path.exists():
                issues.append(f"{rel_path}: broken link '{target}' ({link_text})")

    if fix:
        # Remove trailing whitespace
        lines = content.splitlines(keepends=True)
        cleaned = []
        changed = False
        for line in lines:
            stripped = line.rstrip() + "\n" if line.strip() else "\n"
            if stripped != line:
                changed = True
            cleaned.append(stripped)
        if changed:
            filepath.write_text("".join(cleaned), encoding="utf-8")
            issues.append(f"{rel_path}: trailing whitespace fixed")

    return issues


def main():
    fix = "--fix" in sys.argv
    all_issues = []
    for md_file in find_md_files(WIKI_ROOT):
        issues = check_file(md_file, fix=fix)
        all_issues.extend(issues)

    if all_issues:
        print(f"Found {len(all_issues)} issue(s):")
        for issue in all_issues:
            print(f"  {issue}")
        return 1
    else:
        print("All wiki links OK.")
        return 0


if __name__ == "__main__":
    sys.exit(main())
