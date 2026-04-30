"""HotUpdate verification runner — maps global C++ test results to per-family
hotupdate-verification-report.json files for dashboard injection.

The global C++ test (chaos_hotupdate_verification_test) tests 18 synthetic methods
and produces a JSON result with per-index entries (methodToken, status, etc.).
Each family's method-hotupdate-case-index.json lists hotupdate cases with
methodSubjectIds. This runner maps C++ results by index to unique methodSubjectIds
and writes per-family reports.

Usage:
    python hotupdate_verification_runner.py ^
        --input verification/catalog/programs/ci/hotupdate-verification-output.json ^
        --output-dir verification/foundation-dll
"""

from __future__ import annotations

import argparse
import json
import sys
from collections import OrderedDict
from pathlib import Path
from typing import Any


def _load_json(path: Path) -> dict[str, Any]:
    if not path.exists():
        print(f"ERROR: {path} not found", file=sys.stderr)
        sys.exit(1)
    with open(path, encoding="utf-8") as f:
        return json.load(f)


def _write_json(path: Path, data: Any) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    with open(path, "w", encoding="utf-8") as f:
        json.dump(data, f, indent=2, ensure_ascii=False)
        f.write("\n")


def _deduplicate_by_method_subject_id(cases: list[dict[str, Any]]) -> list[str]:
    """Return ordered unique methodSubjectIds from hotupdate case index entries."""
    seen: set[str] = set()
    result: list[str] = []
    for case in cases:
        sid = str(case.get("methodSubjectId") or "")
        if sid and sid not in seen:
            seen.add(sid)
            result.append(sid)
    return result


def _slug_from_family_id(family_id: str) -> str:
    """Derive directory slug from family ID (matches _slug_from_family_id in native_codegen_generator.py)."""
    value = str(family_id)
    if not value:
        return ""
    parts = value.split("/")
    if len(parts) >= 4:
        return "-".join(part.replace("_", "-") for part in parts[2:])
    return parts[-1].replace("_", "-")


def generate_per_family_reports(
    input_path: Path,
    verification_root: Path,
    assembly_name: str,
    *,
    family_ids: list[str] | None = None,
) -> dict[str, Any]:
    """Load global hotupdate output and generate per-family reports.

    Args:
        input_path: Path to the global hotupdate-verification-output.json.
        verification_root: Root of the verification directory
            (e.g., repo_root / "verification").
        assembly_name: Assembly name (e.g., "System.Private.CoreLib").
        family_ids: If provided, only process these families (by familyId).
            Otherwise scan all families under verification_root / assembly_name.

    Returns:
        Dict mapping familyId -> report dict.
    """
    global_data = _load_json(input_path)
    global_results: list[dict[str, Any]] = list(global_data.get("results") or [])

    print(f"Loaded global hotupdate output: {len(global_results)} methods from {input_path}")

    if family_ids is None:
        # Scan all families under the assembly directory
        assembly_dir = verification_root / "foundation-dll" / assembly_name
        if not assembly_dir.is_dir():
            print(f"ERROR: {assembly_dir} not found", file=sys.stderr)
            sys.exit(1)
        family_ids = []
        for slug_dir in sorted(assembly_dir.iterdir()):
            if slug_dir.is_dir():
                case_idx_path = slug_dir / "method-hotupdate-case-index.json"
                if case_idx_path.is_file():
                    # Reconstruct familyId from the directory
                    # We'll load it from the case index
                    pass
                    family_ids.append(str(slug_dir.name))

    reports: dict[str, Any] = {}

    # Scan families from the assembly directory
    assembly_dir = verification_root / "foundation-dll" / assembly_name
    if not assembly_dir.is_dir():
        print(f"ERROR: {assembly_dir} not found", file=sys.stderr)
        sys.exit(1)

    for slug_dir in sorted(assembly_dir.iterdir()):
        slug = slug_dir.name
        if not slug_dir.is_dir():
            continue

        case_idx_path = slug_dir / "method-hotupdate-case-index.json"
        if not case_idx_path.is_file():
            continue

        if family_ids and slug not in family_ids and slug not in [s.replace("/", "-") for s in family_ids]:
            continue

        case_idx = _load_json(case_idx_path)
        cases = list(case_idx.get("cases") or [])
        unique_ids = _deduplicate_by_method_subject_id(cases)

        if not unique_ids:
            print(f"  SKIP {slug}: no unique methodSubjectIds in case index")
            continue

        # Read familyId from the first case's familyId field
        family_id = str(cases[0].get("familyId") or "") if cases else slug
        if not family_id:
            family_id = f"family/{assembly_name}/{slug.replace('-', '/')}"

        # Map C++ results by index to unique methodSubjectIds
        method_results: list[dict[str, Any]] = []
        for i, sid in enumerate(unique_ids):
            if i < len(global_results):
                result = global_results[i]
                method_results.append({
                    "methodSubjectId": sid,
                    "status": str(result.get("status", "unmatched")),
                    "methodToken": result.get("methodToken"),
                    "originalReturnValue": result.get("originalReturnValue"),
                    "patchedReturnValue": result.get("patchedReturnValue"),
                    "expectedPatchedValue": result.get("expectedPatchedValue"),
                    "revertVerified": result.get("revertVerified", False),
                    "semanticVerified": result.get("semanticVerified", False),
                })
            else:
                # More unique IDs than C++ results — mark as unmatched
                method_results.append({
                    "methodSubjectId": sid,
                    "status": "unmatched",
                    "methodToken": None,
                    "originalReturnValue": None,
                    "patchedReturnValue": None,
                    "expectedPatchedValue": None,
                    "revertVerified": False,
                })

        # Count
        total = len(method_results)
        passed = sum(1 for r in method_results if r["status"] == "passed")
        failed = sum(1 for r in method_results if r["status"] == "failed")
        unmatched = sum(1 for r in method_results if r["status"] == "unmatched")

        report = {
            "schemaVersion": 1,
            "assemblyName": assembly_name,
            "familyId": family_id,
            "summary": {
                "totalMethods": total,
                "passedMethods": passed,
                "failedMethods": failed,
                "unmatchedMethods": unmatched,
            },
            "methodResults": method_results,
        }

        output_path = slug_dir / "hotupdate-verification-report.json"
        _write_json(output_path, report)

        print(f"  {slug}: {total} methods, {passed} passed, {failed} failed, {unmatched} unmatched -> {output_path}")
        reports[family_id] = report

    total_families = len(reports)
    total_passed = sum(r["summary"]["passedMethods"] for r in reports.values())
    total_failed = sum(r["summary"]["failedMethods"] for r in reports.values())
    total_unmatched = sum(r["summary"]["unmatchedMethods"] for r in reports.values())
    print(f"\nProcessed {total_families} families: {total_passed} passed, {total_failed} failed, {total_unmatched} unmatched total")

    return reports


def main() -> None:
    parser = argparse.ArgumentParser(description="Generate per-family hotupdate verification reports")
    parser.add_argument("--input", required=True, type=Path, help="Path to global hotupdate-verification-output.json")
    parser.add_argument("--verification-root", type=Path, default=Path.cwd() / "verification",
                        help="Root of the verification directory")
    parser.add_argument("--assembly-name", default="System.Private.CoreLib", help="Assembly name")
    args = parser.parse_args()

    generate_per_family_reports(
        input_path=args.input.resolve(),
        verification_root=args.verification_root.resolve(),
        assembly_name=args.assembly_name,
    )


if __name__ == "__main__":
    main()
