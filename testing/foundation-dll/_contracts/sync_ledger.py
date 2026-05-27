#!/usr/bin/env python3
"""
sync_ledger.py — Synchronize ledger.json with disk state.

Scans testing/foundation-dll/<assembly>/<family>/contract.json for all families,
compares against _contracts/ledger.json, and applies:
  1. Add new assemblies and families found on disk
  2. Update methodSubjectCount for existing families
  3. Remove junk directories (--family, =--family, etc.)

Usage:
  python _contracts/sync_ledger.py          # apply sync
  python _contracts/sync_ledger.py --check  # dry-run, report only
"""

import json
import os
import sys
import shutil

PROJECT_ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
CONTRACTS_DIR = os.path.join(PROJECT_ROOT, "_contracts")
LEDGER_PATH = os.path.join(CONTRACTS_DIR, "ledger.json")

JUNK_PREFIXES = ("--family", "=--family", "Xfamily")


def load_json(path: str):
    with open(path, "r", encoding="utf-8") as f:
        return json.load(f)


def save_json(path: str, data):
    with open(path, "w", encoding="utf-8") as f:
        json.dump(data, f, indent=2, ensure_ascii=False)
        f.write("\n")


def scan_disk() -> dict:
    """Scan testing/foundation-dll for all contract.json files.

    Returns nested dict: {assembly_name: {slug: contract_dict}}
    """
    result = {}
    for d in sorted(os.listdir(PROJECT_ROOT)):
        path = os.path.join(PROJECT_ROOT, d)
        if not os.path.isdir(path):
            continue
        if d.startswith("_") or d.startswith(".") or d == "verification":
            continue
        if any(d.startswith(p) for p in JUNK_PREFIXES):
            continue
        families = {}
        for root, _dirs, files in os.walk(path):
            if "contract.json" in files:
                slug = os.path.basename(root)
                if slug and not slug.startswith("_"):
                    families[slug] = load_json(os.path.join(root, "contract.json"))
        if families:
            result[d] = families
    return result


def sync(check_only: bool = False) -> int:
    """Sync ledger with disk. Returns count of changes needed."""
    ledger = load_json(LEDGER_PATH)
    disk = scan_disk()
    changes = 0

    ledger_asm_map = {a["assemblyName"]: a for a in ledger["assemblies"]}

    # ── Clean junk directories ──
    for d in sorted(os.listdir(PROJECT_ROOT)):
        if any(d.startswith(p) for p in JUNK_PREFIXES):
            path = os.path.join(PROJECT_ROOT, d)
            if os.path.isdir(path):
                changes += 1
                if check_only:
                    print(f"  WOULD CLEAN: {d}")
                else:
                    shutil.rmtree(path)
                    print(f"  CLEANED: {d}")

    # ── Sync assemblies ──
    for aname in sorted(disk.keys()):
        disk_fams = disk[aname]

        if aname in ledger_asm_map:
            # Update existing assembly
            ledger_fam_map = {f["slug"]: f for f in ledger_asm_map[aname]["families"]}

            for slug in sorted(disk_fams.keys()):
                contract = disk_fams[slug]
                dmc = contract.get("methodSubjectCount") or 0
                did = contract.get("familyId") or f"family/{aname}/{slug}"
                dname = contract.get("displayName") or slug

                if slug in ledger_fam_map:
                    lf = ledger_fam_map[slug]
                    if lf["methodCount"] != dmc or lf.get("familyId") != did:
                        changes += 1
                        if check_only:
                            print(f"  WOULD UPDATE: {aname}/{slug}: methodCount {lf['methodCount']}->{dmc}")
                        else:
                            print(f"  UPDATE: {aname}/{slug}: methodCount {lf['methodCount']}->{dmc}")
                            lf["methodCount"] = dmc
                            lf["familyId"] = did
                else:
                    changes += 1
                    if check_only:
                        print(f"  WOULD ADD: {aname}/{slug} ({dmc} methods)")
                    else:
                        print(f"  ADD: {aname}/{slug} ({dmc} methods)")
                        ledger_asm_map[aname]["families"].append({
                            "familyId": did,
                            "slug": slug,
                            "displayName": dname,
                            "methodCount": dmc,
                            "obligations": ["functional"],
                            "migrationStatus": "imported",
                            "lastVerified": "",
                        })
        else:
            # New assembly
            fams = []
            for slug in sorted(disk_fams.keys()):
                contract = disk_fams[slug]
                dmc = contract.get("methodSubjectCount") or 0
                did = contract.get("familyId") or f"family/{aname}/{slug}"
                dname = contract.get("displayName") or slug
                fams.append({
                    "familyId": did,
                    "slug": slug,
                    "displayName": dname,
                    "methodCount": dmc,
                    "obligations": ["functional"],
                    "migrationStatus": "imported",
                    "lastVerified": "",
                })
                changes += 1
                if check_only:
                    print(f"  WOULD ADD ASSEMBLY: {aname}/{slug} ({dmc} methods)")
                else:
                    print(f"  NEW ASSEMBLY: {aname}/{slug} ({dmc} methods)")

            if not check_only:
                ledger["assemblies"].append({"assemblyName": aname, "families": fams})
            else:
                print(f"  WOULD ADD ASSEMBLY GROUP: {aname} ({len(fams)} families)")

    if not check_only:
        # Sort assemblies by name
        ledger["assemblies"].sort(key=lambda a: a["assemblyName"])
        save_json(LEDGER_PATH, ledger)

    return changes


if __name__ == "__main__":
    check_only = "--check" in sys.argv
    label = "CHECK" if check_only else "SYNC"
    changes = sync(check_only=check_only)

    if changes:
        print(f"\n{label}: {changes} change(s) {'would be' if check_only else 'applied'}")
    else:
        print(f"\n{label}: ledger is already in sync with disk (0 changes)")
