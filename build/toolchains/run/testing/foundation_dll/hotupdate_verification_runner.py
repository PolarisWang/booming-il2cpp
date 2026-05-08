"""Hotpatch verification runner — collects per-family exe JSON results
and produces hotupdate-verification-report.json files for the dashboard.

Each family's standalone Hotpatch exe runs Register → ApplyPatch → InterpreterEntryDirect
and outputs a JSON report. This runner collects all per-family outputs and writes
per-family hotupdate-verification-report.json files that the dashboard generator
can consume.

Usage:
    python hotupdate_verification_runner.py
        --results-dir verification/foundation-dll/hotupdate-results
        --verification-root verification/foundation-dll
"""

from __future__ import annotations

import argparse
import json
import sys
from pathlib import Path
from typing import Any


def _slug_to_family_id(slug: str) -> str:
    """Reconstruct familyId from directory slug (e.g., 'convert-char' -> 'family/System.Private.CoreLib/convert/char')."""
    parts = slug.replace("_", "-").split("-")
    return f"family/System.Private.CoreLib/{'/'.join(parts)}"


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


def generate_per_family_reports(
    results_dir: Path,
    verification_root: Path,
    assembly_name: str,
) -> dict[str, Any]:
    """Load per-family D3 exe outputs and write hotupdate-verification-report.json
    for each family.

    Each per-family exe JSON contains method-level results with methodToken and
    status. We write them as methodResults (with synthetic methodSubjectIds if
    available, or methodToken-based identifiers as fallback).

    Args:
        results_dir: Directory containing per-family .json files from D3 exes.
        verification_root: Root of verification directory (e.g., repo_root / "verification").
        assembly_name: Assembly name (e.g., "System.Private.CoreLib").

    Returns:
        Dict mapping familyId -> report dict.
    """
    if not results_dir.is_dir():
        print(f"ERROR: results dir not found: {results_dir}", file=sys.stderr)
        sys.exit(1)

    reports: dict[str, Any] = {}
    json_files = sorted(results_dir.glob("*.json"))

    if not json_files:
        print(f"WARNING: no JSON files found in {results_dir}")
        return reports

    print(f"Found {len(json_files)} per-family result files in {results_dir}")

    for json_path in json_files:
        slug = json_path.stem  # e.g., "convert-char"

        try:
            data = _load_json(json_path)
        except (json.JSONDecodeError, OSError) as e:
            print(f"  SKIP {slug}: failed to read JSON ({e})")
            continue

        total = int(data.get("totalMethods") or 0)
        passed = int(data.get("passedMethods") or 0)
        failed = int(data.get("failedMethods") or 0)
        family_id = str(data.get("familyId") or _slug_to_family_id(slug))
        raw_results = list(data.get("results") or [])

        # Build methodResults: preserve all fields from the D3 exe output
        method_results: list[dict[str, Any]] = []
        for result in raw_results:
            entry: dict[str, Any] = {
                "methodToken": result.get("methodToken"),
                "status": str(result.get("status", "unmatched")),
                "hotpatchPatched": result.get("d3Patched", False),
                "patchReturnValue": result.get("patchReturnValue"),
                "interpreterDispatched": result.get("interpreterDispatched", False),
                "revertVerified": result.get("revertVerified", False),
                "semanticVerified": result.get("semanticVerified", False),
            }
            method_results.append(entry)

        report = {
            "schemaVersion": 2,
            "assemblyName": assembly_name,
            "familyId": family_id,
            "verificationKind": "hotupdate-proof",
            "hotpatchPatchApplied": data.get("d3PatchApplied", False),
            "hotpatchPatchedCount": data.get("d3PatchedCount", 0),
            "summary": {
                "totalMethods": total,
                "passedMethods": passed,
                "failedMethods": failed,
                "unmatchedMethods": total - passed - failed,
            },
            "methodResults": method_results,
        }

        # Write report to family directory
        report_path = verification_root / "foundation-dll" / assembly_name / slug / "hotupdate-verification-report.json"
        _write_json(report_path, report)

        print(f"  {slug}: {total} methods, {passed} passed, {failed} failed -> {report_path}")
        reports[family_id] = report

    total_families = len(reports)
    total_passed = sum(r["summary"]["passedMethods"] for r in reports.values())
    total_failed = sum(r["summary"]["failedMethods"] for r in reports.values())
    print(f"\nProcessed {total_families} families: {total_passed} passed, {total_failed} failed total")

    return reports


def main() -> None:
    parser = argparse.ArgumentParser(description="Generate per-family D3 hotupdate verification reports")
    parser.add_argument("--results-dir", type=Path,
                        default=Path.cwd() / "verification" / "foundation-dll" / "hotupdate-results",
                        help="Directory containing per-family .json files from D3 exes")
    parser.add_argument("--verification-root", type=Path, default=Path.cwd() / "verification",
                        help="Root of the verification directory")
    parser.add_argument("--assembly-name", default="System.Private.CoreLib",
                        help="Assembly name")
    args = parser.parse_args()

    generate_per_family_reports(
        results_dir=args.results_dir.resolve(),
        verification_root=args.verification_root.resolve(),
        assembly_name=args.assembly_name,
    )


if __name__ == "__main__":
    main()
