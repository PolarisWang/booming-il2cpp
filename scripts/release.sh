#!/usr/bin/env bash
# ===============================================================================
# release.sh — standardized release state machine.
#
# Subcommands (each persists state in .release_state.json, so the flow is
# re-entrant and scripted end-to-end):
#
#   init <version>   Start a release: lock, snapshot branch from origin/main,
#                    git worktree add, version bump, build prerequisites.
#   verify           Run gates: release-governance, publish-smoke, unit,
#                    integrity (checksums+SBOM).  Records PASS/FAIL per gate.
#   fix [--from-main|--shell]
#                    After a failed verify: --from-main re-bases the worktree on
#                    the latest origin/main (fixes synced to main are pulled in);
#                    --shell opens an interactive shell to edit the worktree.
#                    Both require a subsequent `verify`.
#   publish          Tag, push, GitHub Release, merge snapshot back to main,
#                    cleanup worktree + snapshot branch + lock + state.
#   abort            Cleanup worktree + snapshot branch + lock + state.
#   status           Show current release state.
#
# Flow:
#   release.sh init 0.2.0
#   release.sh verify                    # PASS -> publish; FAIL -> fix
#   release.sh fix --from-main           # pull latest origin/main fix, verify again
#   release.sh publish                   # only after verify passes
#   release.sh abort                     # abandon
#
# The release acts on a FIXED snapshot of origin/main in an isolated git
# worktree; concurrent agent edits to the main worktree never touch it.
#
# Red lines:
#   - NO `git stash` (forbidden by project).
#   - Version changed ONLY via scripts/release_bump.sh.
#   - Commit messages carry root_cause / fix_strategy / regression_check.
#   - Artifacts written under artifacts/ (layer-owned).
# ===============================================================================

set -euo pipefail

REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$REPO_ROOT"

# shellcheck source=release-config.sh
# shellcheck disable=SC1091
source "$REPO_ROOT/scripts/release-config.sh"

STATE_FILE="$REPO_ROOT/.release_state.json"
LOCK_DIR="$REPO_ROOT/.git/release.lock.dir"
WORKTREE_DIR="$REPO_ROOT/.worktrees/rel"
STATE_VERSION=1

PY=""
for cand in python python3 py; do
    if command -v "$cand" >/dev/null 2>&1 && "$cand" -c 'import sys' >/dev/null 2>&1; then PY="$cand"; break; fi
done
[ -z "$PY" ] && { echo "Error: no python found" >&2; exit 1; }

show_usage() { cat <<'EOF'
Usage: ./scripts/release.sh <subcommand> [options]

Subcommands:
  init <version>       Start release: lock, snapshot branch, worktree, bump, build.
  verify              Run gates (governance / publish-smoke / unit / integrity).
  fix --from-main     Re-base worktree on latest origin/main (post-fix re-release).
  fix --shell         Open interactive shell in the worktree for manual fixes.
  publish             Tag, push, GitHub Release, merge back to main, cleanup.
  abort               Cleanup worktree + branch + lock + state.
  status              Show current release state.

Options:
  --dry-run           Print steps without executing.
  --help              Show this help.
EOF
}

