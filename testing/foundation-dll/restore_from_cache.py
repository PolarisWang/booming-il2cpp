#!/usr/bin/env python3
"""Restore all projects from hephaestus cache.

For each project under testing/foundation-dll/ that has a .hephaestus-cache/
directory, restore the latest valid cache entry for each chunk to that
chunk's native/ directory.

Usage:
    python testing/foundation-dll/restore_from_cache.py
    python testing/foundation-dll/restore_from_cache.py --dry-run
    python testing/foundation-dll/restore_from_cache.py --verbose
"""

from __future__ import annotations

import argparse
import json
import shutil
import sys
from pathlib import Path

FOUNDATION_DLL = Path(__file__).resolve().parent
CACHE_DIR_NAME = ".hephaestus-cache"
MANIFEST_NAME = "manifest.json"

# Directories that are NOT test project families
SKIP_DIRS = {
    "_batch_hotupdate_audit_results.json", "_batch_rebuild.py", "_contracts",
    "_hotupdate_results.json", "_run_all_hotupdate.py", "_sdk",
    "artifacts", "batch-results.md", "batch_manifest.py", "check_subject_ids.py",
    "ci-benchmark-20260616-0943.log", "ci-medium-20260615-2056.log",
    "ci-medium-20260615-2103.log", "ci-run-all.sh", "ci-run-small.sh",
    "ci-small-results-20260615-2004.log", "compare_benchmarks.py",
    "config", "corelib-20260615-2340.log", "crypto-refs", "fix_per_asm.py",
    "inject_getui.py", "inject_profile_range.py", "interpreter_entry.cpp.o",
    "lib", "Makefile", "nightly-build-report", "results", "run_parallel_benchmark.py",
    "sdk", "src", "testing", "tests", "tpg_build.bat", "tpg_test.bat", "verification",
}


def get_latest_valid_entries(manifest_path: Path) -> dict[str, dict]:
    """Return {chunk_slug: newest_valid_entry} from the manifest."""
    if not manifest_path.exists():
        return {}

    try:
        data = json.loads(manifest_path.read_text(encoding="utf-8"))
    except json.JSONDecodeError as e:
        print(f"      WARNING: corrupt manifest {manifest_path.name}: {e}")
        return {}

    entries = data.get("entries", [])

    # Group by chunk_slug, keep only valid entries (newest first in manifest)
    by_chunk: dict[str, dict] = {}
    for entry in entries:
        if entry.get("status") != "valid":
            continue
        slug = entry.get("chunk_slug")
        if not slug:
            continue
        if slug not in by_chunk:
            by_chunk[slug] = entry

    return by_chunk


def verify_cache_entry(entry_dir: Path) -> bool:
    """Check that the cache directory exists and has entry.exe."""
    if not entry_dir.is_dir():
        return False
    exe = entry_dir / "entry.exe"
    if not (exe.exists() and exe.stat().st_size > 0):
        print(f"      WARNING: {entry_dir.name} has no entry.exe")
        return False
    return True


def restore_entry(entry_dir: Path, target_dir: Path) -> int:
    """Copy all files from entry_dir to target_dir. Returns file count."""
    if not target_dir.parent.exists():
        print(f"      WARNING: chunk dir {target_dir.parent} doesn't exist, skipping")
        return 0

    # Remove target for clean restore
    if target_dir.exists():
        shutil.rmtree(target_dir)
    target_dir.mkdir(parents=True, exist_ok=True)

    file_count = 0
    for item in entry_dir.iterdir():
        dest = target_dir / item.name
        if item.is_dir():
            shutil.copytree(item, dest, symlinks=True, dirs_exist_ok=True)
        else:
            shutil.copy2(item, dest)
        file_count += 1

    return file_count


def main() -> int:
    parser = argparse.ArgumentParser(
        description="Restore all projects from hephaestus cache")
    parser.add_argument("--dry-run", action="store_true",
                        help="Show what would be done without doing it")
    parser.add_argument("--verbose", action="store_true",
                        help="Verbose output including skipped projects")
    parser.add_argument("--project", default=None,
                        help="Restore only a specific project (directory name)")
    args = parser.parse_args()

    restored = 0
    skipped = 0
    failed = 0

    for entry in sorted(FOUNDATION_DLL.iterdir()):
        if not entry.is_dir() or entry.name in SKIP_DIRS:
            continue
        if args.project and entry.name != args.project:
            continue

        cache_dir = entry / CACHE_DIR_NAME
        if not cache_dir.is_dir():
            if args.verbose:
                print(f"[SKIP] {entry.name} — no .hephaestus-cache")
            skipped += 1
            continue

        manifest_path = cache_dir / MANIFEST_NAME
        chunks = get_latest_valid_entries(manifest_path)

        if not chunks:
            if args.verbose:
                print(f"[SKIP] {entry.name} — no valid entries in manifest")
            skipped += 1
            continue

        for slug, metadata in chunks.items():
            cache_key = metadata.get("cache_key", "")
            dir_name = cache_key.replace("/", "_")
            entry_dir = cache_dir / dir_name

            if not verify_cache_entry(entry_dir):
                print(f"[FAIL] {entry.name}/{slug} — cache entry missing or corrupt ({dir_name})")
                failed += 1
                continue

            target_dir = entry / "chunks" / slug / "native"

            if args.dry_run:
                entry_count = metadata.get("entry_count", "?")
                exe_size = metadata.get("entry_exe_size", 0)
                print(f"[DRY] {entry.name}/{slug} — {entry_count} files, "
                      f"{exe_size // 1024}KB entry.exe → {target_dir}")
                restored += 1
                continue

            try:
                file_count = restore_entry(entry_dir, target_dir)
                exe_size = metadata.get("entry_exe_size", 0)
                print(f"[ OK] {entry.name}/{slug} — {file_count} files, "
                      f"{exe_size // 1024}KB entry.exe")
                restored += 1
            except Exception as e:
                print(f"[FAIL] {entry.name}/{slug} — {e}")
                failed += 1

    print(f"\n=== Summary: {restored} restored, {skipped} skipped, {failed} failed ===")
    return 0 if failed == 0 else 1


if __name__ == "__main__":
    sys.exit(main())
