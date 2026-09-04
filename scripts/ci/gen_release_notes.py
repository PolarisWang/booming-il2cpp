#!/usr/bin/env python3
"""gen_release_notes.py — generate industrialized, structured Markdown release notes.

Reads CSV lines from stdin:   hash|subject|author
Emits product-grade structured release notes to stdout.

Metadata (version/range/totals/authors/size) is read from environment vars set by
the caller scripts/generate-release-notes.sh:
  GENRL_VERSION_LABEL (str)    — version label, e.g. 0.3.0
  GENRL_RANGE        (str)    — ref range, e.g. v0.2.0..v0.3.0
  GENRL_TOTAL_COMMITS (str)   — int commit count
  GENRL_AUTHORS      (str)    — comma-separated unique authors
  GENRL_FILES_CHANGED (str)   — git diff --shortstat string
  GENRL_DATE         (str)    — ISO date (YYYY-MM-DD); defaults to today
  GENRL_PRERELEASE   (str)    — "true" if prerelease
  GENRL_REPO_URL     (str)    — repo base URL (default below; override for forks)
"""
from __future__ import annotations

import os
import re
import sys
from collections import defaultdict

# ── Repo metadata (single source, overridable via env for forks) ────────────
REPO_URL = os.environ.get("GENRL_REPO_URL",
                          "https://github.com/PolarisWang/booming-il2cpp")

# ── Platform support matrix (curated; edits here flow to every release) ─────
PLATFORM_SUPPORT = [
    ("Windows x64", "Stable"),
    ("Linux x64", "Stable"),
    ("Linux arm64", "Preview"),
    ("macOS x64", "Preview"),
    ("macOS arm64", "Preview"),
    ("Android arm64", "Preview"),
]

# Area detection: ordered list of (regex, label) — first match wins.
# Uses word-boundary-aware regex to avoid the old "test" substring over-match.
_AREA_RULES = [
    (re.compile(r"\bhot[- ]?update\b|patch-loader", re.I), "Hot Update"),
    (re.compile(r"interpreter|interpret|il\-decode", re.I), "Interpreter"),
    (re.compile(r"\binline\b|jit|vreg|regalloc|reg\-alloc|basic\s*block", re.I), "JIT"),
    (re.compile(r"codegen|code-gen|il2cpp|metadata|emit(ter)?|snapshot|sif|shape", re.I), "CodeGen / Emitter"),
    (re.compile(r"\bgc\b|garbage|collector|bgc|old\-gen|new\-gen|regional|gen[0-2]", re.I), "GC"),
    (re.compile(r"benchmark|perf(line|ormance)?|profile", re.I), "Performance / Benchmark"),
    (re.compile(r"^ci:|workflow|github .*action|github pages|coverage|codeql|dependabot", re.I), "CI / Release"),
    (re.compile(r"\bbuild\b|cmake|preset|toolchain|link(er)?|compile|msvc|clang|sdk", re.I), "Build & Toolchain"),
    (re.compile(r"^docs:|wiki|readme|api doc|site$", re.I), "Docs"),
    (re.compile(r"^test|test_case|assert|covers|fixture", re.I), "Testing"),
    (re.compile(r"dependency|upgrade.*nuget|\bnuget\b", re.I), "Dependencies"),
]
_AREA_GENERAL = "General"

# Strip the conventional-commit prefix (e.g. "feat(gc): " → "foo bar")
_STRIP_RE = re.compile(r"^[a-z]+(\([^)]*\))?[!:] *(.*)$", re.I)

CAT_ORDER = [
    "breaking", "features", "bug_fixes", "performance", "build",
    "refactor", "docs", "tests", "chores", "other",
]
CAT_HEADING = {
    "breaking": "Breaking Changes",
    "features": "New Features",
    "bug_fixes": "Bug Fixes",
    "performance": "Performance",
    "build": "CI / Build",
    "refactor": "Refactoring",
    "docs": "Documentation",
    "tests": "Testing",
    "chores": "Chores / Dependencies",
    "other": "Other",
}