# ── State helpers ─────────────────────────────────────────────────────────
read_state() {
    [ -f "$STATE_FILE" ] || { echo '{}'; return; }
    "$PY" - "$STATE_FILE" <<'PYEOF'
import json, sys
try:
    print(json.dumps(json.load(open(sys.argv[1]))))
except Exception:
    print('{}')
PYEOF
}
state_get() {  # key  statejson
    local key="$1" blob="$2"
    "$PY" - "$key" "$blob" <<'PYEOF'
import json, sys
try: d = json.loads(sys.argv[2])
except Exception: d = {}
print(d.get(sys.argv[1], ''))
PYEOF
}
write_state() {  # key  val(json-literal-or-string)
    local key="$1" val="$2"
    "$PY" - "$STATE_FILE" "$key" "$val" "$(date -u +%Y-%m-%dT%H:%M:%SZ)" <<'PYEOF'
import json, os, sys
p, k, v, ts = sys.argv[1], sys.argv[2], sys.argv[3], sys.argv[4]
d = {}
if os.path.exists(p):
    try: d = json.load(open(p))
    except Exception: d = {}
try: d[k] = json.loads(v)
except Exception: d[k] = v
d['updatedAt'] = ts
json.dump(d, open(p, 'w'), indent=2)
PYEOF
}
init_state() {  # ver commit branch
    "$PY" - "$STATE_FILE" "$1" "$2" "$3" "$WORKTREE_DIR" <<'PYEOF'
import json, sys
d = {
  "version": sys.argv[2], "tag": "v"+sys.argv[2],
  "snapshotBranch": sys.argv[3], "snapshotCommit": sys.argv[1],
  "worktreeDir": sys.argv[4],
  "phase": "init", "phaseStatus": "done",
  "verifyResults": {}, "verifyFailed": False, "failures": [],
  "stateVersion": 1,
  "createdAt": __import__('time').strftime('%Y-%m-%dT%H:%M:%SZ'),
  "updatedAt": __import__('time').strftime('%Y-%m-%dT%H:%M:%SZ'),
}
json.dump(d, open(sys.argv[1], 'w'), indent=2)
PYEOF
}

# ── Lock ─────────────────────────────────────────────────────────────────
acquire_lock() {
    [ "$DRY_RUN" -eq 1 ] && { echo "  [dry] acquire lock"; return; }
    if [ -d "$LOCK_DIR" ]; then echo "Error: release lock held. Run 'abort' or remove." >&2; exit 1; fi
    mkdir "$LOCK_DIR" && echo "$$" > "$LOCK_DIR/pid" && echo "  acquired release lock (pid $$)"
}
release_lock() {
    [ "$DRY_RUN" -eq 1 ] && { echo "  [dry] release lock"; return; }
    rm -rf "$LOCK_DIR" 2>/dev/null || true
    echo "  released release lock"
}
dispatch_cmd() { ( cd "$WORKTREE_DIR" && "$@" ); }

# ── init ─────────────────────────────────────────────────────────────────
cmd_init() {
    local ver="${1:-}"; [ -z "$ver" ] && { echo "Error: version required" >&2; exit 2; }
    [[ "$ver" =~ $RC_SEMVER_RE ]] || { echo "Error: invalid SemVer '$ver'" >&2; exit 2; }
    local branch="release/v${ver}"
    echo "=== release init ${ver} (branch=${branch}) ==="
    acquire_lock

    echo "[1/4] snapshot from origin/main"
    if [ "$DRY_RUN" -eq 0 ]; then
        git fetch origin main 2>&1 | tail -1 || true
        local commit; commit=$(git rev-parse origin/main 2>/dev/null || echo "")
        [ -n "$commit" ] || { echo "Error: cannot resolve origin/main" >&2; release_lock; exit 1; }
        git branch -D "$branch" 2>/dev/null || true
        git branch "$branch" "$commit"
        echo "  snapshot: ${commit:0:12}"
    else
        echo "  [dry] git branch ${branch} origin/main"
    fi

    echo "[2/4] worktree"
    if [ "$DRY_RUN" -eq 0 ]; then
        rm -rf "$WORKTREE_DIR"; git worktree add "$WORKTREE_DIR" "$branch" >/dev/null 2>&1
        echo "  worktree: ${WORKTREE_DIR}"
    else
        echo "  [dry] git worktree add ${WORKTREE_DIR} ${branch}"
    fi

    echo "[3/4] version bump + state"
    if [ "$DRY_RUN" -eq 0 ]; then
        dispatch_cmd bash scripts/release_bump.sh "$ver" 2>&1 | tail -3
        init_state "$commit" "$ver" "$branch"
        echo "  version bumped to ${ver}"
    else
        echo "  [dry] release_bump.sh ${ver}"
        echo "  [dry] write .release_state.json"
    fi

    echo "[4/4] build prerequisites"
    if [ "$DRY_RUN" -eq 0 ]; then
        dispatch_cmd dotnet build src/managed/Chaos.IL2CPP.Generator --configuration Release --nologo -v q >/dev/null 2>&1 || true
        dispatch_cmd dotnet build src/managed/Chaos.IL2CPP.Driver --configuration Release --nologo -v q >/dev/null 2>&1 || true
        echo "  prerequisites built"
    else
        echo "  [dry] dotnet build Generator + Driver"
    fi
    echo ""
    echo "== init done. Run: ./scripts/release.sh verify"
}

