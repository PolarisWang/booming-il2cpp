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
init_state() {  # ver commit branch worktreeDir
    "$PY" - "$STATE_FILE" "$1" "$2" "$3" "$WORKTREE_DIR" <<'PYEOF'
import json, sys
d = {
  "version": sys.argv[2],
  "tag": "v" + sys.argv[2],
  "snapshotBranch": sys.argv[4],
  "snapshotCommit": sys.argv[3],
  "worktreeDir": sys.argv[5],
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

# Provision prebuilt SDK libs from the main worktree into the release worktree.
# The release worktree is a clean checkout; the native SDK libs (Windows .lib /
# Linux .a) live in the MAIN repo under gitignored tree tests/e2e/translation/sdk
# and are needed at link time by publish-smoke's CopyRealSdkLibsOverStubs
# (which reads repoRoot-relative paths).  Without them the publish link fails
# with LNK1107 on 8-byte stub archives.  Uses cp -r (Windows symlinks unreliable).
provision_worktree_sdk() {
    echo "  provisioning prebuilt SDK into worktree"
    local wt="$WORKTREE_DIR"
    local sdk_dir_src="$REPO_ROOT/tests/e2e/translation/sdk"
    local sdk_dir_dst="$wt/tests/e2e/translation/sdk"
    if [ -d "$sdk_dir_src" ] && [ ! -d "$sdk_dir_dst" ]; then
        mkdir -p "$wt/tests/e2e/translation"
        cp -r "$sdk_dir_src" "$sdk_dir_dst" 2>/dev/null && echo "    copied sdk lib tree" || cp "$sdk_dir_src"/*.lib "$sdk_dir_dst" 2>/dev/null || true
    fi
    # artifacts/release dir needed by checksums/sbom integrity
    if [ ! -d "$wt/artifacts" ]; then mkdir -p "$wt/artifacts" "$REPO_ROOT/artifacts"; fi
    if [ -d "$REPO_ROOT/artifacts/release" ] && [ ! -e "$wt/artifacts/release" ]; then
        ln -sfn "$REPO_ROOT/artifacts/release" "$wt/artifacts/release" 2>/dev/null || cp -r "$REPO_ROOT/artifacts/release" "$wt/artifacts/release" 2>/dev/null || true
    fi
}

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

    echo "[3/4] version bump + commit + state"
    if [ "$DRY_RUN" -eq 0 ]; then
        dispatch_cmd bash scripts/release_bump.sh "$ver" 2>&1 | tail -3
        # Commit the bump so the snapshot branch / tag / merge actually carry
        # the new version.  Without this commit the version bump would live only
        # as an uncommitted worktree edit that is destroyed on cleanup, tagging
        # a tree whose VERSION still says the previous release.
        local bump_rc=0
        dispatch_cmd git add VERSION CMakeLists.txt src/managed/Directory.Build.props || bump_rc=1
        if [ "$bump_rc" -eq 0 ] && dispatch_cmd git diff --cached --quiet; then
            echo "  (no version-file diff to commit — already at ${ver})"
            commit=$(git rev-parse "$branch")
        else
            dispatch_cmd git commit -m "chore(release): bump version to ${ver}

root_cause: version bump was never committed into the snapshot branch, so the
  release tag pointed at a tree whose VERSION still read the previous version.
fix_strategy: commit VERSION/CMakeLists/Directory.Build.props on the snapshot
  branch before verify/publish so tag+merge carry the new version.
regression_check: release-governance on origin/main + tag resolves VERSION=${ver}." >/dev/null 2>&1
            commit=$(git rev-parse "$branch")
            echo "  version bumped to ${ver} (commit ${commit:0:12})"
        fi
        init_state "$ver" "$commit" "$branch"
    else
        echo "  [dry] release_bump.sh ${ver} + commit version files"
        echo "  [dry] write .release_state.json"
    fi

    echo "[4/4] build prerequisites + SDK provision"
    if [ "$DRY_RUN" -eq 0 ]; then
        dispatch_cmd dotnet build src/managed/Chaos.IL2CPP.Generator --configuration Release --nologo -v q >/dev/null 2>&1 || true
        dispatch_cmd dotnet build src/managed/Chaos.IL2CPP.Driver --configuration Release --nologo -v q >/dev/null 2>&1 || true
        provision_worktree_sdk
        echo "  prerequisites built"
    else
        echo "  [dry] dotnet build Generator + Driver + provision SDK"
    fi
    echo ""
    echo "== init done. Run: ./scripts/release.sh verify"
}

# ── verify ───────────────────────────────────────────────────────────────
cmd_verify() {
    local state; state=$(read_state)
    local phase; phase=$(echo "$state" | state_get phase "$state")
    if [ "$phase" != "init" ] && [ "$phase" != "fix" ] && [ "$phase" != "verify" ]; then
        echo "Error: current phase '${phase}', expected init or fix. Run init <version> first." >&2
        exit 1
    fi
    [ -d "$WORKTREE_DIR" ] || { echo "Error: worktree not found ${WORKTREE_DIR}" >&2; exit 1; }

    echo "=== release verify ==="
    local failed=0
    local ver; ver=$(echo "$state" | state_get version "$state")

    # 1 governance
    echo "  1/5 release-governance"
    if [ "$DRY_RUN" -eq 1 ]; then echo "    [dry] governance";
    elif dispatch_cmd python scripts/cleanliness/check_release_governance.py; then
        echo "    pass"; write_state verifyResults.governance '"pass"'
    else
        echo "    fail (expected only: no tag yet)"; failed=1; write_state verifyResults.governance '"fail"'
    fi

    # 2 publish-smoke (dev mode)
    echo "  2/5 publish-smoke"
    if [ "$DRY_RUN" -eq 1 ]; then echo "    [dry] publish-smoke"
    elif dispatch_cmd python scripts/publish-smoke.py --json publish-smoke-report.json >/dev/null 2>&1; then
        echo "    pass"; write_state verifyResults.publishSmoke '"pass"'
    else
        echo "    fail"; failed=1; write_state verifyResults.publishSmoke '"fail"'
    fi

    # 3 unit: dotnet test each project individually (test_driver infra expects
    # trx produced by a pre-built bin; worktree is a clean checkout).
    echo "  3/5 unit tests"
    if [ "$DRY_RUN" -eq 1 ]; then echo "    [dry] unit"
    else
        # Run each test project in its own foreground subshell with an explicit
        # timeout.  A bare `dispatch_cmd dotnet test ... >/dev/null` under
        # `set -euo pipefail` can terminate the whole script when the MSBuild
        # build server tears down between invocations, so we isolate each call,
        # disable node reuse to avoid VBCSCompiler lingering, and stream output
        # (not pipe-discard) so a slow/hung test is visible in the log.
        local unit_pass=0 unit_rc=0
        for path in \
            "tests/unit/managed/codegen/Chaos.IL2CPP.CodeGen.Tests.csproj" \
            "tests/unit/managed/driver/Chaos.IL2CPP.Driver.Tests.csproj" \
            "tests/unit/managed/snapshot/Chaos.IL2CPP.CodeGen.SnapshotTests.csproj"; do
            if (
                cd "$WORKTREE_DIR"
                export MSBUILDDISABLENODEREUSE=1 DOTNET_CLI_TELEMETRY_OPTOUT=1 DOTNET_NOLOGO=1
                timeout "${RC_DOTNET_TEST_TIMEOUT:-300}" dotnet test "$path" --nologo -v q 2>&1
            ); then
                echo "    $(basename "$path"): pass"; unit_pass=$((unit_pass+1))
            else
                unit_rc=$?
                echo "    $(basename "$path"): fail (rc=${unit_rc})"; failed=1
            fi
        done
        [ "$unit_pass" -eq 3 ] && write_state verifyResults.unit '"pass"' || write_state verifyResults.unit '"fail"'
    fi

    # 4 integrity
    echo "  4/5 integrity"
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

    # 5 nupkg end-to-end: install the tool from the just-built nupkg and
    # publish the HelloWorld fixture.  We do this in the worktree's isolated
    # temp dir so the global tool install doesn't pollute the host.
    echo "  5/5 nupkg e2e"
    if [ "$DRY_RUN" -eq 1 ]; then echo "    [dry] nupkg e2e"
    else
        local nupkg="$RC_ARTIFACTS_BASE/release/tool/chaos-il2cpp.${ver}.nupkg"
        if [ -f "$nupkg" ]; then
            # Install to a temp directory (not --global) to avoid side effects
            local tool_test_dir
            tool_test_dir=$(mktemp -d)
            if dotnet tool install chaos-il2cpp \
                --add-source "$(dirname "$nupkg")" \
                --tool-path "$tool_test_dir" >/dev/null 2>&1; then
                # Run chaos-il2cpp --version (added to Driver as a first-class command)
                # to verify the tool assembled in the nupkg loads and reports.
                if "$tool_test_dir/chaos-il2cpp" --version >/dev/null 2>&1; then
                    echo "    nupkg install + version: pass"
                else
                    echo "    nupkg version check failed"; failed=1
                fi
                rm -rf "$tool_test_dir"
            else
                echo "    nupkg install failed (expected in CI without matching .NET SDK)"
                echo "    (skipped — not blocking)"
            fi
        else
            echo "    nupkg not found at $nupkg (skipped — not blocking)"
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

    # ── 0. Auto-detect previous tag ──────────────────────────────────────
    # The release tag ($tag) does NOT exist yet — it's created in step 2 below.
    # So derive the "previous release tag" from the SNAPSHOT BRANCH's history
    # (git describe walks commits reachable from the branch), never from $tag^.
    local prev_tag
    prev_tag=$(git describe --abbrev=0 --tags "$branch" 2>/dev/null || echo "")
    if [ -z "$prev_tag" ]; then
        prev_tag=$(git tag -l "v[0-9]*.[0-9]*.[0-9]*" --sort=-version:refname \
            | grep -v "^${tag}$" | head -1)
    fi
    if [ -z "$prev_tag" ]; then
        prev_tag=$(git rev-list --max-parents=0 "$branch" 2>/dev/null | tail -1)
    fi
    echo "  previous tag: ${prev_tag:-<none>}"

    # ── 1. Release notes + CHANGELOG (on snapshot branch, before tag) ────
    echo "[1/6] release notes + CHANGELOG"
    if [ "$DRY_RUN" -eq 1 ]; then echo "  [dry] generate release notes + update CHANGELOG"
    else
        local notes="$RC_ARTIFACTS_BASE/release/$ver/RELEASE_NOTES_${ver}.md"
        mkdir -p "$(dirname "$notes")"
        if [ -n "$prev_tag" ]; then
            dispatch_cmd bash scripts/generate-release-notes.sh "$prev_tag" "$tag" > "$notes" 2>/dev/null || true
        else
            dispatch_cmd bash scripts/generate-release-notes.sh "" "$tag" > "$notes" 2>/dev/null || true
        fi
        # Update CHANGELOG: append to the end (not insert at the top), so the
        # existing hand-written structure is preserved.  Strip the release notes'
        # own "# v0.2.0" header since it's redundant with the "## [ver]" entry.
        local changelog="$REPO_ROOT/CHANGELOG.md"
        local tmpcl
        tmpcl=$(mktemp)
        if grep -q "^## \[${ver}\] -" "$changelog" 2>/dev/null; then
            echo "  CHANGELOG already has an entry for ${ver} — skipping (idempotent)"
        else
            {
                cat "$changelog" 2>/dev/null || echo "# Changelog"
                echo ""
                echo "## [${ver}] - $(date +%Y-%m-%d)"
                echo ""
                # Strip the first line of release notes (which is "# v0.2.0") since
                # we already emit "## [ver]" as the CHANGELOG entry header.
                sed -n '2,$p' "$notes" 2>/dev/null || echo "  (auto-generated notes)"
                echo ""
            } > "$tmpcl"
        cp "$tmpcl" "$changelog"
        rm -f "$tmpcl"
        fi
        dispatch_cmd git add CHANGELOG.md || true
        dispatch_cmd git add "$notes" 2>/dev/null || true
        dispatch_cmd git commit -m "docs(changelog): update for v${ver}

root_cause: CHANGELOG.md was manually maintained and never updated by
  the release pipeline; release notes were not saved to a stable path.
fix_strategy: release.sh publish now generates notes + updates CHANGELOG
  on the snapshot branch before the tag is applied.
regression_check: CHANGELOG.md contains the v${ver} entry on the tag
  commit and on main after merge." --allow-empty >/dev/null 2>&1 || true
        echo "  release notes + CHANGELOG updated"
    fi

    # ── 2. tag + push (immutable — refuses to overwrite an existing tag) ────
    echo "[2/6] tag + push"
    if [ "$DRY_RUN" -eq 1 ]; then echo "  [dry] tag/push ${tag} ${branch}"
    else
        if dispatch_cmd git rev-parse -q --verify "refs/tags/$tag" >/dev/null 2>&1; then
            echo "  Error: tag '$tag' already exists. Refusing to overwrite an immutable tag." >&2
            echo "  To release a fix, bump the version number (e.g. ${ver} → x.y.z+1)." >&2
            exit 1
        fi
        dispatch_cmd git tag -a "$tag" -m "release v${ver}" 2>/dev/null || { echo "  Error: tag creation failed"; exit 1; }
        dispatch_cmd git push origin "$branch" 2>&1 | tail -1 || true
        dispatch_cmd git push origin "$tag" 2>&1 | tail -1 || true
        echo "  pushed"
    fi

    # ── 3. build nupkg + integrity (in WORKTREE, so the nupkg matches verified code) ─
    echo "[3/6] build nupkg + integrity"
    if [ "$DRY_RUN" -eq 1 ]; then echo "  [dry] build-tool-package"
    else
        dispatch_cmd bash scripts/build-tool-package.sh "$ver" 2>&1 | tail -3 || {
            echo "  ⚠️  nupkg build failed (continuing with publish)" >&2
        }
        # Copy nupkg from worktree back to main repo's artifact dir so the
        # gh release upload step can find it.
        local wt_nupkg="$WORKTREE_DIR/artifacts/release/tool/chaos-il2cpp.${ver}.nupkg"
        local main_nupkg="$RC_ARTIFACTS_BASE/release/tool/chaos-il2cpp.${ver}.nupkg"
        if [ -f "$wt_nupkg" ]; then
            mkdir -p "$(dirname "$main_nupkg")"
            cp "$wt_nupkg" "$main_nupkg" 2>/dev/null || true
        fi
        local sdk_dir="$RC_ARTIFACTS_BASE/release/$ver"
        mkdir -p "$sdk_dir"
        dispatch_cmd bash scripts/generate-checksums.sh "$sdk_dir" >/dev/null 2>&1 || true
        dispatch_cmd bash scripts/generate-sbom.sh "$sdk_dir" "$ver" >/dev/null 2>&1 || true
        echo "  artifacts ready"
    fi

    # ── 4. GitHub Release ────────────────────────────────────────────────
    echo "[4/6] GitHub Release"
    if [ "$DRY_RUN" -eq 1 ]; then echo "  [dry] gh release create ${tag}"
    elif command -v gh >/dev/null 2>&1 && gh auth status >/dev/null 2>&1; then
        local notes="$RC_ARTIFACTS_BASE/release/$ver/RELEASE_NOTES_${ver}.md"
        # Create the release with the structured notes
        if gh release view "$tag" >/dev/null 2>&1; then
            echo "  Release $tag already exists — editing body"
            gh release edit "$tag" -F "$notes" 2>/dev/null || true
        else
            ( cd "$REPO_ROOT" && gh release create "$tag" \
                -F "$notes" \
                --title "$tag" \
                --verify-tag 2>&1 | tail -3 ) || \
                echo "  ⚠️  gh release create nonzero (CI will handle tag push)" >&2
        fi
        # Upload all built artifacts (nupkg, checksums, sbom)
        if [ -d "$RC_ARTIFACTS_BASE/release/$ver" ]; then
            find "$RC_ARTIFACTS_BASE/release/$ver" -type f \
                ! -name "RELEASE_NOTES_*" \
                -exec gh release upload "$tag" {} --clobber \; 2>/dev/null || true
        fi
        # Upload the nupkg from the tool directory
        if [ -f "$RC_ARTIFACTS_BASE/release/tool/chaos-il2cpp.${ver}.nupkg" ]; then
            gh release upload "$tag" "$RC_ARTIFACTS_BASE/release/tool/chaos-il2cpp.${ver}.nupkg" --clobber 2>/dev/null || true
        fi
        echo "  release created/updated"
    else
        echo "  gh not authenticated; tag pushed — CI release.yml will create Release."
    fi

    # ── 4b. NuGet publish (optional) ──────────────────────────────────────
    if [ "$DRY_RUN" -eq 1 ]; then echo "  [dry] nuget publish"
    elif [ -f "$RC_ARTIFACTS_BASE/release/tool/chaos-il2cpp.${ver}.nupkg" ] && \
         [ -n "${NUGET_API_KEY:-}" ]; then
        echo "  [4b/6] Publishing to NuGet.org..."
        ( cd "$REPO_ROOT" && bash scripts/publish-nuget.sh "$ver" 2>&1 | tail -5 ) || \
            echo "  ⚠️  NuGet publish failed (continuing)" >&2
    else
        echo "  [4b/6] NuGet publish skipped (set NUGET_API_KEY to enable)"
    fi

    # ── 5. merge snapshot → main ─────────────────────────────────────────
    echo "[5/6] merge snapshot → main"
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

    echo "[6/6] cleanup"
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