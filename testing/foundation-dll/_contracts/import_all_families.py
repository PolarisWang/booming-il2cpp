"""Batch import all 69 capability families from verification/ to testing/.

Creates family directories under testing/foundation-dll/<assembly>/<slug>/,
copies contract data, and updates the ledger.json.

Usage:
    python testing/foundation-dll/_contracts/import_all_families.py
"""

from __future__ import annotations

import json
import sys
from pathlib import Path

_REPO_ROOT = Path(__file__).resolve().parents[3]  # → agent/booming-il2cpp
_VERIFICATION = _REPO_ROOT / "verification" / "foundation-dll"
_TESTING = _REPO_ROOT / "testing" / "foundation-dll"
_LEDGER_PATH = _TESTING / "_contracts" / "ledger.json"


def _slug_from_family_id(family_id: str) -> str:
    """Derive directory slug from family ID.

    'family/System.Private.CoreLib/convert/char' → 'convert-char'
    'family/System.Private.CoreLib/array/indexing-copy' → 'array-indexing-copy'
    """
    parts = family_id.split("/")
    # Skip "family/<assembly>/" prefix → take remaining parts and join
    # family/System.Private.CoreLib/convert/char → ['convert', 'char'] → 'convert-char'
    # family/SnapshotTestFixtures/snapshot/prover → ['snapshot', 'prover'] → 'snapshot-prover'
    slug_parts = parts[2:]  # skip 'family' and assembly name
    return "-".join(slug_parts)


def import_families() -> None:
    assemblies = {}

    # Scan all assemblies in verification/
    for asm_dir in sorted(_VERIFICATION.iterdir()):
        if not asm_dir.is_dir():
            continue
        assembly = asm_dir.name

        families = []
        for family_dir in sorted(asm_dir.iterdir()):
            if not family_dir.is_dir():
                continue
            slug = family_dir.name
            contract_path = family_dir / "capability-family-contract.json"
            if not contract_path.exists():
                continue

            # Read verification contract
            with open(contract_path, encoding="utf-8") as f:
                contract = json.load(f)

            family_id = contract.get("familyId", f"family/{assembly}/{slug.replace('-', '/')}")
            method_ids = contract.get("methodSubjectIds", [])
            display_name = contract.get("displayName", slug.replace("-", " ").title())
            method_count = contract.get("methodSubjectCount", len(method_ids))

            # Create testing/ family directory
            test_family_dir = _TESTING / assembly / slug
            test_family_dir.mkdir(parents=True, exist_ok=True)

            # Write contract.json (same content, renamed)
            test_contract_path = test_family_dir / "contract.json"
            # Strip methodContracts if present (not needed in new framework)
            contract_out = {k: v for k, v in contract.items() if k != "methodContracts"}
            with open(test_contract_path, "w", encoding="utf-8") as f:
                json.dump(contract_out, f, indent=2, ensure_ascii=False)
                f.write("\n")

            # Copy handwritten/ directory if present (partial class files, entry points)
            handwritten_src = family_dir / "handwritten"
            if handwritten_src.exists():
                import shutil
                handwritten_dst = test_family_dir / "handwritten"
                if handwritten_dst.exists():
                    shutil.rmtree(handwritten_dst)
                shutil.copytree(handwritten_src, handwritten_dst)
                print(f"  [OK] {assembly}/{slug} ({method_count} methods, +handwritten)")

            families.append({
                "familyId": family_id,
                "slug": slug,
                "displayName": display_name,
                "methodCount": method_count,
                "obligations": ["functional"],
                "migrationStatus": "imported",
                "lastVerified": "",
            })

        if families:
            assemblies[assembly] = families
            print(f"  → {len(families)} families imported for {assembly}")

    # Update ledger.json
    ledger = {
        "schemaVersion": "2.0",
        "description": "Foundation DLL capability family ledger — new framework. Synchronized from verification/ contracts.",
        "assemblies": [
            {
                "assemblyName": asm,
                "families": fams,
            }
            for asm, fams in sorted(assemblies.items())
        ],
    }
    _LEDGER_PATH.parent.mkdir(parents=True, exist_ok=True)
    with open(_LEDGER_PATH, "w", encoding="utf-8") as f:
        json.dump(ledger, f, indent=2, ensure_ascii=False)
        f.write("\n")

    total = sum(len(fams) for fams in assemblies.values())
    print(f"\n{'='*60}")
    print(f"Import complete: {total} families across {len(assemblies)} assemblies")
    print(f"Ledger: {_LEDGER_PATH}")


if __name__ == "__main__":
    import_families()
