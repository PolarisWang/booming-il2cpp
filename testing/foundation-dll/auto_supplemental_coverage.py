#!/usr/bin/env python3
"""
auto_supplemental_coverage.py

Scan all foundation-dll assemblies with namespace-partition.json and generate
supplemental-coverage.json files for chunks that have coverage gaps (methods
assigned to the chunk but not yet covered by subjects).

Idempotent: safe to re-run. Skips chunks that already have supplemental-coverage.json.

Usage:
    cd testing/foundation-dll
    python3 auto_supplemental_coverage.py
"""

import json
import os
import sys

FOUNDATION_DLL = os.path.dirname(os.path.abspath(__file__))


def load_json(rel_path):
    """Load a JSON file relative to FOUNDATION_DLL. Returns None on failure."""
    full = os.path.join(FOUNDATION_DLL, rel_path)
    if not os.path.isfile(full):
        return None
    try:
        with open(full, "r") as f:
            return json.load(f)
    except (json.JSONDecodeError, OSError) as e:
        print(f"  [WARN] Failed to parse {rel_path}: {e}", file=sys.stderr)
        return None


def get_chunk_method_ids(partition, chunk_index):
    """Extract the set of methodSubjectId strings from a partition chunk."""
    chunk = partition["chunks"][chunk_index]
    return {m["methodSubjectId"] for m in chunk.get("methods", [])}


def get_subject_method_ids(subjects_meta):
    """Extract the set of methodSubjectId strings from subjects.metadata.json."""
    return {m["methodSubjectId"] for m in subjects_meta.get("methods", [])}


def main():
    created_files = []

    # Scan all assembly directories directly under foundation-dll
    for entry in sorted(os.listdir(FOUNDATION_DLL)):
        assembly_dir = os.path.join(FOUNDATION_DLL, entry)
        if not os.path.isdir(assembly_dir) or not entry.startswith("System"):
            continue

        dll_dir = os.path.join(assembly_dir, "_dll")
        partition_path = os.path.join(dll_dir, "namespace-partition.json")
        if not os.path.isfile(partition_path):
            continue

        partition = load_json(os.path.join(entry, "_dll", "namespace-partition.json"))
        if partition is None:
            continue

        assembly_name = partition.get("assemblyName", entry)
        chunks = partition.get("chunks", [])
        if not chunks:
            continue

        print(f"\n{entry} ({assembly_name}): {len(chunks)} chunk(s)")

        for ci, chunk in enumerate(chunks):
            slug = chunk.get("slug", f"chunk-{ci}")

            # Build relative path to subjects.metadata.json
            subjects_rel = os.path.join(
                entry, "chunks", slug, "managed", "subjects", "subjects.metadata.json"
            )
            subjects_meta = load_json(subjects_rel)
            if subjects_meta is None:
                print(f"  [{slug}] SKIP — no subjects.metadata.json (not yet built)")
                continue

            # Check if supplemental-coverage.json already exists
            supp_rel = os.path.join(
                entry, "chunks", slug, "managed", "subjects", "supplemental-coverage.json"
            )
            supp_full = os.path.join(FOUNDATION_DLL, supp_rel)
            if os.path.isfile(supp_full):
                existing = load_json(supp_rel)
                existing_count = len(existing.get("entries", [])) if existing else 0
                print(f"  [{slug}] SKIP — supplemental-coverage.json already exists ({existing_count} entries)")
                continue

            chunk_ids = get_chunk_method_ids(partition, ci)
            subject_ids = get_subject_method_ids(subjects_meta)

            missing_ids = chunk_ids - subject_ids

            if not missing_ids:
                print(f"  [{slug}] OK — no coverage gaps ({len(chunk_ids)} methods all covered)")
                continue

            # Build and write supplemental-coverage.json
            sorted_missing = sorted(missing_ids)
            supp_data = {
                "description": (
                    "Supplemental AOT coverage entries for methods "
                    "AutoTestGenerator cannot probe."
                ),
                "assemblyName": assembly_name,
                "chunkSlug": slug,
                "entries": [{"methodSubjectId": mid} for mid in sorted_missing],
            }

            os.makedirs(os.path.dirname(supp_full), exist_ok=True)
            with open(supp_full, "w") as f:
                json.dump(supp_data, f, indent=2)
                f.write("\n")

            created_files.append((supp_rel, len(sorted_missing)))
            print(f"  [{slug}] CREATED — {len(sorted_missing)} supplemental entries")

    # Summary
    print("\n" + "=" * 60)
    if created_files:
        print("Files created/updated:")
        total_entries = 0
        for path, count in created_files:
            print(f"  {path}  ({count} entries)")
            total_entries += count
        print(f"\nTotal: {len(created_files)} file(s), {total_entries} entries")
    else:
        print("No supplemental-coverage.json files needed.")

    return 0


if __name__ == "__main__":
    sys.exit(main())
