#!/usr/bin/env bash
# Validate all benchmark timings are positive (non-zero) from unified-verification-report.json.
# Usage: bash scripts/check-perf-timings.sh <slug>
# Returns 0 if all timings > 0, 1 otherwise.
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
failures = []
for stage_name, stage in d.get('stages', {}).items():
    if 'benchmark' not in stage_name:
        continue
    details = stage.get('details', {})
    for runtime in ['native-aot', 'native-jit']:
        results = details.get(runtime, {}).get('results', [])
        for r in results:
            t = r.get('elapsedMilliseconds', -1)
            m = r.get('methodSubjectId', 'unknown')
            if t <= 0:
                failures.append(f'{stage_name}/{runtime}/{m}: elapsedMilliseconds={t}')
if failures:
    for f in failures:
        print(f'FAIL: {f}')
    sys.exit(1)
else:
    print('All timing > 0')
"
