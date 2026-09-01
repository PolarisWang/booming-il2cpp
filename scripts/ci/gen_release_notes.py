#!/usr/bin/env python3
"""gen_release_notes.py — categorize git commit lines into grouped Markdown.

Reads CSV lines from stdin:   hash|subject|author
Emits categorized Markdown release notes to stdout.

Metadata (version/range/totals/authors/size) is read from environment vars set by
the caller scripts/generate-release-notes.sh:
  GENRL_VERSION_LABEL (str)
  GENRL_RANGE        (str)
  GENRL_TOTAL_COMMITS (str)
  GENRL_AUTHORS      (str)
  GENRL_FILES_CHANGED (str)
"""
from __future__ import annotations

import collections
import os
import sys


def categorize() -> dict[str, dict[str, list[str]]]:
    categories: dict[str, dict[str, list[str]]] = (
        collections.defaultdict(lambda: collections.defaultdict(list))
    )
    for line in sys.stdin:
        line = line.rstrip("\n")
        if not line:
            continue
        parts = line.split("|", 2)
        if len(parts) < 2:
            continue
        msg = parts[1]
        lc = msg.lower()

        # ── Category by conventional-commit prefix ──────────────────────
        category = "other"
        if lc.startswith("breaking:") or "!:" in lc or lc.startswith("breaking ("):
            category = "breaking"
        elif lc.startswith(("feat:", "feature:")) or "feat(" in lc or "feature(" in lc:
            category = "features"
        elif lc.startswith("fix:") or "fix(" in lc or lc.startswith("bug") or lc.startswith("hotfix"):
            category = "bug_fixes"
        elif lc.startswith("perf:") or "perf(" in lc or lc.startswith("optimiz"):
            category = "performance"
        elif lc.startswith(("ci:", "cd:", "build:")) or "ci(" in lc or "build(" in lc:
            category = "ci"
        elif lc.startswith(("refactor:", "cleanup:")) or "refactor(" in lc or lc.startswith("clean:"):
            category = "refactor"
        elif lc.startswith("docs:") or "docs(" in lc or lc.startswith("doc:"):
            category = "docs"
        elif lc.startswith("test:") or "test(" in lc or lc.startswith(("add test", "add_test", "adding test")):
            category = "tests"
        elif lc.startswith("revert:"):
            category = "reverts"
        elif lc.startswith(("chore:", "deps:")) or "chore(" in lc or lc.startswith("dependabot"):
            category = "chores"

        # ── Area by keyword ─────────────────────────────────────────────
        area = "General"
        if "gc" in lc or "garbage" in lc or "collector" in lc or "bgc" in lc \
                or "old-gen" in lc or "gen1" in lc or "gen2" in lc:
            area = "GC"
        if "jit" in lc:
            area = "JIT"
        if "codegen" in lc or "code-gen" in lc or "il2cpp" in lc:
            area = "CodeGen"
        if "ci" in lc or "workflow" in lc or "github" in lc or "action" in lc:
            area = "CI"
        if "hotupdate" in lc or "hot-update" in lc or "hot update" in lc \
                or lc.startswith("patch") or "patch-" in lc:
            area = "HotUpdate"
        if "interpreter" in lc or "interpret" in lc:
            area = "Interpreter"
        if "build" in lc or "cmake" in lc or "preset" in lc or "toolchain" in lc:
            area = "Build"
        if "test" in lc or "testframework" in lc or "fact" in lc or "benchmark" in lc or "verification" in lc:
            area = "Testing"
        if "docs" in lc or "wiki" in lc or "readme" in lc or "changelog" in lc:
            area = "Docs"

        categories[category][area].append(msg)

    return categories


CAT_ORDER = [
    "breaking", "features", "bug_fixes", "performance", "ci", "refactor",
    "docs", "tests", "reverts", "chores", "other",
]
CAT_HEADING = {
    "breaking": "Breaking Changes",
    "features": "Features",
    "bug_fixes": "Bug Fixes",
    "performance": "Performance",
    "ci": "CI / Build",
    "refactor": "Refactor",
    "docs": "Docs",
    "tests": "Testing",
    "reverts": "Reverts",
    "chores": "Chores / Dependencies",
    "other": "Other",
}


def main() -> int:
    categories = categorize()

    version_label = os.environ.get("GENRL_VERSION_LABEL", "")
    range_label = os.environ.get("GENRL_RANGE", "")
    total = os.environ.get("GENRL_TOTAL_COMMITS", "0")
    authors = os.environ.get("GENRL_AUTHORS", "")
    files_changed = os.environ.get("GENRL_FILES_CHANGED", "")

    out = []
    out.append(f"# Release {version_label}")
    out.append("")
    out.append(f"> Auto-generated release notes for range `{range_label}`.")
    out.append("")
    out.append("## Summary")
    out.append("")
    out.append(f"- **Commits**: {total}")
    out.append(f"- **Authors**: {authors}")
    if files_changed:
        out.append(f"- **Size**: {files_changed}")
    out.append("")

    for cat_key in CAT_ORDER:
        areas = categories.get(cat_key)
        if not areas:
            continue
        out.append(f"## {CAT_HEADING.get(cat_key, cat_key)}")
        out.append("")
        for area_key in sorted(areas.keys()):
            out.append(f"### {area_key}")
            out.append("")
            for msg in areas[area_key]:
                out.append(f"- {msg}")
            out.append("")

    sys.stdout.write("\n".join(out).rstrip() + "\n")
    return 0


if __name__ == "__main__":
    sys.exit(main())
