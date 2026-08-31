#!/usr/bin/env bash
# generate-release-notes.sh — auto-generate release notes from git log.
#
# Categorizes commits between two refs by conventional-commit prefix, groups by
# area (GC/JIT/CodeGen/CI/Build/etc.), and outputs Markdown to stdout.
#
# Usage:
#   ./scripts/generate-release-notes.sh [from-ref] [to-ref]
#   ./scripts/generate-release-notes.sh             # previous tag..HEAD
#   ./scripts/generate-release-notes.sh v0.1.0       # tag..HEAD
#   ./scripts/generate-release-notes.sh v0.1.0 v0.2.0

set -euo pipefail

REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$REPO_ROOT"

FROM_REF="${1:-}"
TO_REF="${2:-HEAD}"

if [ -z "$FROM_REF" ]; then
    FROM_REF=$(git describe --abbrev=0 --tags "$TO_REF^" 2>/dev/null || echo "")
fi
if [ -z "$FROM_REF" ]; then
    FROM_REF=$(git rev-list --max-parents=0 "$TO_REF" 2>/dev/null | tail -1)
fi

RANGE="$FROM_REF..$TO_REF"
echo "== generate-release-notes: $RANGE" >&2

# Write git-log data to a temp file; feed to the python categorizer.
# One CSV line per commit: hash|subject|author
TMP_CSV=$(mktemp)
trap 'rm -f "$TMP_CSV"' EXIT

git log --no-merges --format='%H|%s|%an' "$RANGE" > "$TMP_CSV" 2>/dev/null || {
    echo "No commits in range $RANGE" >&2
    exit 0
}
if [ ! -s "$TMP_CSV" ]; then
    echo "No commits in range $RANGE" >&2
    exit 0
fi

VERSION=$(git show "$TO_REF:VERSION" 2>/dev/null || echo "")
VERSION_LABEL="${VERSION:-$TO_REF}"
TOTAL_COMMITS=$(wc -l < "$TMP_CSV" | tr -d ' ')
AUTHORS=$(git log --no-merges --format='%an' "$RANGE" 2>/dev/null | sort -u | tr '\n' ', ' | sed 's/, $//')
FILES_CHANGED=$(git diff --shortstat "$RANGE" 2>/dev/null | tail -1 || echo "")

# Pass metadata to python via env (avoid quoting collisions).
export GENRL_VERSION_LABEL="$VERSION_LABEL"
export GENRL_RANGE="$RANGE"
export GENRL_TOTAL_COMMITS="$TOTAL_COMMITS"
export GENRL_AUTHORS="$AUTHORS"
export GENRL_FILES_CHANGED="$FILES_CHANGED"

# Resolve a working python interpreter (python3 may be a broken Windows Store alias).
PY=""
for cand in python python3 py; do
    if command -v "$cand" >/dev/null 2>&1 && "$cand" -c 'import sys' >/dev/null 2>&1; then PY="$cand"; break; fi
done
if [ -z "$PY" ]; then echo "Error: no working python interpreter found" >&2; exit 1; fi

"$PY" "$REPO_ROOT/scripts/ci/gen_release_notes.py" < "$TMP_CSV" || {
    # Fallback: simple git log listing
    echo "# Release $VERSION_LABEL"
    echo ""
    echo "## Commits"
    echo ""
    git log --no-merges --format='- %s' "$RANGE" 2>/dev/null
}