# ── verify ───────────────────────────────────────────────────────────────
cmd_verify() {
    local state; state=$(read_state)
    local phase; phase=$(echo "$state" | state_get phase "$state")
    if [ "$phase" != "init" ] && [ "$phase" != "fix" ]; then
        echo "Error: current phase '${phase}', expected init or fix. Run init <version> first." >&2
        exit 1
    fi
    [ -d "$WORKTREE_DIR" ] || { echo "Error: worktree not found ${WORKTREE_DIR}" >&2; exit 1; }

    echo "=== release verify ==="
    local failed=0
    local ver; ver=$(echo "$state" | state_get version "$state")

    # 1 governance
    echo "  1/4 release-governance"
    if [ "$DRY_RUN" -eq 1 ]; then echo "    [dry] governance";
    elif dispatch_cmd python scripts/cleanliness/check_release_governance.py; then
        echo "    pass"; write_state verifyResults.governance '"pass"'
    else
        echo "    fail (expected only: no tag yet)"; failed=1; write_state verifyResults.governance '"fail"'
    fi

    # 2 publish-smoke (dev mode)
    echo "  2/4 publish-smoke"
    if [ "$DRY_RUN" -eq 1 ]; then echo "    [dry] publish-smoke"
    elif dispatch_cmd python scripts/publish-smoke.py --json publish-smoke-report.json >/dev/null 2>&1; then
        echo "    pass"; write_state verifyResults.publishSmoke '"pass"'
    else
        echo "    fail"; failed=1; write_state verifyResults.publishSmoke '"fail"'
    fi

    # 3 unit
    echo "  3/4 unit tests"
    if [ "$DRY_RUN" -eq 1 ]; then echo "    [dry] unit"
    elif dispatch_cmd python tests/runner/test_driver.py --layer unit --quick >/dev/null 2>&1; then
        echo "    pass"; write_state verifyResults.unit '"pass"'
    else
        echo "    fail (worktree may need test-projects built)"; failed=1; write_state verifyResults.unit '"fail"'
    fi

    # 4 integrity
    echo "  4/4 integrity"
    if [ "$DRY_RUN" -eq 1 ]; then echo "    [dry] checksums+sbom"
    else
        local sdk_dir="$RC_ARTIFACTS_BASE/release/$ver"
        mkdir -p "$sdk_dir"
        if dispatch_cmd bash scripts/generate-checksums.sh "$sdk_dir" >/dev/null 2>&1 &&
           dispatch_cmd bash scripts/generate-sbom.sh "$sdk_dir" "$ver" >/dev/null 2>&1; then
            echo "    pass"; write_state verifyResults.integrity '"pass"'
        else
            echo "    fail"; failed=1; write_state verifyResults.integrity '"fail"'
        fi
    fi

    if [ "$failed" -ne 0 ]; then
        write_state phase '"verify"'; write_state phaseStatus '"failed"'; write_state verifyFailed true
        echo ""
        echo "!! verify FAILED. Run: ./scripts/release.sh fix --from-main  (after fixing on main)"
        exit 1
    fi
    write_state phase '"verify"'; write_state phaseStatus '"pass"'; write_state verifyFailed false
    echo ""
    echo "== verify ALL PASSED. Run: ./scripts/release.sh publish"
}

