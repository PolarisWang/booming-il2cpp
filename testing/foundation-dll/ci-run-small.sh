#!/bin/bash
# Quick CI run: small assemblies only (< 50 methods each)
# ~5-10 min total on CI
cd "$(dirname "$0")"
STAGES="${1:-build,fact,benchmark,managed_benchmark,benchmark_report,aggregate,reporting}"
for asm in System.Collections System.Collections.Immutable System.IO.Compression.Brotli System.Linq System.Net.ServerSentEvents System.ObjectModel System.Security.Claims System.Threading.Tasks.Parallel; do
  echo "=== $asm ==="
  timeout 1200 python verification/chunk_pipeline.py --assembly "$asm" --all-chunks --stages "$STAGES" 2>&1 | grep -E "passed|failed|error|Done|cross"
done
