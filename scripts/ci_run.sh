#!/usr/bin/env bash
# ===============================================================================
# ci_run.sh - self-contained, portable test-run entry for this repository.
#
# WHY: the repo must be able to run its full test suite with NO dependency on
# GitHub, the gh CLI, or any CI service. This script wraps the single unified
# driver (tests/runner/test_driver.py) which already drives all five layers -
# unit (dotnet), integration (native CTest, auto cmake-configured), e2e
# (foundation-dll engine, env from suite_contract), pytest (authority/integration)
# - and reports through the same machine-readable test-report.json.
#
# This is the canonical "run the tests" entry. Older scripts that baked in stale
# pre-L-migration paths (scripts/test/run-all.sh) or that depend on GitHub
# (scripts/test/verify-ci.sh) are superseded; prefer this.
#
# Usage:
#   ./scripts/ci_run.sh                 # full suite: --layer all
#   ./scripts/ci_run.sh --layer unit    # a single layer
#   ./scripts/ci_run.sh --quick         # reuse configured native build dir, no reconfigure
#   ./scripts/ci_run.sh --layer all --quick
#
# Exit: 0 if every layer is OK (known failures surfaced but don't fail the gate);
#       1 if any layer had an UNEXPECTED (non-known) failure.
# ===============================================================================

set -euo pipefail

REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$REPO_ROOT"

LAYER="all"
QUICK=""
EXTRA=""   # extra flags to pass through to test_driver.py (e.g. --dry-run, --cases, --group)

while [ $# -gt 0 ]; do
    case "$1" in
        --layer) LAYER="$2"; shift 2 ;;
        --quick) QUICK="--quick"; shift ;;
        --dry-run) EXTRA="$EXTRA --dry-run"; shift ;;
        --cases) EXTRA="$EXTRA --cases"; shift ;;
        --group) EXTRA="$EXTRA --group $2"; shift 2 ;;
        --help|-h)
            echo "Usage: $0 [--layer {unit,integration,e2e,pytest,all}] [--quick] [--dry-run]" >&2
            exit 0 ;;
        *) echo "Unknown option: $1 (try --help)" >&2; exit 1 ;;
    esac
done

# Validate python availability (test_driver needs python 3.10+ for the e2e/pytest layers).
if ! command -v python >/dev/null 2>&1; then
    echo "[ci_run] ERROR: 'python' not found on PATH." >&2
    exit 1
fi

echo "============================================================"
echo "  Chaos IL2CPP - unified test run  (layer=${LAYER}${QUICK:+ quick})"
echo "  repo: ${REPO_ROOT}"
echo "============================================================"

# Delegate to the single portable driver. --machine fixes the report path so a
# later caller can always read tests/runner/test-report.json. We do NOT require
# the gh CLI or any CI service here.
set +e
# shellcheck disable=SC2086  # QUICK/EXTRA expand to flags or empty
python tests/runner/test_driver.py --layer "$LAYER" --machine ${QUICK} ${EXTRA}
RC=$?
set -e

echo ""
echo "============================================================"
if [ $RC -eq 0 ]; then
    echo "  [ci_run] OVERALL OK - all layers gate-clean (known failures surfaced)."
else
    echo "  [ci_run] FAILED (rc=$RC) - see test-report.json / output above."
fi
echo "============================================================"
exit $RC