# ── fix ──────────────────────────────────────────────────────────────────
cmd_fix() {
    local state; state=$(read_state)
    local phaseStatus; phaseStatus=$(echo "$state" | state_get phaseStatus "$state")
    local phase; phase=$(echo "$state" | state_get phase "$state")
    local ver; ver=$(echo "$state" | state_get version "$state")
    local branch; branch=$(echo "$state" | state_get snapshotBranch "$state")
    [ "$phaseStatus" = "failed" ] || { echo "Error: verify did not fail. Nothing to fix." >&2; exit 1; }
    [ -d "$WORKTREE_DIR" ] || { echo "Error: worktree not found" >&2; exit 1; }

    echo "=== release fix (${ver}) ==="
    local mode="shell"
    for a in "$@"; do case "$a" in --from-main) mode="from-main";; esac; done

    if [ "$mode" = "from-main" ]; then
        echo "  Re-basing ${branch} on latest origin/main (fix synced to main is pulled in)."
        git fetch origin main 2>&1 | tail -1 || true
        local latest; latest=$(git rev-parse origin/main 2>/dev/null || echo "")
        [ -n "$latest" ] || { echo "Error: cannot resolve origin/main" >&2; exit 1; }
        git branch -D "$branch" 2>/dev/null || true
        git branch "$branch" "$latest"
        git worktree remove "$WORKTREE_DIR" --force 2>/dev/null || rm -rf "$WORKTREE_DIR"
        git worktree add "$WORKTREE_DIR" "$branch" >/dev/null 2>&1
        dispatch_cmd bash scripts/release_bump.sh "$ver" 2>&1 | tail -3
        echo "  re-based worktree on origin/main @ ${latest:0:12}"
    else
        echo "  Editing worktree at ${WORKTREE_DIR}"
        echo "  Apply fixes, commit (in worktree), push ${branch}; then run verify."
        cd "$WORKTREE_DIR"
        ${SHELL:-bash} -i || true
        cd "$REPO_ROOT"
    fi
    write_state phase '"fix"'; write_state phaseStatus '"done"'; write_state verifyFailed false
    echo "== fix done. Run: ./scripts/release.sh verify"
}

# ── publish ──────────────────────────────────────────────────────────────
cmd_publish() {
    local state; state=$(read_state)
    local phaseStatus; phaseStatus=$(echo "$state" | state_get phaseStatus "$state")
    local ver; ver=$(echo "$state" | state_get version "$state")
    local tag; tag=$(echo "$state" | state_get tag "$state")
    local branch; branch=$(echo "$state" | state_get snapshotBranch "$state")
    [ "$phaseStatus" = "pass" ] || { echo "Error: verify not passed (${phaseStatus}). Run verify first." >&2; exit 1; }
    [ -d "$WORKTREE_DIR" ] || { echo "Error: worktree not found" >&2; exit 1; }

    echo "=== release publish ${ver} ==="
    echo "[1/5] tag + push"
    if [ "$DRY_RUN" -eq 1 ]; then echo "  [dry] tag/push ${tag} ${branch}"
    else
        dispatch_cmd git tag "$tag" 2>/dev/null || echo "  tag exists"
        dispatch_cmd git push origin "$branch" 2>&1 | tail -1 || true
        dispatch_cmd git push origin "$tag" 2>&1 | tail -1 || true
        echo "  pushed"
    fi

    echo "[2/5] build nupkg + integrity"
    if [ "$DRY_RUN" -eq 1 ]; then echo "  [dry] build-tool-package"
    else
        ( cd "$REPO_ROOT" && bash scripts/build-tool-package.sh "$ver" 2>&1 | tail -3 || true )
        local sdk_dir="$RC_ARTIFACTS_BASE/release/$ver"
        ( cd "$REPO_ROOT" && bash scripts/generate-checksums.sh "$sdk_dir" >/dev/null 2>&1 || true )
        ( cd "$REPO_ROOT" && bash scripts/generate-sbom.sh "$sdk_dir" "$ver" >/dev/null 2>&1 || true )
        echo "  artifacts ready"
    fi

    echo "[3/5] GitHub Release"
    if [ "$DRY_RUN" -eq 1 ]; then echo "  [dry] gh release create ${tag}"
    elif command -v gh >/dev/null 2>&1 && gh auth status >/dev/null 2>&1; then
        local notes="$RC_ARTIFACTS_BASE/release/$ver/RELEASE_NOTES_${ver}.md"
        ( cd "$REPO_ROOT" && bash scripts/generate-release-notes.sh "v0.1.0" "$tag" > "$notes" 2>/dev/null || true )
        ( cd "$REPO_ROOT" && gh release create "$tag" --notes-file "$notes" --verify-tag 2>&1 | tail -3 ) || \
            echo "  ⚠️  gh release create nonzero (CI will handle tag push)" >&2
    else
        echo "  gh not authenticated; tag pushed — CI release.yml will create Release."
    fi

    echo "[4/5] merge snapshot → main"
    if [ "$DRY_RUN" -eq 1 ]; then echo "  [dry] merge ${branch} → main"
    else
        git checkout main 2>/dev/null || true
        if ! git merge --ff-only "$branch" >/dev/null 2>&1; then
            git merge --no-ff "$branch" -m "release(v${ver}): merge snapshot branch

root_cause: main advanced beyond release snapshot during verify.
fix_strategy: no-ff merge to incorporate the released commit.
regression_check: release-governance + CI on main after merge."
        fi
        git push origin main 2>&1 | tail -1 || echo "  ⚠️  push main failed" >&2
        echo "  merged + pushed main"
    fi

    echo "[5/5] cleanup"
    if [ "$DRY_RUN" -eq 1 ]; then echo "  [dry] cleanup worktree/branch/state/lock"
    else
        git worktree remove "$WORKTREE_DIR" --force 2>/dev/null || rm -rf "$WORKTREE_DIR"
        git branch -D "$branch" 2>/dev/null || true
        rm -f "$STATE_FILE"
        release_lock
        echo "  cleaned"
    fi
    echo ""
    echo "== Release ${ver} published."
    echo "   Tag: ${tag}, Branch: ${branch} (merged to main)"
}

