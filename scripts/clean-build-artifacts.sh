#!/usr/bin/env bash
# ===============================================================================
# clean-build-artifacts.sh -- reclaim disk from the foundation-dll build cache.
#
# The 29-family translation tree (tests/e2e/translation) accumulates per-family
# CMake/MSBuild/JIT build outputs under `chunks/*/native/build/` etc. These are
# ignored by git (never uploaded), but can total several GB on disk. This script
# removes ONLY the pure, rebuildable build caches -- NEVER the engine-consumed
# inputs (metadata/, subjects/, .cpp sources, dll-method-manifest.json, aot-core-ir).
#
# It is safe to re-run and is default-conservative:
#   - default  : removes only `chunks/*/native/build/**` + `chunks/*/results/**`
#                (pure CMake output + result reports -- rebuildable, not engine input)
#   - --aggressive : ALSO removes `chunks/*/build_jit_output/**`
#                (JIT output; keep default OFF because it may hold engine-referenced
#                 aot-core-ir.jdata until you've confirmed the live engine reconsumes it)
#
# Rebuilding: next `--layer integration`/`e2e` run reconstitutes the caches via
# native.py's auto cmake / the pipeline. Nothing downstream breaks.
#
# Usage:
#   ./scripts/clean-build-artifacts.sh            # conservative (native/build + results)
#   ./scripts/clean-build-artifacts.sh --dry-run  # preview, delete nothing
#   ./scripts/clean-build-artifacts.sh --aggressive
# ===============================================================================

set -uo pipefail

REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$REPO_ROOT"

AGGRESSIVE=0
DRY_RUN=0

while [ $# -gt 0 ]; do
    case "$1" in
        --aggressive) AGGRESSIVE=1; shift ;;
        --dry-run) DRY_RUN=1; shift ;;
        -h|--help) sed -n '1,12p' "$0" | sed 's/^# \{0,1\}//'; exit 0 ;;
        *) echo "Unknown: $1 (--help)"; exit 1 ;;
    esac
done

echo "=== clean-build-artifacts ==="
[ $DRY_RUN -eq 1 ] && echo "(dry-run: previewing only, nothing deleted)"

# Directories that are PURE build cache (rebuildable). Glob-guarded so a missing
# family (or future layout change) -> 0 matches -> no-op.
BUILD_PATTERNS=(
    "tests/e2e/translation/*/chunks/*/native/build"
    "tests/e2e/translation/*/chunks/*/results"
)
if [ $AGGRESSIVE -eq 1 ]; then
    BUILD_PATTERNS+=(
        "tests/e2e/translation/*/chunks/*/build_jit_output"
    )
fi

reclaimed=0
for pat in "${BUILD_PATTERNS[@]}"; do
    # expand glob, tolerate no-match
    for dir in $pat; do
        [ -d "$dir" ] || continue
        size=$(du -sh "$dir" 2>/dev/null | cut -f1)
        if [ $DRY_RUN -eq 1 ]; then
            echo "  [would-remove] $dir  ($size)"
        else
            echo "  removing   $dir  ($size)"
            rm -rf "$dir" && reclaimed=$((reclaimed+1))
        fi
    done
done

echo "=== done: ${reclaimed} dir(s) reclaimed ==="
[ $DRY_RUN -eq 1 ] && echo "(no changes made -- dry run)"
