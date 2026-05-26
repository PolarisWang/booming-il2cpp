"""Inject Phase 0 methodSubjectIds into capability-family-ledger.json."""
import json
from copy import deepcopy
from pathlib import Path

REPO_ROOT = Path("D:/agent/booming-il2cpp")
LEDGER_PATH = REPO_ROOT / "verification" / "projections" / "foundation-dll-audit" / "capability-family-ledger.json"
PHASE0_PATH = REPO_ROOT / "artifacts" / ".tmp" / "phase0-corelib-method-subject-ids.json"

# Load both files
ledger = json.loads(LEDGER_PATH.read_text(encoding="utf-8"))
phase0 = json.loads(PHASE0_PATH.read_text(encoding="utf-8"))

# Build lookup: family_id -> methodSubjectIds
phase0_lookup = {}
for family in phase0["families"]:
    phase0_lookup[family["familyId"]] = family["methodSubjectIds"]

# Find System.Private.CoreLib in ledger
stats = {"updated": 0, "skipped_existing": 0, "not_found": 0}
for dll in ledger["dlls"]:
    if dll["assemblyName"] != "System.Private.CoreLib":
        continue
    for family in dll["families"]:
        fid = family["familyId"]
        if fid in phase0_lookup:
            m_ids = phase0_lookup[fid]
            existing = [str(s) for s in family.get("methodSubjectIds", []) if str(s)]
            if existing:
                print(f"  SKIP {fid}: already has {len(existing)} methodSubjectIds")
                stats["skipped_existing"] += 1
                continue
            family["methodSubjectIds"] = m_ids
            family["methodCount"] = len(m_ids)
            print(f"  UPDATE {fid}: {len(m_ids)} methodSubjectIds")
            stats["updated"] += 1
        else:
            print(f"  NOT FOUND in phase0: {fid}")
            stats["not_found"] += 1

# Write back
LEDGER_PATH.write_text(json.dumps(ledger, indent=2, ensure_ascii=False) + "\n", encoding="utf-8")
print(f"\nDone. Updated: {stats['updated']}, Skipped: {stats['skipped_existing']}, Not found: {stats['not_found']}")
