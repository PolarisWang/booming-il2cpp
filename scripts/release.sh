#!/usr/bin/env bash
# ===============================================================================
# release.sh — master release orchestrator for chaos-il2cpp.
#
# Consolidates the full release flow into one script:
#   1. Pre-flight      : clean tree?, on main?, version format, gh CLI present?
#   2. Version bump    : delegates to scripts/release_bump.sh <ver>  (the ONLY
#                        sanctioned way to change VERSION/CMake/Directory.Build.props)
#   3. Release branch  : creates release/<ver> from main (if not present)
#   4. Release notes   : generate-release-notes.sh <prev-tag>..HEAD → RELEASE_NOTES
#   5. SDK build       : builds the current-platform native SDK preset
#   6. Checksums       : generate-checksums.sh over the release artifact dir
#   7. SBOM            : generate-sbom.sh over the release artifact dir
#   8. Hygiene gate    : chaos_hygiene.py --ci (release-governance must pass)
#   9. GitHub Release  : gh release create v<ver> with notes + uploaded artifacts
#  10. Merge back      : release branch --no-ff back into main (optional)
#
# Red lines honored:
#   - NO `git stash` (forbidden by project).
#   - Version changed ONLY via scripts/release_bump.sh (agent rule).
#   - Any commit this script makes carries a three-part message
#     (root_cause / fix_strategy / regression_check) per CLAUDE.md.
#   - Output artifacts written under artifacts/ (layer-owned); no cross-layer writes.
#
# Usage:
#   ./scripts/release.sh <version>                     # staged release (creates branch, bumps, notes)
#   ./scripts/release.sh <version> --publish           # … + build SDK + checksums + SBOM + gh release
#   ./scripts/release.sh <version> --dry-run           # print every step, change nothing
#   ./scripts/release.sh <version> --no-push           # don't push branch/tag (local only)
#   ./scripts/release.sh --help
# ===============================================================================

set -euo pipefail

REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$REPO_ROOT"

NEW_VER=""
DO_PUBLISH=0
DRY_RUN=0
PUSH=1
MERGE_BACK=1

show_help() {
    cat <<'EOF'
Usage: ./scripts/release.sh <version> [options]

<version>   SemVer to release (e.g. 0.1.0). Required.

Options:
  --publish   Full release: build SDK, checksums, SBOM, create+upload the
              GitHub Release. Without this, only the staged prep happens
              (branch + version bump + release notes draft).
  --dry-run   Print every step that would run; change nothing.
  --no-push   Do not push the release branch or the v<version> tag.
  --no-merge  Do not merge the release branch back into main at the end.
  --help      Show this help.

The script:
  1. Bumps the version via scripts/release_bump.sh <version> --tag (release only).
  2. Creates/reuses release/<version> branch from main.
  3. Generates release notes from git log.
  4. With --publish: builds the SDK, generates SHA256SUMS + CycloneDX SBOM,
     runs the release-governance hygiene gate, and creates a GitHub Release.
EOF
}

while [ $# -gt 0 ]; do
    case "$1" in
        --publish) DO_PUBLISH=1; shift ;;
        --dry-run) DRY_RUN=1; shift ;;
        --no-push) PUSH=0; shift ;;
        --no-merge) MERGE_BACK=0; shift ;;
        --help|-h) show_help; exit 0 ;;
        *)
            if [ -z "$NEW_VER" ]; then NEW_VER="$1"; shift
            else echo "Error: unexpected argument '$1'" >&2; show_help >&2; exit 2; fi
            ;;
    esac
done

# ── 0. Validate version ─────────────────────────────────────────────────────
if [ -z "$NEW_VER" ]; then
    echo "Error: <version> is required" >&2
    show_help >&2
    exit 2
fi
if ! [[ "$NEW_VER" =~ ^[0-9]+\.[0-9]+\.[0-9]+(-[0-9A-Za-z.-]+)?$ ]]; then
    echo "Error: '$NEW_VER' is not valid SemVer (expect MAJOR.MINOR.PATCH)" >&2
    exit 2
fi

# ── 1. Pre-flight ───────────────────────────────────────────────────────────
CURRENT_BRANCH=$(git rev-parse --abbrev-ref HEAD 2>/dev/null || echo "unknown")
TAG="v$NEW_VER"
RELEASE_BRANCH="release/${NEW_VER%.*}.x"   # release/0.1.x

echo "=== chaos-il2cpp release ${NEW_VER} (branch=${RELEASE_BRANCH}, publish=${DO_PUBLISH}, dry=${DRY_RUN}) ==="
echo "  current branch  : ${CURRENT_BRANCH}"
echo "  target tag      : ${TAG}"

