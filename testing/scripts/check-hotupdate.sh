#!/usr/bin/env bash
# Validate hotupdate semantic_changed > 0 and overhead ≤ 100%.
# Usage: bash scripts/check-hotupdate.sh <slug>
# Returns 0 if all checks pass, 1 otherwise.
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
stages = d.get('stages', {})

# Find hotupdate fact stage
hu_stage = None
for name, stage in stages.items():
    if name == 'hotupdate':
        hu_stage = stage
        break

if hu_stage is None:
    print('FAIL: no hotupdate stage found')
    sys.exit(1)

hu_details = hu_stage.get('details', {})
semantic_changed = hu_details.get('semanticChangedCount', 0)
total = hu_details.get('totalMethods', 0)

failures = []

if semantic_changed <= 0:
    failures.append(f'semantic_changed_count={semantic_changed} (expected > 0)')

# Check hotupdate benchmark overhead
for bench_label, bench_key in [('AOT', 'hotupdate_aot_benchmark'), ('JIT', 'hotupdate_jit_benchmark')]:
    bench_stage = stages.get(bench_key)
    if bench_stage is None:
        print(f'WARNING: {bench_key} stage not found, skipping')
        continue
    details = bench_stage.get('details', {})
    results = details.get('results', [])
    if not results:
        continue
    for r in results:
        base_ns = r.get('baseElapsedNs', 0) or r.get('elapsedBeforePatchNs', 0)
        patch_ns = r.get('patchedElapsedNs', 0) or r.get('elapsedAfterPatchNs', 0)
        method = r.get('methodSubjectId', 'unknown')
        if base_ns > 0 and patch_ns > 0:
            overhead = (patch_ns / base_ns - 1.0) * 100
            if overhead > 100:
                failures.append(f'{bench_label} {method}: overhead {overhead:.1f}% > 100%')

if failures:
    for f in failures:
        print(f'FAIL: {f}')
    sys.exit(1)
else:
    print(f'HotUpdate OK: semantic_changed={semantic_changed}/{total}, all overhead ≤ 100%')
"
