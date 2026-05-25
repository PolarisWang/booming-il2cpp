#!/usr/bin/env bash
# Run foundation DLL verification for a single family.
# Usage: bash scripts/run-foundation-dll-family.sh <family-slug> [--strict] [--skip ...]
set -euo pipefail

REPO_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$REPO_ROOT"

if [ $# -lt 1 ]; then
    echo "Usage: $0 <family-slug> [--strict] [--skip stage ...]"
    echo ""
    echo "Examples:"
    echo "  $0 convert-char"
    echo "  $0 convert-char --strict"
    echo "  $0 convert-char --skip benchmark hotupdate"
    exit 1
fi

FAMILY_SLUG="$1"
shift

PYTHONPATH="$REPO_ROOT/testing/foundation-dll/verification:$REPO_ROOT/build/toolchains/run/testing/foundation_dll:$REPO_ROOT/build/toolchains/run"
export PYTHONPATH

echo "=== Foundation DLL Verification: $FAMILY_SLUG ==="
echo "PYTHONPATH=$PYTHONPATH"

python -m verification.entry_points.cli "$FAMILY_SLUG" "$@"
