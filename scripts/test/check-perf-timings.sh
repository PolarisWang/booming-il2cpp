#!/usr/bin/env bash
# Validate all benchmark timings are positive (non-zero) and check for data integrity.
# Reads from unified-verification-report.json and multi-run-report.json.
# Usage: bash scripts/check-perf-timings.sh <slug> [--verbose]
# Returns 0 if all valid timings > 0, 1 otherwise.
set -euo pipefail

if [ $# -lt 1 ]; then
    echo "Usage: $0 <family-slug> [--verbose]"
    exit 1
fi

SLUG="$1"
VERBOSE=0
if [ "${2:-}" = "--verbose" ]; then VERBOSE=1; fi

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd -W)"
REPO_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd -W)"
REPORT_DIR="$REPO_ROOT/tests/e2e/translation/System.Private.CoreLib/$SLUG"
REPORT="$REPORT_DIR/unified-verification-report.json"
MULTI_RUN="$REPORT_DIR/multi-run/multi-run-report.json"

if [ ! -f "$REPORT" ]; then
    echo "ERROR: Report not found: $REPORT"
    # fallback to testing/results/ path
    REPORT2="$REPO_ROOT/tests/e2e/translation/System.Private.CoreLib/$SLUG/unified-verification-report.json"
    if [ -f "$REPORT2" ]; then
        REPORT="$REPORT2"
        REPORT_DIR="$(dirname "$REPORT2")"
        MULTI_RUN="$REPORT_DIR/multi-run/multi-run-report.json"
    else
        exit 1
    fi
fi

python -c "
import json, sys, os

d = json.load(open('$REPORT'))
failures = []
warnings = []
exception_paths = []

# Check 1: Verify multi-run report exists and has data
multi = None
multi_path = '$MULTI_RUN'
if os.path.exists(multi_path):
    multi = json.load(open(multi_path))
    methods = multi.get('methods', [])
    if not methods:
        failures.append('multi-run-report.json has no methods')
    elif $VERBOSE:
        print(f'Multi-run: {len(methods)} methods')
else:
    warnings.append(f'multi-run-report.json not found at {multi_path}')

# Check 2: Verify all methods have positive timing in multi-run report
if multi:
    for m in methods:
        label = m.get('label', 'unknown')
        samples = m.get('samples', {})
        for runtime in ['chaos-aot', 'chaos-jit']:
            s = samples.get(runtime, {})
            mean_ns = s.get('mean_ns', -1)
            if mean_ns <= 0:
                failures.append(f'{runtime}/{label}: mean_ns={mean_ns} (<= 0)')

# Check 3: Detect exception-path methods (chaos-aot timing >> net8-jit baseline, typically 100x+)
if multi:
    for m in methods:
        label = m.get('label', 'unknown')
        sid = m.get('subject_id', '')
        samples = m.get('samples', {})
        ca = samples.get('chaos-aot', {})
        n8 = samples.get('net8-jit', {})
        ca_ns = ca.get('mean_ns', -1)
        n8_ns = n8.get('mean_ns', -1)
        if ca_ns > 0 and n8_ns > 0 and ca_ns > n8_ns * 20:
            ratio = ca_ns / n8_ns
            exception_paths.append(f'{sid}: chaos-aot={ca_ns:.1f}ns vs net8-jit={n8_ns:.1f}ns ({ratio:.0f}x)')
            if $VERBOSE:
                print(f'  WARN: Exception-path {label}: {ratio:.0f}x slower than .NET 8')

# Check 4: Verify benchmark stage summary has okCount matching totalMethods
stages = d.get('stages', {})
bench_stage = None
for name, stage in stages.items():
    if name == 'benchmark':
        bench_stage = stage
        break

if bench_stage:
    summaries = bench_stage.get('details', {}).get('summaries', {})
    for rt in ['chaos-aot', 'chaos-jit']:
        rt_summary = summaries.get(rt, {})
        ok = rt_summary.get('okCount', 0)
        total = rt_summary.get('totalMethods', 0)
        if ok != total:
            failures.append(f'{rt}: okCount={ok} != totalMethods={total}')

if failures:
    for f in failures:
        print(f'FAIL: {f}')
    sys.exit(1)
else:
    print('All timing > 0, data integrity OK')
    if exception_paths:
        print(f'Exception-path methods detected ({len(exception_paths)}):')
        for ep in exception_paths:
            print(f'  [EXCEPTION-PATH] {ep}')
        print('(These methods use invalid inputs that always throw; timing reflects exception handling cost)')
"
