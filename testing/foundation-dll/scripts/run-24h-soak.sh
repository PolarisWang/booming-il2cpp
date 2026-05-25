#!/bin/bash
# 24h soak test entry point
# Usage: ./run-24h-soak.sh [--hours 24] [--phase gc] [--report-dir ...]

set -euo pipefail
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"

cd "$PROJECT_ROOT"

# Convert --hours to --minutes if provided
MINUTES=""
while [[ $# -gt 0 ]]; do
    case "$1" in
        --hours)
            MINUTES=$(( $2 * 60 ))
            shift 2
            ;;
        --minutes)
            MINUTES="$2"
            shift 2
            ;;
        *)
            break
            ;;
    esac
done

ARGS=()
if [ -n "$MINUTES" ]; then
    ARGS+=(--minutes "$MINUTES")
fi
ARGS+=("$@")

exec python _core/python/soak_orchestrator.py "${ARGS[@]}"
