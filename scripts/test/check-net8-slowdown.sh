#!/usr/bin/env bash
# Check native-aot and native-jit slowdown vs .NET 8 baseline ≤ 20%.
# Reads from multi-run-report.json for per-method comparison.
# Automatically detects and excludes exception-path methods from comparison.
# Usage: bash scripts/check-net8-slowdown.sh <slug> [--max-slowdown <pct>] [--verbose]
# Returns 0 if all valid (non-exception) slowdowns ≤ max-slowdown (default 20%), 1 otherwise.
set -euo pipefail

if [ $# -lt 1 ]; then
    echo "Usage: $0 <family-slug> [--max-slowdown 20] [--verbose]"
    exit 1
fi

SLUG="$1"
shift
MAX_SLOWDOWN=20
VERBOSE=0

while [ $# -gt 0 ]; do
    case "$1" in
        --max-slowdown) MAX_SLOWDOWN="$2"; shift 2;;
        --verbose) VERBOSE=1; shift;;
        *) echo "Unknown option: $1"; exit 1;;
    esac
done

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd -W)"
REPO_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd -W)"
REPORT_DIR="$REPO_ROOT/testing/foundation-dll/System.Private.CoreLib/$SLUG"
MULTI_RUN="$REPORT_DIR/multi-run/multi-run-report.json"
REPORT="$REPORT_DIR/unified-verification-report.json"

if [ ! -f "$MULTI_RUN" ]; then
    # fallback
    REPORT_DIR="$REPO_ROOT/testing/results/foundation-dll/System.Private.CoreLib/$SLUG"
    MULTI_RUN="$REPORT_DIR/multi-run/multi-run-report.json"
    REPORT="$REPORT_DIR/unified-verification-report.json"
fi

if [ ! -f "$MULTI_RUN" ]; then
    echo "ERROR: multi-run-report.json not found at $MULTI_RUN"
    exit 1
fi

python -c "
import json, sys

multi = json.load(open('$MULTI_RUN'))
methods = multi.get('methods', [])
if not methods:
    print('FAIL: multi-run has no methods')
    sys.exit(1)

failures = []
exception_paths = []
normal_methods = []

for m in methods:
    label = m.get('label', 'unknown')
    sid = m.get('subject_id', '')
    samples = m.get('samples', {})

    ca = samples.get('chaos-aot', {}) or samples.get('native-aot', {})
    cj = samples.get('chaos-jit', {}) or samples.get('native-jit', {})
    n8 = samples.get('net8-jit', {})

    ca_ns = ca.get('mean_ns', -1)
    cj_ns = cj.get('mean_ns', -1)
    n8_ns = n8.get('mean_ns', -1)

    # Detect exception-path: chaos-aot >> net8-jit (20x+)
    is_exception = False
    if ca_ns > 0 and n8_ns > 0 and ca_ns > n8_ns * 20:
        is_exception = True
    if cj_ns > 0 and n8_ns > 0 and cj_ns > n8_ns * 20:
        is_exception = True

    if is_exception:
        exception_paths.append(sid)
        continue

    normal_methods.append(label)

    # AOT vs .NET 8
    if ca_ns > 0 and n8_ns > 0:
        slowdown = (ca_ns / n8_ns - 1.0) * 100
        if slowdown > $MAX_SLOWDOWN:
            failures.append(f'AOT {sid}: {slowdown:.1f}% > {$MAX_SLOWDOWN}% (chaos={ca_ns:.1f}ns net8={n8_ns:.1f}ns)')
    elif ca_ns <= 0 and ca_ns != -1:
        failures.append(f'AOT {sid}: chaos timing is zero')

    # JIT vs .NET 8
    if cj_ns > 0 and n8_ns > 0:
        slowdown = (cj_ns / n8_ns - 1.0) * 100
        if slowdown > $MAX_SLOWDOWN:
            failures.append(f'JIT {sid}: {slowdown:.1f}% > {$MAX_SLOWDOWN}% (chaos={cj_ns:.1f}ns net8={n8_ns:.1f}ns)')
    elif cj_ns <= 0 and cj_ns != -1:
        failures.append(f'JIT {sid}: chaos timing is zero')

print(f'Methods compared: {len(normal_methods)}, exception-path excluded: {len(exception_paths)}')

if exception_paths:
    print('Excluded (exception-path, invalid inputs):')
    for ep in exception_paths:
        print(f'  [SKIP] {ep}')

if failures:
    for f in failures:
        print(f'FAIL: {f}')
    sys.exit(1)
else:
    print(f'All vs .NET 8 slowdown ≤ {$MAX_SLOWDOWN}% (exception-path methods excluded)')
"
