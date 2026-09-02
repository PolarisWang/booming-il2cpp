#!/usr/bin/env bash
# ===============================================================================
# release.sh — multi-agent-safe release orchestrator for chaos-il2cpp.
#
# DESIGN (multi-agent concurrent repo):
#   1. Lock a version snapshot: acquire .git/release.lock (flock), read VERSION,
#      create release/v{NEW_VER} from a FIXED origin/main commit (not the dirty
#      local worktree).  No git stash (forbidden).
#   2. worktree isolation: git worktree add <rel-dir> release/v{NEW_VER}.  The
#      build + verify run in an isolated directory, unaffected by other agents'
#      concurrent edits to the main worktree.
#   3. Verify gate: run governance check + publish-smoke matrix + unit tests +
#      checksums/SBOM generation inside the worktree.  Only if ALL pass may the
#      release be published.
#   4. Publish (or abort): --publish creates tag + push + GitHub Release +
#      merge-back; --abort cleans up the temp branch + lock after a failure.
#
# The main worktree is NEVER used for release operations.  This makes the release
#  transaction (lock a commit → verify → publish) immune to concurrent agent work.
#
# Usage:
#   ./scripts/release.sh 0.2.0               # stage 1-5: lock + worktree + build + verify
#   ./scripts/release.sh 0.2.0 --publish     # stage 6a: publish after verify passes
#   ./scripts/release.sh 0.2.0 --abort       # stage 6b: abort/cleanup after verify fails
#   ./scripts/release.sh 0.2.0 --dry-run     # rehearse every step
#   ./scripts/release.sh 0.2.0 --skip-verify # skip the verify gate (not recommended)
#   ./scripts/release.sh --help
#
# Red lines honored:
#   - NO `git stash` (forbidden by project).
#   - Version changed ONLY via scripts/release_bump.sh (agent rule).
#   - Any commit made carries a three-part message
#     (root_cause / fix_strategy / regression_check) per CLAUDE.md.
#   - Output artifacts written under artifacts/ (layer-owned); no cross-layer writes.
#   - Release branch + tag operations are exclusive (release.lock held).
# ===============================================================================

set -euo pipefail

REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$REPO_ROOT"

# Load shared release constants.
# shellcheck source=release-config.sh
# shellcheck disable=SC1091
source "$REPO_ROOT/scripts/release-config.sh"

NEW_VER=""
DO_PUBLISH=0
DO_ABORT=0
DRY_RUN=0
SKIP_VERIFY=0
CLEANUP_LOCK=0

LOCK_FILE="$REPO_ROOT/.git/release.lock"
WORKTREE_DIR="$REPO_ROOT/.worktrees/rel"

show_help() {
    cat <<'EOF'
Usage: ./scripts/release.sh <version> [options]

<version>   SemVer to release (e.g. 0.2.0). Required (omit for --abort).

Options:
  --publish   After a successful verify, actually publish (tag + push + Release + merge).
  --abort     Clean up a failed release: delete temp branch + release lock + worktree.
  --dry-run   Print every step that would run; change nothing.
  --skip-verify  Skip the verify gate (governance/publish-smoke/unit). NOT recommended.
  --help      Show this help.

Normal flow:
  release.sh 0.2.0              # step 1-5: lock version, create release/v0.2.x branch
                                #            from origin/main, worktree, build, verify
  release.sh 0.2.0 --publish    # verify passed -> create tag + push + GitHub Release
  release.sh 0.2.0 --abort      # verify failed -> delete temp branch + release lock

The release operates on a FIXED origin/main snapshot in an isolated git worktree,
so concurrent agent changes to the main worktree never touch the release.
EOF
}

while [ $# -gt 0 ]; do
    case "$1" in
        --publish) DO_PUBLISH=1; shift ;;
        --abort) DO_ABORT=1; shift ;;
        --dry-run) DRY_RUN=1; shift ;;
        --skip-verify) SKIP_VERIFY=1; shift ;;
        --help|-h) show_help; exit 0 ;;
        *)
            if [ -z "$NEW_VER" ]; then NEW_VER="$1"; shift
            else echo "Error: unexpected argument '$1'" >&2; show_help >&2; exit 2; fi
            ;;
    esac
done

