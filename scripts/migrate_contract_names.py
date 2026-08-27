"""Migrate contract.json data into capability-family-contract.json and remove legacy files.

This one-time migration:
1. Merges customEntryIndices from contract.json into capability-family-contract.json
   (only when the primary has None or empty customEntryIndices and the legacy has data).
2. Deletes the legacy contract.json afterwards.
3. Updates all Python lookup code to point to the single canonical path.

Usage:
    python scripts/migrate_contract_names.py          # dry-run (preview only)
    python scripts/migrate_contract_names.py --apply   # actually migrate + convert
"""

from __future__ import annotations

import json
import shutil
import sys
from pathlib import Path

_REPO_ROOT = Path(__file__).resolve().parents[1]
_TESTING = _REPO_ROOT / "testing" / "foundation-dll"


def _find_legacy_pairs() -> list[tuple[Path, Path]]:
    """Return list of (legacy_contract_json, capability_contract_json) pairs."""
    pairs: list[tuple[Path, Path]] = []
    for legacy in sorted(_TESTING.rglob("contract.json")):
        if "results" in str(legacy) or "node_modules" in str(legacy):
            continue
        cap = legacy.parent / "capability-family-contract.json"
        if cap.exists():
            pairs.append((legacy, cap))
    return pairs


def migrate_contracts(dry_run: bool = True) -> int:
    """Migrate customEntryIndices from legacy to capability contract files.

    Returns count of files that would be / were modified.
    """
    pairs = _find_legacy_pairs()
    print(f"Found {len(pairs)} legacy contract.json files with companion capability files")

    migrated = 0
    for legacy_path, cap_path in pairs:
        try:
            legacy = json.loads(legacy_path.read_text(encoding="utf-8"))
        except (json.JSONDecodeError, OSError) as e:
            print(f"  SKIP {legacy_path.parent.name}: cannot read legacy ({e})")
            continue

        try:
            primary = json.loads(cap_path.read_text(encoding="utf-8"))
        except (json.JSONDecodeError, OSError) as e:
            print(f"  SKIP {cap_path.parent.name}: cannot read capability ({e})")
            continue

        leg_indices = legacy.get("customEntryIndices")
        pri_indices = primary.get("customEntryIndices")

        # Only merge if primary is missing or empty and legacy has data
        need_merge = (
            leg_indices is not None
            and len(leg_indices) > 0
            and (pri_indices is None or len(pri_indices) == 0)
        )

        if need_merge:
            slug = cap_path.parent.name
            print(f"  MERGE {slug}: {len(leg_indices)} indices into capability file")
            if not dry_run:
                primary["customEntryIndices"] = leg_indices
                cap_path.write_text(
                    json.dumps(primary, indent=2, ensure_ascii=False) + "\n",
                    encoding="utf-8",
                )
            migrated += 1

        # Delete legacy file after merge
        if not dry_run:
            backup = legacy_path.with_suffix(".json.bak")
            if not backup.exists():
                shutil.move(str(legacy_path), str(backup))
                print(f"    -> renamed contract.json -> contract.json.bak for {slug}")
            else:
                legacy_path.unlink()
                print(f"    -> deleted contract.json for {slug}")

    if dry_run:
        print(f"\nWould migrate {migrated} file(s) and delete all legacy contract.json files.")
        print("Run with --apply to execute.")
    else:
        remaining = len(_find_legacy_pairs())
        print(f"\nMigrated {migrated} file(s). Remaining legacy files: {remaining}")

    return migrated


def main() -> None:
    dry_run = "--apply" not in sys.argv
    migrate_contracts(dry_run=dry_run)


if __name__ == "__main__":
    main()
