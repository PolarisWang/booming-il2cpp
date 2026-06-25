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

STAGES="${1:-build,fact,benchmark,managed_benchmark,benchmark_report,aggregate,reporting}"
BATCH="${2:-small}"

echo "=== CI Pipeline: $BATCH assemblies, stages=$STAGES ==="
echo "Started: $(date -u +%Y-%m-%dT%H:%M:%SZ)"

run_asm() {
    local asm="$1"
    echo ""
    echo "=== $asm ==="
    timeout 1800 python verification/chunk_pipeline.py \
        --assembly "$asm" --all-chunks --stages "$STAGES" \
        2>&1 | grep -E "Stage|passed|failed|error|Done|reporting"
    echo "Exit: $?"
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