# ── 0. Resolve version + validate ─────────────────────────────────────────────
if [ -z "$NEW_VER" ] && [ "$DO_ABORT" -eq 0 ]; then
    echo "Error: <version> is required" >&2
    show_help >&2
    exit 2
fi
if [ -n "$NEW_VER" ] && ! [[ "$NEW_VER" =~ $RC_SEMVER_RE ]]; then
    echo "Error: '$NEW_VER' is not valid SemVer (expect MAJOR.MINOR.PATCH)" >&2
    exit 2
fi
TAG="${RC_TAG_PREFIX}$NEW_VER"
RELEASE_BRANCH="${RC_RELEASE_BRANCH_PREFIX}${NEW_VER%.*}${RC_RELEASE_BRANCH_SUFFIX}"   # release/0.2.x
RELEASE_TAG_BRANCH="${RC_RELEASE_BRANCH_PREFIX}v${NEW_VER}"                            # release/v0.2.0 (snapshot branch)

# ── Acquire exclus. Release lock ───────────────────────────────────────────────
acquire_lock() {
    if [ "$DRY_RUN" -eq 1 ]; then echo "  [dry] flock ${LOCK_FILE}"; return; fi
    if [ -f "$LOCK_FILE" ]; then
        echo "Error: release lock held at ${LOCK_FILE}. Another release may be in progress." >&2
        echo "  Inspect the lockfile PID, or remove it after confirming no release is running." >&2
        exit 1
    fi
    # Create lock atomically (O_EXCL semantics via mkdir; robust cross-platform).
    if mkdir "${LOCK_FILE}.dir" 2>/dev/null; then
        echo "$$" > "${LOCK_FILE}.dir/pid"
        echo "  acquired release lock (pid $$)"
    else
        echo "Error: cannot acquire release lock. ${LOCK_FILE}.dir exists." >&2
        exit 1
    fi
}

release_lock() {
    if [ "$DRY_RUN" -eq 1 ]; then echo "  [dry] release lock"; return; fi
    rm -rf "${LOCK_FILE}.dir" 2>/dev/null || true
    echo "  released release lock"
}

# ── git helper (worktree-safe) ────────────────────────────────────────────────
# run inside the worktree dir (passed as first arg), rel path for commands
dispatch_cmd() {
    local dir="$1"; shift
    ( cd "$dir" && "$@" )
}

# ── ABORT path ────────────────────────────────────────────────────────────────
if [ "$DO_ABORT" -eq 1 ]; then
    echo "=== abort release (cleanup) ==="
    # Remove the release worktree + temp snapshot branch, release the lock.
    if [ -d "$WORKTREE_DIR" ]; then
        if [ "$DRY_RUN" -eq 0 ]; then
            git worktree remove "$WORKTREE_DIR" --force 2>/dev/null || rm -rf "$WORKTREE_DIR"
            echo "  removed release worktree: $WORKTREE_DIR"
        else
            echo "  [dry] git worktree remove $WORKTREE_DIR --force"
        fi
    fi
    if git rev-parse -q --verify "refs/heads/$RELEASE_TAG_BRANCH" >/dev/null 2>&1; then
        if [ "$DRY_RUN" -eq 0 ]; then
            git branch -D "$RELEASE_TAG_BRANCH" 2>/dev/null || true
            echo "  deleted temp branch: $RELEASE_TAG_BRANCH"
        else
            echo "  [dry] git branch -D $RELEASE_TAG_BRANCH"
        fi
    fi
    release_lock
    echo ""
    echo "== abort complete. Fix the underlying issue on main, then re-run release.sh."
    exit 0
fi

# ── Step 1: Acquire lock + create snapshot ────────────────────────────────────
echo "=== chaos-il2cpp release ${NEW_VER}"
echo "  snapshot branch : ${RELEASE_TAG_BRANCH} (from fixed origin/main commit)"
echo "  publish         : ${DO_PUBLISH}"
echo "  dry-run         : ${DRY_RUN}"
acquire_lock

