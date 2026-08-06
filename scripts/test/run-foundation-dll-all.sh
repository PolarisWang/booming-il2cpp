#!/usr/bin/env bash
# Run foundation DLL verification for all imported families.
# Usage: bash scripts/run-foundation-dll-all.sh [--strict]
set -euo pipefail

REPO_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$REPO_ROOT"

MODE="${1:---skip}"
if [ "$MODE" = "--strict" ]; then
    MODE="--strict"
    shift
else
    MODE=""
fi

echo "=== Foundation DLL Verification: All Imported Families ==="

LEDGER="$REPO_ROOT/testing/foundation-dll/_contracts/ledger.json"
if [ ! -f "$LEDGER" ]; then
    echo "ERROR: Ledger not found at $LEDGER"
    exit 1
fi

# Extract imported families from ledger
python3 -c "
import json
ledger = json.load(open('$LEDGER'))
for asm in ledger.get('assemblies', []):
    for fam in asm.get('families', []):
        if fam.get('migrationStatus') in ('imported', 'verified'):
            print(f'{fam[\"slug\"]} {asm[\"assemblyName\"]}')
" | while read -r slug assembly; do
    echo ""
    echo "============================================"
    echo "  Running: $slug ($assembly)"
    echo "============================================"
    bash "$REPO_ROOT/scripts/test/run-foundation-dll-family.sh" "$slug" --assembly "$assembly" $MODE || true
done

echo ""
echo "=== All families completed ==="