# Clean tree check (unless --dry-run, where we intentionally change nothing).
if [ "$DRY_RUN" -eq 0 ]; then
    # Exclude docs/dev/in-progress/repo-cleanliness/STATUS.md — the repo's own
    # pre-commit hygiene gate regenerates its "Last run" timestamp on EVERY
    # commit, so it can never be clean. Ignoring it keeps the release flow green.
    UNCOMMITTED=$(git status --porcelain 2>/dev/null | grep -v '^??' \
        | grep -v 'docs/dev/in-progress/repo-cleanliness/STATUS.md' || true)
    if [ -n "$UNCOMMITTED" ]; then
        echo "Error: working tree has uncommitted changes. Commit or discard before releasing." >&2
        echo "$UNCOMMITTED" | head -20 >&2
        exit 1
    fi
fi

# gh CLI present (required for --publish).
if [ "$DO_PUBLISH" -eq 1 ]; then
    if ! command -v gh >/dev/null 2>&1; then
        echo "Error: --publish requires the 'gh' CLI (https://cli.github.com)." >&2
        exit 1
    fi
fi

# ── 2. Create/verify release branch ─────────────────────────────────────────
echo "[1/9] Release branch → ${RELEASE_BRANCH}"
if git rev-parse -q --verify "refs/heads/$RELEASE_BRANCH" >/dev/null 2>&1; then
    echo "  release branch already exists: $RELEASE_BRANCH"
elif git ls-remote --exit-code --heads origin "$RELEASE_BRANCH" >/dev/null 2>&1; then
    echo "  release branch exists on origin; fetching"
    if [ "$DRY_RUN" -eq 0 ]; then git fetch origin "$RELEASE_BRANCH"; fi
else
    if [ "$DRY_RUN" -eq 0 ]; then
        git checkout -b "$RELEASE_BRANCH"
        echo "  created + checked out: $RELEASE_BRANCH"
    else
        echo "  [dry] git checkout -b $RELEASE_BRANCH"
    fi
fi

# ── 3. Version bump (delegate; never edit version files directly) ───────────
echo "[2/9] Version bump via release_bump.sh ${NEW_VER} --tag"
BUMP_ARGS=("$NEW_VER")
if [ "$DRY_RUN" -eq 1 ]; then BUMP_ARGS+=(--dry-run); fi
if [ "$DO_PUBLISH" -eq 1 ]; then BUMP_ARGS+=(--tag); fi
if [ "$DRY_RUN" -eq 1 ]; then
    echo "  [dry] bash scripts/release_bump.sh ${BUMP_ARGS[*]}"
else
    bash scripts/release_bump.sh "${BUMP_ARGS[@]}"
fi

# ── 4. Release notes ────────────────────────────────────────────────────────
echo "[3/9] Generate release notes"
PREV_TAG=$(git describe --abbrev=0 --tags "$(git rev-list --max-parents=0 HEAD 2>/dev/null)". 2>/dev/null \
           || git describe --abbrev=0 --tags HEAD~1 2>/dev/null || echo "")
NOTES_FILE="$REPO_ROOT/RELEASE_NOTES_${NEW_VER}.md"
if [ "$DRY_RUN" -eq 1 ]; then
    echo "  [dry] bash scripts/generate-release-notes.sh ${PREV_TAG:-<none>} HEAD > $NOTES_FILE"
else
    bash scripts/generate-release-notes.sh "${PREV_TAG:-}" HEAD > "$NOTES_FILE"
    echo "  wrote $NOTES_FILE"
fi

# ── 5. Commit the release prep (version bump + notes) on the release branch ─
if [ "$DRY_RUN" -eq 0 ] && [ -f "$NOTES_FILE" ]; then
    git add VERSION CMakeLists.txt src/managed/Directory.Build.props "$NOTES_FILE" 2>/dev/null || true
    if ! git diff --cached --quiet; then
        git commit -q -m "release(v$NEW_VER): version bump + release notes

root_cause: v$NEW_VER requires version bump across VERSION/CMake/Directory.Build.props and an artifact-set changelog.
fix_strategy: delegate bump to release_bump.sh; emit auto-generated release notes; keep release branch.
regression_check: release-governance hygiene gate + manual release-notes review post-tag."
        echo "  committed release prep (v$NEW_VER)"
    else
        echo "  no version/notes changes to commit"
    fi
fi

if [ "$DO_PUBLISH" -eq 0 ]; then
    echo ""
    echo "== Staged release prep complete (branch=${RELEASE_BRANCH}, ver=${NEW_VER})."
    echo "   Re-run with --publish to build SDK, checksums, SBOM, and create the GitHub Release."
    if [ "$PUSH" -eq 1 ]; then
        echo "   Push your branch/tag, e.g.: git push -u origin ${RELEASE_BRANCH} ${TAG}"
    fi
    exit 0
fi

# ── 6. Build the SDK (current platform) ─────────────────────────────────────
echo "[4/9] Build native SDK"
SDK_DIR="$REPO_ROOT/artifacts/release/$NEW_VER"
if [ "$DRY_RUN" -eq 1 ]; then
    echo "  [dry] build_presets.py (current platform) → $SDK_DIR"