def detect_category(subj: str) -> str:
    lc = subj.lower()
    if lc.startswith("revert:"):
        return "breaking"
    if "!:" in lc or lc.startswith("breaking:") or lc.startswith("breaking("):
        return "breaking"
    for pref, cat in (("feat", "features"), ("fix", "bug_fixes"),
                      ("perf", "performance"), ("refactor", "refactor"),
                      ("docs", "docs"), ("test", "tests"),
                      ("ci", "build"), ("build", "build"),
                      ("chore", "chores"), ("deps", "chores")):
        if lc.startswith(pref + "(") or lc == pref + ":" or lc.startswith(pref + ":"):
            return cat
    head = lc.split(":", 1)[0] if ":" in lc else lc
    if head in ("feat", "feature", "features"):
        return "features"
    if head.startswith("fix"):
        return "bug_fixes"
    if head.startswith("perf"):
        return "performance"
    if head in ("refactor", "cleanup", "clean"):
        return "refactor"
    if head in ("ci", "cd"):
        return "build"
    if head == "build":
        return "build"
    if head in ("docs", "doc"):
        return "docs"
    if head == "test" or head.startswith("test_") or head.startswith("add test"):
        return "tests"
    if head == "revert":
        return "breaking"
    if head in ("chore", "deps"):
        return "chores"
    return "other"


def detect_area(subj: str) -> str:
    for rx, label in _AREA_RULES:
        if rx.search(subj):
            return label
    return _AREA_GENERAL


def clean_subject(subj: str) -> str:
    """Strip the conventional-commit prefix for a human-readable summary."""
    m = _STRIP_RE.match(subj.strip())
    return (m.group(2).strip() if m and m.group(2) else subj.strip())


def short_hash(h: str) -> str:
    return h[:7] if h else ""


def load_commits() -> list[dict]:
    commits = []
    for line in sys.stdin:
        line = line.rstrip("\n")
        if not line:
            continue
        parts = line.split("|", 2)
        if len(parts) < 2:
            continue
        h = parts[0]
        subj = parts[1].strip()
        commits.append({"hash": h, "subject": subj})
    return commits


def is_high_value(text: str) -> float:
    """Heuristic score for the Highlights section: favour user-visible work."""
    s = 0.0
    lc = text.lower()
    if lc.startswith("feat"):
        s += 3
    elif lc.startswith("fix"):
        s += 2
    elif lc.startswith("perf"):
        s += 2.5
    if any(k in lc for k in ("gc", "jit", "codegen", "aot", "publish",
                             "standalone", "dotnet tool", "install", "sdk")):
        s += 1
    if ":" in lc:  # has a scope → more substantive
        s += 0.5
    # Penalize pure internal noise so they don't steal highlight slots.
    for noise in ("cleanup", "bump", "version", "merge", "sync",
                  "spell", "comment", "typo", "lint", "changelog"):
        if noise in lc:
            s -= 0.75
    return s


def pick_highlights(commits: list[dict], n: int = 3) -> list[dict]:
    """Auto-pick the most user-notable commits; never returns un-notable ones."""
    scored = [(is_high_value(c["subject"]), c) for c in commits]
    scored = [c for s, c in scored if s >= 2.5]
    scored.sort(key=lambda c: is_high_value(c["subject"]), reverse=True)
    return scored[:n]