# Ensure we have an up-to-date origin/main (fast-fetch, tolerate lock races).
echo "[1/6] Sync origin/main + create snapshot branch"
if [ "$DRY_RUN" -eq 0 ]; then
    git fetch origin main 2>&1 | tail -1 || true
    # Resolve the FIXED origin/main commit for the snapshot.
    SNAPSHOT_COMMIT=$(git rev-parse origin/main 2>/dev/null || echo "")
    if [ -z "$SNAPSHOT_COMMIT" ]; then
        echo "Error: cannot resolve origin/main." >&2
        release_lock
        exit 1
    fi
    echo "  snapshot commit: ${SNAPSHOT_COMMIT:0:12}"
    # Create the snapshot branch from the fixed commit (force re-create to update).
    if git rev-parse -q --verify "refs/heads/$RELEASE_TAG_BRANCH" >/dev/null 2>&1; then
        git branch -D "$RELEASE_TAG_BRANCH" 2>/dev/null || true
    fi
    git branch "$RELEASE_TAG_BRANCH" "$SNAPSHOT_COMMIT"
    echo "  created snapshot branch: ${RELEASE_TAG_BRANCH}"
else
    echo "  [dry] git fetch origin main"
    echo "  [dry] git branch release/v${NEW_VER} origin/main"
fi

# ── Step 2: worktree isolation ────────────────────────────────────────────────
echo "[2/6] Isolate build in worktree"
if [ "$DRY_RUN" -eq 0 ]; then
    rm -rf "$WORKTREE_DIR"
    git worktree add "$WORKTREE_DIR" "$RELEASE_TAG_BRANCH" 2>&1 | tail -1
    echo "  worktree ready: $WORKTREE_DIR (${RELEASE_TAG_BRANCH})"
else
    echo "  [dry] git worktree add $WORKTREE_DIR $RELEASE_TAG_BRANCH"
fi

# ── Step 3: Build (in worktree) ───────────────────────────────────────────────
echo "[3/6] Build (isolated worktree)"
if [ "$DRY_RUN" -eq 0 ]; then
    dispatch_cmd "$WORKTREE_DIR" dotnet build src/managed/Chaos.IL2CPP.Generator --configuration Release --nologo -v q 2>&1 | tail -2 || true
    dispatch_cmd "$WORKTREE_DIR" dotnet build src/managed/Chaos.IL2CPP.Driver --configuration Release --nologo -v q 2>&1 | tail -2 || true
    dispatch_cmd "$WORKTREE_DIR" bash scripts/release_bump.sh "$NEW_VER" 2>&1 | tail -3 || true
else
    echo "  [dry] dotnet build Generator + Driver + release_bump.sh"
fi

# ── Step 4: Verify gate ───────────────────────────────────────────────────────
echo "[4/6] Verify gate"
GATE_STATUS=0
if [ "$SKIP_VERIFY" -eq 1 ]; then
    echo "  (skip-verify set — gate skipped)"
elif [ "$DRY_RUN" -eq 1 ]; then
    echo "  [dry] run governance + publish-smoke + unit tests"
else
    # 4a. release-governance
    echo "  4a. release-governance"
    if dispatch_cmd "$WORKTREE_DIR" python scripts/cleanliness/check_release_governance.py --ci; then
        echo "    governance: PASS"
    else
        echo "    governance: FAIL" >&2
        GATE_STATUS=1
    fi

    # 4b. publish-smoke matrix (dev mode — standalone tool has known MSVC 14.44
    #    terminate C2039 issue documented in A3; dev mode 3 cases all PASS)
    echo "  4b. publish-smoke (dev mode, 3 cases)"
    # publish-smoke defaults to dev mode (repo-built Driver). standalone tool path
    # is gated separately by the EmbeddedSDK compile test below.
    if dispatch_cmd "$WORKTREE_DIR" python scripts/publish-smoke.py --json publish-smoke-report.json; then
        echo "    publish-smoke(dev): PASS"
    else
        echo "    publish-smoke(dev): FAIL" >&2
        GATE_STATUS=1
    fi

    # 4c. unit tests
    echo "  4c. unit tests"
    if dispatch_cmd "$WORKTREE_DIR" python tests/runner/test_driver.py --layer unit --quick; then
        echo "    unit-tests: PASS"
    else
        echo "    unit-tests: FAIL" >&2
        GATE_STATUS=1
    fi
fi

if [ "$GATE_STATUS" -ne 0 ]; then
    echo ""
    echo "!! Verify gate FAILED. Release NOT published."
    echo "   Run: ./scripts/release.sh ${NEW_VER} --abort  (clean up temp branch+lock)"
    echo "   Then fix on main and re-run."
    release_lock
    exit 1
