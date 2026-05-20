"""Synchronize the new framework ledger from the old capability-family-ledger.json.

Usage:
    python _contracts/ledger-sync.py              # dry-run, show differences
    python _contracts/ledger-sync.py --update     # update new ledger from old
    python _contracts/ledger-sync.py --verify     # verify consistency
"""

from __future__ import annotations

import json
import sys
from pathlib import Path

_HERE = Path(__file__).resolve().parent
_TESTING_ROOT = _HERE.parents[1]
_NEW_LEDGER_PATH = _HERE / "ledger.json"
_OLD_LEDGER_PATH = (
    _TESTING_ROOT.parents[0]  # agent/booming-il2cpp
    / "verification" / "projections" / "foundation-dll-audit"
    / "capability-family-ledger.json"
)


def _find_family(ledger: dict, assembly: str, slug: str) -> dict | None:
    """Find a family in the new ledger by assembly and slug."""
    for asm in ledger.get("assemblies", []):
        if asm["assemblyName"] == assembly:
            for fam in asm.get("families", []):
                if fam["slug"] == slug:
                    return fam
    return None


def sync(dry_run: bool = True, update: bool = False, verify: bool = False) -> None:
    if not _OLD_LEDGER_PATH.exists():
        print(f"[sync] Old ledger not found: {_OLD_LEDGER_PATH}")
        return

    old = json.loads(_OLD_LEDGER_PATH.read_text(encoding="utf-8"))
    new = json.loads(_NEW_LEDGER_PATH.read_text(encoding="utf-8")) if _NEW_LEDGER_PATH.exists() else {"assemblies": []}

    old_count = 0
    new_count = 0
    added = 0
    mismatches = []

    for dll in old.get("dlls", []):
        asm_name = dll["assemblyName"]
        # Ensure assembly exists in new ledger
        asm_entry = next((a for a in new["assemblies"] if a["assemblyName"] == asm_name), None)
        if asm_entry is None:
            asm_entry = {"assemblyName": asm_name, "families": []}
            if update:
                new["assemblies"].append(asm_entry)
                print(f"[sync] ADD assembly: {asm_name}")

        for fam in dll.get("families", []):
            old_count += 1
            family_id = fam["familyId"]
            slug = family_id.split("/")[-1]  # "family/System.Private.CoreLib/convert/char" -> "char"
            # Handle multi-segment slugs: "convert/char" -> "convert-char"
            parts = family_id.split("/")
            if len(parts) >= 4:
                slug = "-".join(parts[3:])

            existing = _find_family(new, asm_name, slug)
            if existing is None:
                method_count = fam.get("methodCount", len(fam.get("methodSubjectIds", [])))
                entry = {
                    "familyId": family_id,
                    "slug": slug,
                    "displayName": fam.get("displayName", ""),
                    "methodCount": method_count,
                    "obligations": _extract_obligations(fam),
                    "migrationStatus": "pending",
                    "lastVerified": "",
                }
                if update:
                    asm_entry["families"].append(entry)
                    print(f"[sync] ADD family: {slug} ({asm_name})")
                    added += 1
                else:
                    print(f"[sync] WOULD ADD: {slug} ({asm_name}) — {fam.get('displayName', '')}")
            else:
                new_count += 1
                # Check for mismatches
                if existing.get("methodCount", 0) != fam.get("methodCount", 0):
                    mismatches.append(f"{slug}: methodCount {existing.get('methodCount')} vs {fam.get('methodCount')}")

    if update:
        _NEW_LEDGER_PATH.write_text(json.dumps(new, indent=2, ensure_ascii=False) + "\n", encoding="utf-8")
        print(f"\n[sync] Updated: {added} families added")
    else:
        print(f"\n[sync] Dry-run: {added} families would be added")

    if mismatches:
        print(f"\n[sync] MISMATCHES ({len(mismatches)}):")
        for m in mismatches:
            print(f"  {m}")

    print(f"\n[sync] Old: {old_count} families, New: {new_count} families (matching)")


def _extract_obligations(fam: dict) -> list[str]:
    """Extract active obligations from a family entry."""
    obligations = []
    gates = fam.get("verificationGates", {})
    if fam.get("functionalObligation", {}).get("required"):
        obligations.append("functional")
    if fam.get("performanceObligation", {}).get("required"):
        obligations.append("performance")
    if fam.get("hotupdateFunctionalObligation", {}).get("required"):
        obligations.append("hotupdate_functional")
    if fam.get("hotupdatePerformanceObligation", {}).get("required"):
        obligations.append("hotupdate_performance")
    return obligations


if __name__ == "__main__":
    dry_run = "--update" not in sys.argv and "--verify" not in sys.argv
    verify = "--verify" in sys.argv
    update = "--update" in sys.argv
    sync(dry_run=dry_run, update=update, verify=verify)
