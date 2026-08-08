#!/usr/bin/env bash
# ─── Git Hooks Activation (setup-hooks.sh) ───────────────────────────────
# Configures git to use the repository's hooks directory (.githooks/) so the
# pre-commit (arch/trace/layer-boundary/secret/artifact gate) and pre-push
# (LLM deep review) hooks actually run.
#
# WHY: `core.hooksPath` is stored in your LOCAL .git/config and is NOT
# distributed with a fresh clone. Without this setup step a fresh checkout
# runs NO project hooks — the "automatically" claim in CONTRIBUTING.md is
# only true after this script has been run once. Idempotent: safe to re-run.
#
# Usage:
#   ./scripts/setup-hooks.sh
# =========================================================================

set -euo pipefail

REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
HOOKS_DIR=".githooks"

cd "${REPO_ROOT}"

if [[ ! -d "${HOOKS_DIR}" ]]; then
    echo "[setup-hooks] ERROR: ${HOOKS_DIR}/ not found under ${REPO_ROOT}"
    echo "[setup-hooks]          expected repository hooks directory (not git hooks/.sample)"
    exit 1
fi

git config core.hooksPath "${HOOKS_DIR}"
echo "[setup-hooks] core.hooksPath -> ${HOOKS_DIR} (set for this checkout)"

# Show the resolved hooks path for confirmation.
echo "[setup-hooks] resolved hooks: $(git config core.hooksPath)"

# Verify the primary hooks exist & are executable.
for h in pre-commit pre-push; do
    if [[ -x "${HOOKS_DIR}/${h}" || -f "${HOOKS_DIR}/${h}" ]]; then
        echo "[setup-hooks]   ✓ ${h} present"
    else
        echo "[setup-hooks]   ! ${h} missing (check ${HOOKS_DIR}/)"
    fi
done

echo "[setup-hooks] Done. The pre-commit/pre-push gates will now run."
