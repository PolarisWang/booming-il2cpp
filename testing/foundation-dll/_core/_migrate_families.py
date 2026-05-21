#!/usr/bin/env python3
"""Migrate non-CoreLib families from verification/ to testing/ framework.

Copies essential source files (excluding build artifacts) and renames
capability-family-contract.json -> contract.json for the new pipeline.

Usage: python _migrate_families.py [--dry-run]
"""

import json
import shutil
import sys
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parents[3]
VERIFICATION = REPO_ROOT / "verification" / "foundation-dll"
TESTING = REPO_ROOT / "testing" / "foundation-dll"

# Assemblies to migrate (those missing from testing/ or stub-only)
ASSEMBLIES_TO_MIGRATE = [
    "System.Collections.Immutable",
    "System.Linq",
    "System.Formats.Asn1",
    "System.IO.Compression.Brotli",
    "System.IO.Compression.ZipFile",
    "System.IO.Pipelines",
    "System.Net.ServerSentEvents",
    "System.ObjectModel",
    "System.Runtime.InteropServices",
    "System.Runtime.Serialization.Formatters",
    "System.Security.Principal.Windows",
    "System.Text.Json",
    "System.Threading.Tasks.Parallel",
]

# Directories/files to exclude (build artifacts, binaries, reports)
EXCLUDE_DIRS = {"obj", "build-output", "build", "bin"}
EXCLUDE_FILE_EXTS = {".dll", ".pdb", ".exe", ".recipe", ".tlog", ".ilk"}
EXCLUDE_TOPICS = {"reports", "reports_generated"}
# Old framework report files (not needed in new testing framework)
EXCLUDE_REPORT_FILES = {
    "unified-verification-report.json",
    "benchmark-comparison-report.json",
    "hotupdate-verification-report.json",
    "hotupdate-aot-benchmark-report.json",
    "hotupdate-jit-benchmark-report.json",
    "hotupdate-jit-verification-report.json",
    "interpreter-microbench-report.json",
    "mechanism-audit-report.json",
    "post-hotupdate-benchmark-report.json",
    "principle-alignment-report.json",
    "native-benchmark.json",
}


def _should_copy(path: Path, rel_path: str) -> bool:
    """Check if a file should be copied (not build artifact)."""
    parts = rel_path.replace("\\", "/").split("/")
    # Exclude build artifact dirs at any depth
    for part in parts[:-1]:
        if part in EXCLUDE_DIRS:
            return False
    # Exclude by extension
    if path.suffix in EXCLUDE_FILE_EXTS:
        return False
    # Exclude old framework report files
    if path.name in EXCLUDE_REPORT_FILES:
        return False
    # Exclude generated build timestamp files
    if path.name.endswith(".lastbuildstate"):
        return False
    if path.name in ("CMakeCache.txt", "cmake_install.cmake", "generate.stamp",
                      "generate.stamp.depend", "generate.stamp.list",
                      "generate.stamp.rule", "TargetDirectories.txt",
                      "CMakeOutput.log", "cmake.check_cache",
                      "VCTargetsPath.txt", "chaos_entry.sln",
                      ".vs"):
        return False
    return True


def migrate_topic(verif_topic: Path, test_topic: Path, dry_run: bool) -> bool:
    """Migrate a single topic from verification to testing. Returns True if copied."""
    if verif_topic.name in EXCLUDE_TOPICS:
        return False

    # Create target directory
    if not dry_run:
        test_topic.mkdir(parents=True, exist_ok=True)

    # Copy essential files recursively
    copied_count = 0
    for src_file in verif_topic.rglob("*"):
        if not src_file.is_file():
            continue
        rel_path = str(src_file.relative_to(verif_topic))
        if not _should_copy(src_file, rel_path):
            continue

        # Rename capability-family-contract.json -> contract.json
        if src_file.name == "capability-family-contract.json":
            dst_file = test_topic / "contract.json"
        else:
            dst_file = test_topic / rel_path

        if dry_run:
            print(f"  [dry-run] {rel_path}")
            copied_count += 1
            continue

        dst_file.parent.mkdir(parents=True, exist_ok=True)
        try:
            shutil.copy2(src_file, dst_file)
            copied_count += 1
        except Exception as e:
            print(f"  [ERROR] {rel_path}: {e}")

    if copied_count > 0:
        rel_name = str(test_topic.relative_to(TESTING))
        print(f"  {rel_name}: {copied_count} files")
    return copied_count > 0


