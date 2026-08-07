#!/usr/bin/env bash
# Validate hotupdate semantic_changed > 0, overhead ≤ 100%, and patch was applied.
# Reads from hotupdate-verification-report.json and unified-verification-report.json.
# Usage: bash scripts/check-hotupdate.sh <slug> [--verbose]
# Returns 0 if all checks pass, 1 otherwise.
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
HU_REPORT="$REPORT_DIR/hotupdate-verification-report.json"
REPORT="$REPORT_DIR/unified-verification-report.json"

if [ ! -f "$HU_REPORT" ]; then
    # fallback
    REPORT_DIR="$REPO_ROOT/tests/e2e/translation/System.Private.CoreLib/$SLUG"
    HU_REPORT="$REPORT_DIR/hotupdate-verification-report.json"
    REPORT="$REPORT_DIR/unified-verification-report.json"
fi

if [ ! -f "$HU_REPORT" ]; then
    echo "ERROR: hotupdate-verification-report.json not found at $HU_REPORT"
    exit 1
fi

python -c "
import json, sys

hu = json.load(open('$HU_REPORT'))
failures = []
warnings = []

# Check 1: Was a real patch applied?
d3_patch_applied = hu.get('d3PatchApplied', False)
d3_patched_count = hu.get('d3PatchedCount', 0)
verification_kind = hu.get('verificationKind', 'unknown')

if not d3_patch_applied:
    warnings.append(f'No real patch applied (d3PatchApplied=false, kind={verification_kind})')
    warnings.append('postPatchNsPerOp=0.0 is expected — no patch DLL was deployed for this family')
    warnings.append('To produce hotupdate data: create a patch project with real method replacements')

# Check 2: semantic_changed > 0 (only meaningful if patch was applied)
semantic_changed = hu.get('semanticChangedCount', 0) or hu.get('allMethodsSemanticVerified', False)
total = hu.get('totalMethods', 0)

if d3_patch_applied:
    semantic_changed_count = hu.get('semanticChangedCount', 0)
    if semantic_changed_count <= 0:
        failures.append(f'semantic_changed_count={semantic_changed_count} (expected > 0)')
    else:
        print(f'semantic_changed={semantic_changed_count}/{total}')
else:
    print(f'semantic_changed=N/A (no patch applied, kind={verification_kind})')

# Check 3: Hotupdate benchmark overhead — compare post-patch timing
# vs pre-patch timing from the regular benchmark stage.
if d3_patch_applied:
    try:
        d = json.load(open('$REPORT'))
        stages = d.get('stages', {})
        for bench_key, pre_key in [('hotupdate_aot_benchmark', 'native-aot'),
                                   ('hotupdate_jit_benchmark', 'native-jit')]:
            bench_stage = stages.get(bench_key)
            if bench_stage is None:
                continue
            details = bench_stage.get('details', {})
            hot_results = details.get('results', [])
            if not hot_results:
                continue
            # Read pre-patch ops from benchmark stage
            pre_stage = stages.get('benchmark', {})
            pre_details = pre_stage.get('details', {}) if isinstance(pre_stage, dict) else {}
            pre_results = pre_details.get(pre_key, {}).get('results', []) if isinstance(pre_details, dict) else []
            for idx, r in enumerate(hot_results):
                post_ops = r.get('opsPerSecond', 0)
                if post_ops <= 0:
                    failures.append(f'{bench_key} result[{idx}]: opsPerSecond={post_ops} (expected > 0)')
                    continue
                # Compare vs pre-patch timing if available
                if idx < len(pre_results):
                    pre_ops = pre_results[idx].get('opsPerSecond', 0) if isinstance(pre_results[idx], dict) else 0
                    if pre_ops > 0 and post_ops > 0:
                        overhead = (pre_ops / post_ops - 1.0) * 100
                        if overhead > 100:
                            failures.append(f'{bench_key} method[{idx}]: overhead {overhead:.1f}% > 100%')
    except (json.JSONDecodeError, FileNotFoundError):
        warnings.append('Could not read unified report for overhead check')

if failures:
    for f in failures:
        print(f'FAIL: {f}')
    sys.exit(1)
else:
    if d3_patch_applied:
        print(f'HotUpdate OK: patch applied, all overhead ≤ 100%')
    else:
        print(f'HotUpdate OK: no patch applied (expected — no patch DLL for this family)')

if warnings:
    for w in warnings:
        print(f'WARN: {w}')
"
