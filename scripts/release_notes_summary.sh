#!/usr/bin/env bash
# release_notes_summary.sh — extract structured fields from generated RELEASE_NOTES.md
# for consumption by notify-feishu.sh.
#
# Usage:
#   source <(bash scripts/release_notes_summary.sh <release-notes-file>)
#   # sets FEISHU_HIGHLIGHTS, FEISHU_CHANGED_SUMMARY, FEISHU_STATS in the current shell
#
# Or:
#   bash scripts/release_notes_summary.sh <release-notes-file> [prev-tag] [cur-tag]
#   # prints env lines to stdout for eval
#
# Output env vars:
#   FEISHU_HIGHLIGHTS       \n-separated 3 highlights (no leading "- ")
#   FEISHU_CHANGED_SUMMARY  "New Features 1 · Bug Fixes 2 · Performance 0 · Refactor 0 · Docs 0"
#   FEISHU_STATS            "8 commits · 17 files · <prev> → <cur>"

set -euo pipefail

NOTES_FILE="${1:-}"
PREV_TAG="${2:-}"
CUR_TAG="${3:-}"

if [ -z "$NOTES_FILE" ] || [ ! -s "$NOTES_FILE" ]; then
    echo "  [release-notes-summary] no notes file; skipping" >&2
    exit 0
fi

# Resolve a working python interpreter (python3 may be a broken Windows Store alias).
PY=""
for cand in python3 python py; do
    if command -v "$cand" >/dev/null 2>&1 && "$cand" -c 'import sys' >/dev/null 2>&1; then PY="$cand"; break; fi
done
if [ -z "$PY" ]; then echo "  [release-notes-summary] no working python; skipping" >&2; exit 0; fi

eval "$("$PY" - "$NOTES_FILE" "$PREV_TAG" "$CUR_TAG" <<'PYEOF'
import json, os, sys, re

try:
    sys.stdout.reconfigure(encoding="utf-8", errors="replace")
except Exception:
    pass

notes_file = sys.argv[1]
prev_tag = sys.argv[2] or ""
cur_tag = sys.argv[3] or ""

with open(notes_file, "r", encoding="utf-8", errors="replace") as f:
    text = f.read()

# ── Highlights ──
# Between ## 🚀 Highlights and the next ## heading, extract bullet lines.
highlights = []
m = re.search(r"^## 🚀 Highlights\s*\n(.*?)(?=^## )", text, re.MULTILINE | re.DOTALL)
if m:
    block = m.group(1)
    for line in block.split("\n"):
        line = line.strip()
        if line.startswith("- "):
            highlights.append(line[2:].strip())
# Take top 3, encode as \n literal string
hl_str = "\\n".join(h for h in highlights[:3] if h)

# ── What's Changed category counts ──
cat_counts = {}
# Map of section heading to category label
heading_map = {
    "New Features": "New Features",
    "Bug Fixes": "Bug Fixes",
    "Performance": "Performance",
    "Refactoring": "Refactor",
    "CI / Build": "CI / Build",
    "Documentation": "Docs",
    "Testing": "Testing",
    "Chores / Dependencies": "Chores / Deps",
    "Other": "Other",
}
current_cat = None
in_changed = False
for line in text.split("\n"):
    if line.startswith("## ✨ What's Changed"):
        in_changed = True
        continue
    if not in_changed:
        continue
    if line.startswith("## "):
        break  # left the What's Changed section
    for heading, label in heading_map.items():
        if line.strip().startswith("### " + heading):
            current_cat = label
            cat_counts[current_cat] = 0
            break
    else:
        if current_cat and line.strip().startswith("- "):
            cat_counts[current_cat] = cat_counts.get(current_cat, 0) + 1

# Build change summary string, e.g. "New Features 1 · Bug Fixes 2 · Performance 0"
cat_order = ["New Features", "Bug Fixes", "Performance", "Refactor", "CI / Build", "Docs", "Testing", "Chores / Deps"]
change_parts = [f"{c} {cat_counts.get(c, 0)}" for c in cat_order if c in cat_counts]
changed_summary = " · ".join(change_parts)

# ── Stats ──
# Parse the summary line: "8 commits — 17 files changed, 505 insertions(+), 65 deletions(-)."
stats_line = ""
commit_count = ""
files_changed = ""
for line in text.split("\n"):
    line = line.strip()
    m = re.match(r"^(\d+)\s+commits?\s*[—–-]\s*(\d+)\s+files?", line)
    if m:
        commit_count = m.group(1)
        files_changed = m.group(2)
        break

stats_parts = []
if commit_count:
    stats_parts.append(f"{commit_count} commits")
if files_changed:
    stats_parts.append(f"{files_changed} files")
if prev_tag and cur_tag:
    stats_parts.append(f"{prev_tag} → {cur_tag}")
stats_str = " · ".join(stats_parts)

# Print as env vars
print(f"FEISHU_HIGHLIGHTS={json.dumps(hl_str, ensure_ascii=False)}")
print(f"FEISHU_CHANGED_SUMMARY={json.dumps(changed_summary, ensure_ascii=False)}")
print(f"FEISHU_STATS={json.dumps(stats_str, ensure_ascii=False)}")
PYEOF
)"