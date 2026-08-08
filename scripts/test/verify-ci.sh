#!/usr/bin/env bash
# ⚠️ DEPENDS ON GITHUB — only relevant if CI runs on GitHub (polls gh for a workflow
# run conclusion). The repo is GitHub-independent; use `./scripts/ci_run.sh` for the
# portable, self-contained test entry that does not require gh or any CI service.
# Guarded below with `gh` existence checks; retained for the GitHub-hosting case only.
# Wait for CI to pass after a push to main.
# Usage: bash scripts/verify-ci.sh [--timeout 10] [--branch main]
# Returns 0 if CI passes within timeout, 1 on failure, 2 on timeout.
set -euo pipefail

TIMEOUT_MINUTES=10
BRANCH="main"
REPO=""

while [ $# -gt 0 ]; do
    case "$1" in
        --timeout) TIMEOUT_MINUTES="$2"; shift 2 ;;
        --branch) BRANCH="$2"; shift 2 ;;
        --repo) REPO="$2"; shift 2 ;;
        *) echo "Unknown option: $1"; exit 1 ;;
    esac
done

INTERVAL_SECONDS=30
MAX_ELAPSED=$((TIMEOUT_MINUTES * 60))
ELAPSED=0

# Determine repo if not specified
if [ -z "$REPO" ]; then
    if command -v gh &> /dev/null; then
        REPO=$(gh repo view --json nameWithOwner --jq .nameWithOwner 2>/dev/null || true)
    fi
fi

echo "[ci-verify] Waiting for CI on branch '$BRANCH' (timeout=${TIMEOUT_MINUTES}min)..."

check_ci_status() {
    local status=""
    local url=""

    # Try gh CLI first
    if command -v gh &> /dev/null; then
        local run_data
        run_data=$(gh run list --branch "$BRANCH" --limit 1 --json conclusion,headSha,url 2>/dev/null || true)
        if [ -n "$run_data" ] && [ "$run_data" != "[]" ]; then
            status=$(echo "$run_data" | python3 -c "import json,sys; d=json.load(sys.stdin); print(d[0]['conclusion'] if d and d[0].get('conclusion') else '')" 2>/dev/null || echo "")
            url=$(echo "$run_data" | python3 -c "import json,sys; d=json.load(sys.stdin); print(d[0].get('url',''))" 2>/dev/null || echo "")
        fi
    fi

    # Fallback: try GitHub API with GITHUB_TOKEN
    if [ -z "$status" ] && [ -n "$GITHUB_TOKEN" ] && [ -n "$REPO" ]; then
        local api_data
        api_data=$(curl -sf -H "Authorization: token $GITHUB_TOKEN" \
            "https://api.github.com/repos/$REPO/actions/runs?branch=$BRANCH&per_page=1" 2>/dev/null || true)
        if [ -n "$api_data" ]; then
            status=$(echo "$api_data" | python3 -c "import json,sys; d=json.load(sys.stdin); runs=d.get('workflow_runs',[]); print(runs[0]['conclusion'] if runs and runs[0].get('conclusion') else '')" 2>/dev/null || echo "")
            url=$(echo "$api_data" | python3 -c "import json,sys; d=json.load(sys.stdin); runs=d.get('workflow_runs',[]); print(runs[0].get('html_url',''))" 2>/dev/null || echo "")
        fi
    fi

    if [ "$status" = "success" ]; then
        echo "[ci-verify] CI passed"
        return 0
    elif [ "$status" = "failure" ] || [ "$status" = "cancelled" ]; then
        echo "[ci-verify] CI FAILED: $url"
        return 1
    elif [ -n "$status" ]; then
        # Still in progress (neutral, skipped, or null)
        return 2
    fi

    # No status available yet
    return 3
}

while [ $ELAPSED -lt $MAX_ELAPSED ]; do
    check_ci_status
    case $? in
        0) exit 0 ;;
        1) exit 1 ;;
        2|3) ;; # still running or no data yet
    esac

    sleep $INTERVAL_SECONDS
    ELAPSED=$((ELAPSED + INTERVAL_SECONDS))
    if [ $((ELAPSED % 120)) -eq 0 ]; then
        echo "[ci-verify] Still waiting... (${ELAPSED}s elapsed)"
    fi
done

echo "[ci-verify] TIMEOUT after ${TIMEOUT_MINUTES}min — check CI manually"
exit 2
