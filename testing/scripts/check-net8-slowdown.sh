#!/usr/bin/env bash
# Check native-aot and native-jit slowdown vs .NET 8 baseline ≤ 20%.
# Usage: bash scripts/check-net8-slowdown.sh <slug>
# Returns 0 if all slowdowns ≤ 20%, 1 otherwise.
set -euo pipefail

if [ $# -lt 1 ]; then
    echo "Usage: $0 <family-slug>"
    exit 1
fi

SLUG="$1"
REPO_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
REPORT="$REPO_ROOT/testing/results/foundation-dll/System.Private.CoreLib/$SLUG/unified-verification-report.json"

if [ ! -f "$REPORT" ]; then
    echo "ERROR: Report not found: $REPORT"
    exit 1
fi

python -c "
import json, sys

d = json.load(open('$REPORT'))

# Extract benchmark stage
stages = d.get('stages', {})
bench_stage = None
for name, stage in stages.items():
    if 'benchmark' in name and 'hotupdate' not in name:
        bench_stage = stage
        break

if bench_stage is None:
    print('FAIL: no benchmark stage found')
    sys.exit(1)

details = bench_stage.get('details', {})
failures = []

for runtime_label, runtime_key in [('AOT', 'native-aot'), ('JIT', 'native-jit')]:
    rt_data = details.get(runtime_key, {})
    if not isinstance(rt_data, dict):
        print(f'WARNING: {runtime_key} has no structured data')
        continue
    results = rt_data.get('results', [])
    net8 = rt_data.get('net8Results', [])
    if not results:
        print(f'WARNING: {runtime_key} has no results')
        continue
    for i, r in enumerate(results):
        chaos_ns = r.get('elapsedMilliseconds', 0)
        net8_ns = net8[i].get('elapsedMilliseconds', 0) if i < len(net8) else 0
        method = r.get('methodSubjectId', f'method-{i}')
        if chaos_ns > 0 and net8_ns > 0:
            slowdown = (chaos_ns / net8_ns - 1.0) * 100
            if slowdown > 20:
                failures.append(f'{runtime_label} {method}: {slowdown:.1f}% > 20%')
        elif chaos_ns <= 0:
            failures.append(f'{runtime_label} {method}: chaos timing is zero')

if failures:
    for f in failures:
        print(f'FAIL: {f}')
    sys.exit(1)
else:
    print('All vs .NET 8 slowdown ≤ 20%')
"
