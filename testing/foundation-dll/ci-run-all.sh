#!/bin/bash
# CI Pipeline — Foundation-DLL Full Verification
# Run from: testing/foundation-dll/
# Usage: bash ci-run-all.sh [--stages build,fact] [--batch small|medium|large]
#
# Prerequisites:
#   1. cmake --preset windows-x64-reference && cmake --build ... (prebuilt runtime libs)
#   2. dotnet build all managed projects
#   3. At least 16GB RAM, 8+ CPU cores recommended

set -e
cd "$(dirname "$0")"

# ── Locate the verification engine ────────────────────────────────────────────
# This script lives in testing/foundation-dll/, but the engine moved to
# tests/e2e/verification/ (commit 9951957db "删除旧 verification 管线").  The
# pre-move path `verification/chunk_pipeline.py` no longer exists, which made
# this script die instantly under `set -e` (it printed the assembly banner and
# then exited 1 with no diagnostic).
FOUNDATION_DIR="$(pwd)"                 # testing/foundation-dll
REPO_ROOT="$(cd ../.. && pwd)"
ENGINE_DIR="$REPO_ROOT/tests/e2e"

# The engine resolves its roots through tests/e2e/verification/_path.py, which
# marker-walks upward for config/pipeline-config.yaml.  That marker only exists
# under testing/foundation-dll/, which is NOT an ancestor of tests/e2e/ — so the
# walk fails and the override below is required (verified: without it,
# foundation_root() falls back to a wrong directory).
export CHAOS_FOUNDATION_DLL="$FOUNDATION_DIR"
# testing_tree_root() then resolves to the parent of that, i.e. the `testing/`
# tree that holds _pipeline/ — which is what `_pipeline.tool_helpers` expects.
#
# Guard the splice: when PYTHONPATH is unset, "$ENGINE_DIR:$PYTHONPATH" yields a
# TRAILING COLON.  That empty entry is documented as harmless ("the current
# directory"), but under Git Bash + Windows Python it makes the whole variable
# unusable — `import verification` then fails with ModuleNotFoundError even
# though the path is present and correct.  Measured: PYTHONPATH="$E:" fails,
# PYTHONPATH="$E" succeeds, same directory.
if [ -n "${PYTHONPATH:-}" ]; then
    export PYTHONPATH="$ENGINE_DIR:$PYTHONPATH"
else
    export PYTHONPATH="$ENGINE_DIR"
fi

STAGES="${1:-build,fact,benchmark,managed_benchmark,benchmark_report,aggregate,reporting}"
BATCH="${2:-small}"

echo "=== CI Pipeline: $BATCH assemblies, stages=$STAGES ==="
echo "Started: $(date -u +%Y-%m-%dT%H:%M:%SZ)"
echo "Engine: $ENGINE_DIR"

run_asm() {
    local asm="$1"
    echo ""
    echo "=== $asm ==="
    # `python -m` (not a file path) so the package-relative imports inside the
    # engine resolve; `-u` so progress is not swallowed by the grep pipe below.
    timeout 1800 python -u -m verification.chunk_pipeline \
        --assembly "$asm" --all-chunks --stages "$STAGES" \
        2>&1 | grep -E "Stage|passed|failed|error|Done|reporting"
    echo "Exit: ${PIPESTATUS[0]}"
}

# Batch 1: Small assemblies (< 50 methods)
if [ "$BATCH" = "small" ] || [ "$BATCH" = "all" ]; then
    echo "=== BATCH: Small Assemblies ==="
    for asm in System.Collections.NonGeneric System.Collections.Immutable System.IO.Compression.Brotli \
               System.IO.Compression.ZipFile System.Linq System.Net.ServerSentEvents \
               System.ObjectModel System.Runtime.InteropServices System.Security.Claims \
               System.Security.Principal.Windows System.Threading.Tasks.Parallel; do
        run_asm "$asm"
    done
fi

# Batch 2: Medium assemblies (50-200 methods)
if [ "$BATCH" = "medium" ] || [ "$BATCH" = "all" ]; then
    echo "=== BATCH: Medium Assemblies ==="
    for asm in System.Diagnostics.DiagnosticSource System.Formats.Asn1 System.IO.Pipelines \
               System.Linq.Expressions System.Net.Http System.Runtime.Serialization.Formatters; do
        run_asm "$asm"
    done
fi

# Batch 3: Large assemblies (>200 methods) — may need separate CI jobs
if [ "$BATCH" = "large" ] || [ "$BATCH" = "all" ]; then
    echo "=== BATCH: Large Assemblies ==="
    for asm in System.ComponentModel.TypeConverter System.Data.Common System.Net.Sockets \
               System.Private.CoreLib System.Private.Xml System.Reflection.Metadata \
               System.Runtime.Intrinsics System.Security.Cryptography System.Text.Json \
               System.Xml.ReaderWriter; do
        run_asm "$asm"
    done
fi

echo ""
echo "=== CI Pipeline Complete ==="
echo "Finished: $(date -u +%Y-%m-%dT%H:%M:%SZ)"
