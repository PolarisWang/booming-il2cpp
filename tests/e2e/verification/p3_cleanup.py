"""P3 cleanup script — removes old family directories and pipeline files.

This script is SAFE to run only AFTER the new chunk pipeline has been
verified end-to-end on System.Private.CoreLib.  It:

  1. Deletes old family directories (65 families under System.Private.CoreLib/)
  2. Deletes old pipeline stage files (codegen.py, test_code_generator.py, etc.)
  3. Cleans up _contracts/, results/, and other redundant directories

Usage:
    # Dry run (show what would be deleted)
    python verification/p3_cleanup.py --dry-run

    # Actually delete
    python verification/p3_cleanup.py
"""

from __future__ import annotations

import argparse
import shutil
import sys
from pathlib import Path

_HERE = Path(__file__).resolve().parent  # verification/
import sys as _b_sys
from pathlib import Path as _b_Path
for _b_d in _b_Path(__file__).resolve().parents:
    if (_b_d / "_path.py").exists():
        if str(_b_d) not in _b_sys.path:
            _b_sys.path.insert(0, str(_b_d))
        break
from _path import foundation_root

_FOUNDATION_DLL = foundation_root()# testing/foundation-dll/


def main():
    parser = argparse.ArgumentParser(description="P3 cleanup script")
    parser.add_argument("--dry-run", action="store_true",
                        help="Dry run — show what would be deleted without deleting")
    parser.add_argument("--assembly", default="System.Private.CoreLib",
                        help="Target assembly (default: System.Private.CoreLib)")
    args = parser.parse_args()

    dry_run = args.dry_run
    assembly_dir = _FOUNDATION_DLL / args.assembly
    deleted_count = 0
    errors = 0

    # ── 1. Old family directories ──
    # All directories under the assembly dir EXCEPT: chunks/, _dll/, .autogen/
    print(f"\n{'='*60}")
    print(f"P3 Cleanup: {args.assembly}")
    print(f"{'='*60}")
    print(f"\n[1/3] Old family directories...")

    if not assembly_dir.is_dir():
        print(f"ERROR: Assembly directory not found: {assembly_dir}")
        return 1

    for d in sorted(assembly_dir.iterdir()):
        if not d.is_dir():
            continue
        name = d.name
        if name in ("chunks", "_dll", ".autogen"):
            continue

        print(f"  DELETE {d.relative_to(_FOUNDATION_DLL)}/")
        if not dry_run:
            try:
                shutil.rmtree(d)
                deleted_count += 1
            except OSError as e:
                print(f"  ERROR: {e}")
                errors += 1

    # ── 2. Old pipeline stage files ──
    print(f"\n[2/3] Old pipeline stage files...")

    old_stage_files = [
        _HERE / "stages" / "codegen.py",
        _HERE / "stages" / "pre_verification_audit.py",
        _HERE / "stages" / "subject_correctness_audit.py",
        _HERE / "stages" / "pipeline_native_aot_runner.py",
        _HERE / "stages" / "test_code_generator.py",
        _HERE / "stages" / "native_code_generator.py",
        _HERE / "orchestration" / "family_entrypoint.py",
    ]

    for f in old_stage_files:
        if f.exists():
            print(f"  DELETE {f.relative_to(_FOUNDATION_DLL)}")
            if not dry_run:
                try:
                    f.unlink()
                    deleted_count += 1
                except OSError as e:
                    print(f"  ERROR: {e}")
                    errors += 1
        else:
            print(f"  (not found) {f.relative_to(_FOUNDATION_DLL)}")

    # ── 3. Redundant directories ──
    print(f"\n[3/3] Redundant directories...")

    redundant_dirs = [
        _FOUNDATION_DLL / "_contracts",
        _FOUNDATION_DLL / "results",
    ]

    for d in redundant_dirs:
        if d.exists():
            print(f"  DELETE {d.relative_to(_FOUNDATION_DLL)}/")
            if not dry_run:
                try:
                    shutil.rmtree(d)
                    deleted_count += 1
                except OSError as e:
                    print(f"  ERROR: {e}")
                    errors += 1
        else:
            print(f"  (not found) {d.relative_to(_FOUNDATION_DLL)}/")

    # ── Summary ──
    print(f"\n{'='*60}")
    if dry_run:
        print(f"Dry run complete. Would delete {deleted_count} items.")
        print(f"Run without --dry-run to actually delete.")
    else:
        print(f"Cleanup complete: {deleted_count} items deleted.")
    if errors:
        print(f"Errors: {errors}")

    return 0 if errors == 0 else 1


if __name__ == "__main__":
    sys.exit(main())