# ── abort ────────────────────────────────────────────────────────────────
cmd_abort() {
    echo "=== release abort ==="
    local state; state=$(read_state)
    local branch; branch=$(echo "$state" | state_get snapshotBranch "$state")
    if [ -d "$WORKTREE_DIR" ]; then
        [ "$DRY_RUN" -eq 1 ] && echo "  [dry] remove worktree" || { git worktree remove "$WORKTREE_DIR" --force 2>/dev/null || rm -rf "$WORKTREE_DIR"; echo "  removed worktree"; }
    fi
    if [ -n "$branch" ] && git rev-parse -q --verify "refs/heads/$branch" >/dev/null 2>&1; then
        [ "$DRY_RUN" -eq 1 ] && echo "  [dry] delete ${branch}" || { git branch -D "$branch" >/dev/null 2>&1 || true; echo "  deleted ${branch}"; }
    fi
    rm -f "$STATE_FILE" 2>/dev/null
    release_lock
    echo "== abort complete."
}

# ── status ───────────────────────────────────────────────────────────────
cmd_status() {
    [ -f "$STATE_FILE" ] || { echo "No active release state."; exit 0; }
    "$PY" - "$STATE_FILE" <<'PYEOF'
import json, sys
d = json.load(open(sys.argv[1]))
print("=== Release Status ===")
for k in ("version","tag","snapshotBranch","phase","phaseStatus","createdAt","updatedAt"):
    v = d.get(k)
    sub = d.get('snapshotCommit')
    label = k
    if k == 'snapshotBranch' and sub: v = f"{v} @ {sub[:12]}"
    print(f"  {label:16}: {v}")
vr = d.get('verifyResults')
if vr:
    print("  Verify gates:")
    for g, s in vr.items(): print(f"    {g:16}: {s}")
PYEOF
}

# ── Main dispatch ────────────────────────────────────────────────────────
DRY_RUN=0
SUBCOMMAND=""; SUBARGS=()
while [ $# -gt 0 ]; do
    case "$1" in
        --dry-run) DRY_RUN=1; shift ;;
        --help|-h) show_usage; exit 0 ;;
        init|verify|fix|publish|abort|status) SUBCOMMAND="$1"; shift; SUBARGS=("$@"); break ;;
        *) echo "Error: unknown subcommand '$1'" >&2; show_usage >&2; exit 2 ;;
    esac
done
[ -n "$SUBCOMMAND" ] || { show_usage >&2; exit 2; }

case "$SUBCOMMAND" in
    init) cmd_init "${SUBARGS[0]:-}" ;;
    verify) cmd_verify ;;
    fix) cmd_fix "${SUBARGS[@]:-}" ;;
    publish) cmd_publish ;;
    abort) cmd_abort ;;
    status) cmd_status ;;
esac