def migrate_assembly(assembly: str, dry_run: bool) -> int:
    """Migrate all topics for an assembly. Returns topic count."""
    verif_assembly = VERIFICATION / assembly
    test_assembly = TESTING / assembly

    if not verif_assembly.is_dir():
        print(f"[SKIP] {assembly}: not found in verification/")
        return 0

    if not test_assembly.is_dir() and not dry_run:
        test_assembly.mkdir(parents=True, exist_ok=True)

    topic_count = 0
    for topic_dir in sorted(verif_assembly.iterdir()):
        if not topic_dir.is_dir():
            continue
        test_topic = test_assembly / topic_dir.name
        if migrate_topic(topic_dir, test_topic, dry_run):
            topic_count += 1

    return topic_count


def update_ledger(assembly: str, dry_run: bool) -> None:
    """Update the testing/ ledger.json with migrated families."""
    ledger_path = TESTING / "_contracts" / "ledger.json"
    if not ledger_path.exists():
        print(f"  [SKIP] ledger.json not found")
        return

    if dry_run:
        return

    with open(ledger_path, "r", encoding="utf-8") as f:
        ledger = json.load(f)

    # Check if assembly already in ledger
    existing = None
    for a in ledger["assemblies"]:
        if a["assemblyName"] == assembly:
            existing = a
            break

    if existing:
        print(f"  [SKIP] ledger: {assembly} already registered ({len(existing['families'])} families)")
        return

    # Find all topics with contract.json
    test_assembly = TESTING / assembly
    if not test_assembly.is_dir():
        return

    new_entry = {
        "assemblyName": assembly,
        "families": []
    }

    for topic_dir in sorted(test_assembly.iterdir()):
        contract_path = topic_dir / "contract.json"
        if not topic_dir.is_dir() or not contract_path.exists():
            continue
        try:
            with open(contract_path, "r", encoding="utf-8") as f:
                contract = json.load(f)
            new_entry["families"].append({
                "familyId": contract.get("familyId", ""),
                "slug": topic_dir.name,
                "displayName": contract.get("displayName", topic_dir.name),
                "methodCount": contract.get("methodSubjectCount", 0),
                "obligations": ["functional"],
                "migrationStatus": "migrated",
                "lastVerified": ""
            })
        except (json.JSONDecodeError, KeyError) as e:
            print(f"  [WARN] ledger: {topic_dir.name} contract parse error: {e}")

    if new_entry["families"]:
        ledger["assemblies"].append(new_entry)
        with open(ledger_path, "w", encoding="utf-8") as f:
            json.dump(ledger, f, indent=2, ensure_ascii=False)
        print(f"  ledger: added {assembly} ({len(new_entry['families'])} families)")


def main():
    dry_run = "--dry-run" in sys.argv

    if dry_run:
        print("=== DRY RUN MODE ===\n")

    total_topics = 0
    for assembly in ASSEMBLIES_TO_MIGRATE:
        print(f"\n{'='*60}")
        print(f"Assembly: {assembly}")
        print(f"{'='*60}")
        count = migrate_assembly(assembly, dry_run)
        if count > 0:
            update_ledger(assembly, dry_run)
        total_topics += count

    print(f"\n{'='*60}")
    print(f"Total: {total_topics} topics migrated across {len(ASSEMBLIES_TO_MIGRATE)} assemblies")
    if dry_run:
        print("(dry run - no files copied)")
    else:
        print("Done. Run 'python _migrate_families.py --dry-run' to preview.")


if __name__ == "__main__":
    main()
