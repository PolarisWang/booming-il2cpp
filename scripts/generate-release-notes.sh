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
source "$REPO_ROOT/scripts/release-config.sh"

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
# Format: hash|subject|author  (body is NOT included — breaking-change detection
# is done on the subject line itself via the `!:` convention, which is simpler
# and already enforced by the commit-style guide).
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

# Fix author trailing comma: join with bare commas, then replace with comma+space.
# The old `tr '\n' ', ' | sed 's/, $//'` could leave a trailing comma because
# the `$` anchor mismatches when the input has trailing whitespace.
AUTHORS=$(git log --no-merges --format='%an' "$RANGE" 2>/dev/null \
    | sort -u \
    | tr '\n' ',' \
    | sed 's/,$//' \
    | sed 's/,/, /g')

FILES_CHANGED=$(git diff --shortstat "$RANGE" 2>/dev/null | tail -1 || echo "")

# Pass metadata to python via env (avoid quoting collisions).
# GENRL_REPO_URL defaults to the centralized RC_REPO_URL (from release-config.sh),
# so release notes full-changelog links track the fork config in one place.
export GENRL_VERSION_LABEL="$VERSION_LABEL"
export GENRL_RANGE="$RANGE"
export GENRL_TOTAL_COMMITS="$TOTAL_COMMITS"
export GENRL_AUTHORS="$AUTHORS"
export GENRL_FILES_CHANGED="$FILES_CHANGED"
export GENRL_DATE="$(date +%Y-%m-%d)"
export GENRL_REPO_URL="${GENRL_REPO_URL:-$RC_REPO_URL}"

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