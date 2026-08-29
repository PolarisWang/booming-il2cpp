#!/usr/bin/env bash
# release_bump.sh — bump the project version and sync it everywhere.
#
# The single source of truth for the project version is the top-level `VERSION`
# file. This script reads it and propagates the value to:
#   - CMakeLists.txt              (CHAOS_VERSION)
#   - src/managed/Directory.Build.props  (the managed <Version>)
#   - a git tag `v<version>`       (SemVer tag for releases)
#
# This is the ONLY sanctioned way to change the project version. Edit VERSION,
# run this script, and the three consumers stay in lock-step. The agent-facing
# `scripts/cleanliness/check_release_governance.py` gate then verifies they
# actually agree on every commit/CI run.
#
# Usage:
#   ./scripts/release_bump.sh                 # sync from current VERSION
#   ./scripts/release_bump.sh 0.2.0           # set VERSION=0.2.0 then sync
#   ./scripts/release_bump.sh --tag           # also create + push tag v<VERSION>
#   ./scripts/release_bump.sh --dry-run       # show what would change, change nothing
#
# Note: this repo forbids `git stash`; this script never stashes.

set -euo pipefail
REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
VERSION_FILE="$REPO_ROOT/VERSION"

DRY_RUN=0
DO_TAG=0
NEW_VER=""

for a in "$@"; do
    case "$a" in
        --dry-run) DRY_RUN=1 ;;
        --tag) DO_TAG=1 ;;
        *)
            if [[ -z "$NEW_VER" ]]; then
                NEW_VER="$a"
            else
                echo "error: unexpected argument '$a'" >&2
                exit 2
            fi
            ;;
    esac
done

# ── 1. Resolve the new version ────────────────────────────────────────────
if [[ -z "$NEW_VER" ]]; then
    if [[ -f "$VERSION_FILE" ]]; then
        NEW_VER="$(cat "$VERSION_FILE" | tr -d '[:space:]')"
    else
        echo "error: no VERSION file and no version argument given" >&2
        exit 2
    fi
fi

# Basic SemVer sanity (MAJOR.MINOR.PATCH, optionally -prerelease).
if ! [[ "$NEW_VER" =~ ^[0-9]+\.[0-9]+\.[0-9]+(-[0-9A-Za-z.-]+)?$ ]]; then
    echo "error: '$NEW_VER' is not a valid SemVer (expect MAJOR.MINOR.PATCH)" >&2
    exit 2
fi

echo "== VERSION -> $NEW_VER"

# ── 2. Write VERSION ──────────────────────────────────────────────────────
if [[ "$DRY_RUN" -eq 1 ]]; then
    echo "  [dry] VERSION would be: $NEW_VER"
else
    printf '%s\n' "$NEW_VER" > "$VERSION_FILE"
    echo "  [ok ] VERSION  = $NEW_VER"
fi

# ── 3. Sync CMakeLists.txt CHAOS_VERSION ──────────────────────────────────
CMAKE_FILE="$REPO_ROOT/CMakeLists.txt"
if grep -q 'set(CHAOS_VERSION' "$CMAKE_FILE"; then
    if [[ "$DRY_RUN" -eq 1 ]]; then
        echo "  [dry] CMakeLists CHAOS_VERSION -> $NEW_VER"
    else
        sed -i "s/set(CHAOS_VERSION \"[^\"]*\")/set(CHAOS_VERSION \"$NEW_VER\")/" "$CMAKE_FILE"
        echo "  [ok ] CMakeLists CHAOS_VERSION = $NEW_VER"
    fi
else
    echo "  [skip] no CHAOS_VERSION marker in CMakeLists.txt (didn't touch)" >&2
fi

# ── 4. Sync src/managed/Directory.Build.props <Version> ───────────────────
PROPS_FILE="$REPO_ROOT/src/managed/Directory.Build.props"
if [[ -f "$PROPS_FILE" ]] && grep -q '<Version>' "$PROPS_FILE"; then
    if [[ "$DRY_RUN" -eq 1 ]]; then
        echo "  [dry] Directory.Build.props <Version> -> $NEW_VER"
    else
        # preserve leading indent
        sed -i "s/^\(\s*<Version>\).*\(<\/Version>\)$/\1$NEW_VER\2/" "$PROPS_FILE"
        echo "  [ok ] Directory.Build.props <Version> = $NEW_VER"
    fi
else
    echo "  [skip] no <Version> in Directory.Build.props (didn't touch)" >&2
fi

# ── 5. Optional tag ───────────────────────────────────────────────────────
if [[ "$DO_TAG" -eq 1 ]]; then
    TAG="v$NEW_VER"
    if git -C "$REPO_ROOT" rev-parse -q --verify "refs/tags/$TAG" >/dev/null 2>&1; then
        echo "  [ok ] tag already exists: $TAG"
    else
        if [[ "$DRY_RUN" -eq 1 ]]; then
            echo "  [dry] would tag: $TAG"
        else
            git -C "$REPO_ROOT" tag -a "$TAG" -m "release $NEW_VER"
            echo "  [ok ] tagged: $TAG"
        fi
    fi
fi

if [[ "$DRY_RUN" -eq 1 ]]; then
    echo "(dry run — no changes made)"
else
    echo "== done. Verify with: python scripts/cleanliness/check_release_governance.py"
fi