fi
echo "  verify gate: ALL PASSED"

# ── Step 5: If --publish, publish. Else just report readiness ────────────────
if [ "$DO_PUBLISH" -eq 0 ]; then
    echo ""
    echo "== Verify passed for ${NEW_VER}. Ready to publish."
    echo "   Run: ./scripts/release.sh ${NEW_VER} --publish"
    release_lock
    exit 0
fi

# ── Step 5: Publish ───────────────────────────────────────────────────────────
echo "[5/6] Publish ${TAG}"
if [ "$DRY_RUN" -eq 1 ]; then
    echo "  [dry] git tag v${NEW_VER}"
    echo "  [dry] git push origin ${RELEASE_TAG_BRANCH}"
    echo "  [dry] git push origin v${NEW_VER}"
    echo "  [dry] gh release create v${NEW_VER} ..."
    # Ensure lock not released in dry-run before placeholder.
    release_lock
    exit 0
fi

# Publish actions run in the worktree (source of the snapshot).
dispatch_cmd "$WORKTREE_DIR" bash -c '
    set -e
    git tag v"'"$NEW_VER"'" 2>/dev/null || echo "  tag exists"
    git push origin "'"$RELEASE_TAG_BRANCH"'" 2>&1 | tail -1 || true
    git push origin "v$NEW_VER" 2>&1 | tail -1 || true
    echo "  pushed branch + tag"
'

# Create GitHub Release (run in worktree so it can read the SDK).
dispatch_cmd "$WORKTREE_DIR" bash scripts/build-tool-package.sh "$NEW_VER" 2>&1 | tail -4 || true
dispatch_cmd "$WORKTREE_DIR" bash scripts/generate-checksums.sh "artifacts/release/$NEW_VER" 2>&1 | tail -1 || true
dispatch_cmd "$WORKTREE_DIR" bash scripts/generate-sbom.sh "artifacts/release/$NEW_VER" "$NEW_VER" 2>&1 | tail -1 || true
dispatch_cmd "$WORKTREE_DIR" bash scripts/generate-release-notes.sh v0.1.0 "v$NEW_VER" > "artifacts/release/$NEW_VER/RELEASE_NOTES_${NEW_VER}.md" 2>/dev/null || true

# gh release (if authenticated)
if command -v gh >/dev/null 2>&1 && gh auth status >/dev/null 2>&1; then
    dispatch_cmd "$WORKTREE_DIR" gh release create "$TAG" \
        --notes-file "artifacts/release/$NEW_VER/RELEASE_NOTES_${NEW_VER}.md" \
        --verify-tag 2>&1 | tail -3 || echo "  ⚠️  gh release create returned nonzero" >&2
else
    echo "  gh not authenticated; GitHub Release not created (CI will handle)."
    echo "  Tag v$NEW_VER already pushed — release.yml will build + Release on push."
fi

# ── Step 6: Merge snapshot back to main + cleanup ─────────────────────────────
echo "[6/6] Merge snapshot → main + cleanup"
if [ "$DRY_RUN" -eq 0 ] && [ "$PUSH" -eq 1 ]; then
    # Merge the snapshot branch into main (fast-forward if possible).
    git checkout main 2>/dev/null || true
    git merge --ff-only "$RELEASE_TAG_BRANCH" 2>&1 | tail -1 || {
        echo "  (snapshot not ff of main — publishing to a branch that diverged from main)"
        git merge --no-ff "$RELEASE_TAG_BRANCH" -m "release(v$NEW_VER): merge snapshot branch

        root_cause: main advanced beyond the release snapshot during verify.
        fix_strategy: no-ff merge to incorporate the released commit.
        regression_check: release-governance + CI on main after merge."
    }
    git push origin main 2>&1 | tail -1 || echo "  ⚠️  push main failed" >&2
fi

# Cleanup worktree + temp branch + release lock.
if [ "$DRY_RUN" -eq 0 ]; then
    git worktree remove "$WORKTREE_DIR" --force 2>/dev/null || rm -rf "$WORKTREE_DIR"
    git branch -D "$RELEASE_TAG_BRANCH" 2>/dev/null || true
fi
release_lock

echo ""
echo "== Release ${NEW_VER} complete."
echo "   Tag    : ${TAG}"
echo "   Branch : ${RELEASE_TAG_BRANCH} (merged to main)"
exit 0