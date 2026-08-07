#!/usr/bin/env bash
# Run foundation DLL verification for a single family.
# Usage: bash scripts/run-foundation-dll-family.sh <family-slug> [--strict] [--skip ...]
#
# INVOKES the real verification entry point:
#   python -m verification.chunk_pipeline --chunk <slug> [--mode strict] ...
# (NOT the stale 'verification.entry_points.cli' path which never existed.)
#
# Script-level flags:
#   --strict          → --mode strict            (coverage fails if >5% methods missing)
#   --skip <stage>... → removes those stages from --stages
#   --assembly <name> → passed through (used by run-foundation-dll-all.sh)
#   any other arg     → passed through to the pipeline verbatim
set -euo pipefail

REPO_ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
cd "$REPO_ROOT"

if [ $# -lt 1 ]; then
    echo "Usage: $0 <family-slug> [--strict] [--skip stage ...] [pipe flags...]"
    echo ""
    echo "Examples:"
    echo "  $0 convert-char"
    echo "  $0 convert-char --strict"
    echo "  $0 convert-char --skip benchmark hotupdate"
    exit 1
fi

FAMILY_SLUG="$1"
shift

PYTHONPATH="$REPO_ROOT/tests/e2e:$REPO_ROOT/tests/e2e/verification:$REPO_ROOT/build/toolchains/run"
export PYTHONPATH

# The verification engine now lives at tests/e2e/verification (decoupled from the
# family tree). Point it at where the 28-family tree + config/pipeline-config.yaml
# (CHAOS_FOUNDATION_DLL) and the shared _pipeline support package (CHAOS_TESTING_DIR)
# currently live.
export CHAOS_FOUNDATION_DLL="$REPO_ROOT/testing/foundation-dll"
export CHAOS_TESTING_DIR="$REPO_ROOT/testing"

MODE_ARGS=()
ASSEMBLY_ARGS=()
SKIP_STAGES=()
PASSTHROUGH_ARGS=()

# Parse script-level flags; everything else passes through to the pipeline.
while [ $# -gt 0 ]; do
    case "$1" in
        --strict)
            MODE_ARGS+=(--mode strict)
            shift
            ;;
        --skip)
            shift
            while [ $# -gt 0 ] && [[ "$1" != --* ]]; do
                SKIP_STAGES+=("$1")
                shift
            done
            ;;
        --assembly)
            shift
            ASSEMBLY_ARGS+=(--assembly "$1")
            shift
            ;;
        *)
            PASSTHROUGH_ARGS+=("$1")
            shift
            ;;
    esac
done

# Default stage set (mirror of chunk_pipeline's default: build,fact,hotupdate,coverage-audit).
DEFAULT_STAGES="build,fact,hotupdate,coverage-audit"
STAGES_ARG=()
if [ ${#SKIP_STAGES[@]} -gt 0 ]; then
    kept=()
    IFS=',' read -r -a all_stages <<< "$DEFAULT_STAGES"
    for s in "${all_stages[@]}"; do
        skip_it=0
        for skip in "${SKIP_STAGES[@]}"; do
            if [ "$s" = "$skip" ]; then skip_it=1; break; fi
        done
        [ $skip_it -eq 0 ] && kept+=("$s")
    done
    if [ ${#kept[@]} -gt 0 ]; then
        STAGES_ARG=(--stages "$(IFS=,; echo "${kept[*]}")")
    fi
fi

echo "=== Foundation DLL Verification: $FAMILY_SLUG ==="
echo "PYTHONPATH=$PYTHONPATH"

# shellcheck disable=SC2086
python -m verification.chunk_pipeline \
    --chunk "$FAMILY_SLUG" \
    "${ASSEMBLY_ARGS[@]}" \
    "${STAGES_ARG[@]}" \
    "${MODE_ARGS[@]}" \
    "${PASSTHROUGH_ARGS[@]}"