def render(categories: dict[str, dict[str, list[dict]]],
           highlights: list[dict], metadata: dict) -> str:
    W = []
    W.append(f"# v{metadata['version']}")
    W.append("")
    tag = metadata.get("tag", "")
    date = metadata.get("date", "")
    total = metadata.get("total", "0")
    files = metadata.get("files", "")

    if date:
        W.append(f"> Released on **{date}**")
        W.append("")

    # Summary strip
    W.append(f"{total} commits" + (f" — {files}" if files else "") + ".")
    W.append("")

    # ── Highlights ────────────────────────────────────────────────────────
    if highlights:
        W.append("## 🚀 Highlights")
        W.append("")
        for c in highlights:
            W.append(f"- {clean_subject(c['subject'])}")
        W.append("")

    # ── Breaking Changes ──────────────────────────────────────────────────
    brk = categories.get("breaking", {})
    brk_items = [i for sub in brk.values() for i in sub]
    if brk_items:
        W.append("## ⚠️ Breaking Changes")
        W.append("")
        W.append("> These changes may require updates to existing code or config.")
        W.append("")
        for i in brk_items:
            W.append(f"- {clean_subject(i['subject'])} (`{short_hash(i['hash'])}`)")
        W.append("")

    # ── Installation ──────────────────────────────────────────────────────
    W.append("## 📦 Installation")
    W.append("")
    ver = metadata["version"]
    if metadata.get("prerelease", False):
        W.append("```bash")
        W.append(f"dotnet tool install --global chaos-il2cpp --version {ver} --prerelease")
        W.append("chaos-il2cpp --version")
        W.append("```")
        W.append("")
    else:
        W.append("```bash")
        W.append(f"dotnet tool install --global chaos-il2cpp --version {ver}")
        W.append("chaos-il2cpp --version")
        W.append("```")
        W.append("")
        W.append("To upgrade from a previous version:")
        W.append("")
        W.append("```bash")
        W.append("dotnet tool update --global chaos-il2cpp")
        W.append("```")
        W.append("")

    # ── What's Changed ────────────────────────────────────────────────────
    W.append("## ✨ What's Changed")
    W.append("")
    for cat_key in CAT_ORDER:
        if cat_key == "breaking":
            continue  # already rendered above
        items_by_area = categories.get(cat_key)
        if not items_by_area:
            continue
        flat = [i for sub in items_by_area.values() for i in sub]
        if not flat:
            continue
        heading = CAT_HEADING.get(cat_key, cat_key.capitalize())
        W.append(f"### {heading}")
        W.append("")
        for i in flat:
            W.append(f"- {clean_subject(i['subject'])} (`{short_hash(i['hash'])}`)")
        W.append("")

    # ── Platform Support ──────────────────────────────────────────────────
    W.append("## 🖥️ Platform Support")
    W.append("")
    W.append("| Platform | Status |")
    W.append("|----------|--------|")
    for plat, status in PLATFORM_SUPPORT:
        W.append(f"| {plat} | {status} |")
    W.append("")

    # ── Full Changelog Link ───────────────────────────────────────────────
    W.append("---")
    W.append("")
    if range_label := metadata.get("range"):
        left, _, right = range_label.partition("..")
        if left and right:
            W.append(f"**Full Changelog**: [{left}...{right}]({REPO_URL}/compare/{left}...{right})")
        else:
            W.append(f"**Full Changelog**: {REPO_URL}/commits/{range_label}")
    W.append("")

    text = "\n".join(W)
    # Collapse 3+ newlines into 2
    text = re.sub(r"\n{3,}", "\n\n", text)
    return text


def _force_utf8_io() -> None:
    """Read stdin & write stdout as UTF-8 regardless of host locale.  Git-log
    data across Windows/linux may mix encodings; forcing UTF-8 on both ends keeps
    the markdown (incl. emoji) lossless and avoids locale-dependent crashes."""
    for stream, mode in ((sys.stdin, "read"), (sys.stdout, "write")):
        try:
            stream.reconfigure(encoding="utf-8", errors="replace")
        except (AttributeError, ValueError):
            pass


def main() -> int:
    _force_utf8_io()

    commits = load_commits() or []
    if not commits:
        print("# Release (no commits in range)")
        return 0

    # Metadata
    version = os.environ.get("GENRL_VERSION_LABEL", "").strip() or "0.x.x"
    total = os.environ.get("GENRL_TOTAL_COMMITS", str(len(commits)))
    authors = os.environ.get("GENRL_AUTHORS", "").strip().lstrip(", ")
    files = os.environ.get("GENRL_FILES_CHANGED", "").strip()
    rng = os.environ.get("GENRL_RANGE", "")
    date = os.environ.get("GENRL_DATE", "")
    prerelease = os.environ.get("GENRL_PRERELEASE", "false") == "true"

    # Classify
    categories: dict[str, dict[str, list[dict]]] = defaultdict(lambda: defaultdict(list))
    for c in commits:
        cat = detect_category(c["subject"])
        if "!:" in c["subject"].lower():
            cat = "breaking"
        area = detect_area(c["subject"])
        categories[cat][area].append(c)

    highlights = pick_highlights(commits, 3)
    # Remove any breaking highlights (already surfaced)
    highlights = [c for c in highlights if detect_category(c["subject"]) != "breaking"]

    metadata = {
        "version": version,
        "range": rng,
        "total": total,
        "authors": authors,
        "files": files,
        "date": date,
        "prerelease": prerelease,
        "tag": f"v{version}",
    }
    text = render(categories, highlights, metadata)
    text = re.sub(r"\n{3,}", "\n\n", text)
    sys.stdout.write(text.rstrip() + "\n")
    return 0


if __name__ == "__main__":
    sys.exit(main())