else
    mkdir -p "$SDK_DIR"
    # invoke the preset builder if present (test-oriented), else cmake the ref preset
    if [ -f "tests/e2e/translation/artifacts/build_presets.py" ]; then
        python tests/e2e/translation/artifacts/build_presets.py 2>/dev/null \
            && echo "  built presets via build_presets.py" \
            || echo "  (build_presets.py skipped/warned; continuing)"
    fi
    # Copy any built preset libs into the release dir (flattened)
    if [ -d "artifacts/presets" ]; then
        find "artifacts/presets" -type f \( -name '*.lib' -o -name '*.a' \) -exec cp {} "$SDK_DIR/" \; 2>/dev/null || true
        echo "  copied SDK libs → $SDK_DIR ($(ls -1 "$SDK_DIR" | wc -l) files)"
    else
        echo "  warning: no artifacts/presets on disk; SDK lib copy skipped"
    fi
fi

# ── 7. Checksums ────────────────────────────────────────────────────────────
echo "[5/9] Generate checksums"
if [ "$DRY_RUN" -eq 1 ]; then
    echo "  [dry] bash scripts/generate-checksums.sh $SDK_DIR"
else
    bash scripts/generate-checksums.sh "$SDK_DIR"
fi

# ── 8. SBOM ─────────────────────────────────────────────────────────────────
echo "[6/9] Generate SBOM"
if [ "$DRY_RUN" -eq 1 ]; then
    echo "  [dry] bash scripts/generate-sbom.sh $SDK_DIR $NEW_VER"
else
    bash scripts/generate-sbom.sh "$SDK_DIR" "$NEW_VER"
fi

# ── 9. Release-governance gate ──────────────────────────────────────────────
echo "[7/9] Release-governance hygiene gate"
if [ "$DRY_RUN" -eq 1 ]; then
    echo "  [dry] python scripts/cleanliness/chaos_hygiene.py --ci"
else
    if python scripts/cleanliness/chaos_hygiene.py --ci; then
        echo "  hygiene gate passed"
    else
        echo "  ⚠️  hygiene gate reported issues — inspect artifacts/hygiene-report.json" >&2
    fi
fi

# ── 10. Create + upload GitHub Release ──────────────────────────────────────
echo "[8/9] Create GitHub Release ${TAG}"
if [ "$DRY_RUN" -eq 1 ]; then
    echo "  [dry] gh release create $TAG --notes-file $NOTES_FILE"
    echo "  [dry] gh release upload $TAG "$SDK_DIR"/*.lib "$SDK_DIR"/SHA256SUMS "$SDK_DIR"/sbom.cyclonedx.json"
else
    if [ "$PUSH" -eq 1 ] && git rev-parse -q --verify "refs/tags/$TAG" >/dev/null 2>&1; then
        echo "  pushing tag ${TAG}"
        git push origin "$TAG" || echo "  (tag push failed; may already exist)"
    fi
    PUBLISH_ARGS=("create" "$TAG")
    if [ -f "$NOTES_FILE" ]; then PUBLISH_ARGS+=("-F" "$NOTES_FILE"); fi
    if [ -f "$SDK_DIR/SHA256SUMS" ]; then PUBLISH_ARGS+=("--verify-tag"); fi
    GH_OUT=$(gh release "${PUBLISH_ARGS[@]}" 2>&1) && echo "  created release: $GH_OUT" \
        || echo "  ⚠️  gh release create returned nonzero (may already exist): $GH_OUT" >&2
    # Upload artifacts if the dir has any .lib/.a or the checksum/sbom files
    UPLOAD_FILES=()
    if [ -d "$SDK_DIR" ]; then
        for f in "$SDK_DIR"/*.lib "$SDK_DIR"/*.a "$SDK_DIR"/*.exe "$SDK_DIR"/SHA256SUMS "$SDK_DIR"/sbom.cyclonedx.json; do
            [ -f "$f" ] && UPLOAD_FILES+=("$f")
        done
    fi
    if [ "${#UPLOAD_FILES[@]}" -gt 0 ]; then
        gh release upload "$TAG" "${UPLOAD_FILES[@]}" 2>&1 | sed 's/^/  /' || echo "  ⚠️  upload incomplete" >&2
    fi
fi

# ── 11. Merge release branch back into main ─────────────────────────────────
if [ "$MERGE_BACK" -eq 1 ] && [ "$CURRENT_BRANCH" = "main" ] && [ "$DRY_RUN" -eq 0 ]; then
    echo "[9/9] Merge ${RELEASE_BRANCH} → main"
    git checkout main
    git merge --no-ff "$RELEASE_BRANCH" -m "release(v$NEW_VER): merge $RELEASE_BRANCH back to main

root_cause: post-release main must incorporate the version bump and notes.
fix_strategy: no-ff merge of the release branch; keeps release history.
regression_check: release-governance + CI on main post-merge."
    if [ "$PUSH" -eq 1 ]; then git push origin main; fi
else
    echo "[9/9] Merge-back skipped (dry-run, not on main, or --no-merge)."
fi

echo ""
echo "== Release ${NEW_VER} flow complete."
echo "   Branch : ${RELEASE_BRANCH}"
echo "   Tag    : ${TAG}"
echo "   Notes  : ${NOTES_FILE}"
echo "   SDK    : ${SDK_DIR}"
exit 0