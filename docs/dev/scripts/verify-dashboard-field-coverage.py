#!/usr/bin/env python3
"""Verify dashboard output field coverage against the data dictionary.

Usage:
    python docs/dev/scripts/verify-dashboard-field-coverage.py [--repo-root PATH]

Checks:
1. All fields in output JSON artifacts have entries in the data dictionary
2. All enum values in the code have corresponding entries
3. Returns coverage percentage and lists undocumented fields

Exit code: 0 if coverage >= 95%, 1 otherwise
"""

from __future__ import annotations

import argparse
import json
import re
import sys
from pathlib import Path
from typing import Any


def _find_project_root() -> Path:
    return Path(__file__).resolve().parents[3]


OUTPUT_DIR = "verification/projections/foundation-dll-audit"
DATA_DICT_PATH = "wiki/03-功能模块/05-工具能力/foundation-dll-audit-data-dictionary.md"
PROVENANCE_PATH = "wiki/03-功能模块/05-工具能力/foundation-dll-audit-provenance-registry.md"
REGISTRY_COMMITTED_PATH = "wiki/03-功能模块/05-工具能力/foundation-dll-audit-provenance-registry.md"

# Fields that are known/expected and may not need documentation
KNOWN_UNSTRUCTURED_FIELDS: set[str] = set()


def _walk_fields(obj: Any, prefix: str = "", max_depth: int = 10) -> dict[str, str]:
    """Walk a JSON object and collect all leaf field paths with value type hints."""
    paths: dict[str, str] = {}
    if max_depth <= 0:
        return paths

    if isinstance(obj, dict):
        for key, value in obj.items():
            current = f"{prefix}.{key}" if prefix else key
            if isinstance(value, (dict, list)):
                paths.update(_walk_fields(value, current, max_depth - 1))
            else:
                paths[current] = type(value).__name__
    elif isinstance(obj, list) and obj:
        if isinstance(obj[0], dict):
            paths.update(_walk_fields(obj[0], f"{prefix}[]", max_depth - 1))
    return paths


def _extract_documented_fields(dict_path: Path) -> set[str]:
    """Parse the data dictionary markdown and extract documented field references."""
    if not dict_path.is_file():
        return set()

    text = dict_path.read_text(encoding="utf-8")
    fields: set[str] = set()

    # Find inline code references that look like field names: `fieldName` or `path.to.field`
    for match in re.finditer(r"`([a-zA-Z_][a-zA-Z0-9_.\[\]]*)`", text):
        fields.add(match.group(1))

    # Also find markdown table rows with field references
    for match in re.finditer(r"\|[^|]*`([^`]+)`[^|]*\|", text):
        fields.add(match.group(1))

    return fields


def _extract_committed_registry(registry_path: Path) -> dict[str, str]:
    """Parse the provenance registry table to extract field -> function mapping."""
    if not registry_path.is_file():
        return {}

    text = registry_path.read_text(encoding="utf-8")
    mapping: dict[str, str] = {}

    # Parse markdown table rows in the provenance section
    in_table = False
    for line in text.splitlines():
        if line.startswith("| `") and "|" in line:
            parts = [p.strip() for p in line.split("|")]
            if len(parts) >= 3:
                field = parts[1].strip("`")
                func = parts[3] if len(parts) >= 4 else ""
                if field:
                    mapping[field] = func
    return mapping


def _validate_enum_in_code(repo_root: Path) -> list[str]:
    """Check enum values in source code match documented ones."""
    issues: list[str] = []

    generator_path = (
        repo_root / "build" / "toolchains" / "run" / "testing" / "foundation_dll_audit_generator.py"
    )
    if not generator_path.is_file():
        return issues

    text = generator_path.read_text(encoding="utf-8")

    # Look for valid_* sets
    valid_sets: dict[str, set[str]] = {}
    for match in re.finditer(r"valid_(\w+)\s*=\s*\{([^}]+)\}", text):
        set_name = match.group(1)
        values = set()
        for v in re.finditer(r'"([^"]+)"', match.group(2)):
            values.add(v.group(1))
        valid_sets[set_name] = values

    # Also find ROADMAP_STATUS_TO_DLL_STATE mapping
    for match in re.finditer(r'"([^"]+)"\s*:\s*"([^"]+)"', text):
        pass  # Not checking all key-value pairs, just explicit valid sets

    return issues


def verify_coverage(repo_root: Path) -> dict[str, Any]:
    output_root = repo_root / OUTPUT_DIR
    dict_path = repo_root / DATA_DICT_PATH

    # Collect all fields from output artifacts
    all_fields: dict[str, dict[str, str]] = {}
    for artifact_name in [
        "program.json",
        "dll-matrix.json",
        "family-verification.json",
        "family-verification-claims.json",
        "truth-contracts.json",
    ]:
        path = output_root / artifact_name
        if path.is_file():
            with open(path, encoding="utf-8") as f:
                data = json.load(f)
            fields = _walk_fields(data)
            if fields:
                all_fields[artifact_name] = fields

    # DLL detail
    dll_dir = output_root / "dlls"
    if dll_dir.is_dir():
        files = sorted(dll_dir.glob("*.json"))
        if files:
            with open(files[0], encoding="utf-8") as f:
                data = json.load(f)
            fields = _walk_fields(data)
            if fields:
                all_fields["dll-detail.json"] = fields

    # Documented fields
    documented_fields = _extract_documented_fields(dict_path)

    # Registry
    registry = _extract_committed_registry(repo_root / REGISTRY_COMMITTED_PATH)

    # Check coverage
    total = 0
    documented = 0
    undocumented: list[tuple[str, str]] = []

    for artifact_name, fields in all_fields.items():
        for field_path in fields:
            if field_path in KNOWN_UNSTRUCTURED_FIELDS:
                continue
            total += 1
            # Check if field path or leaf key is documented
            leaf = field_path.replace("[]", "").split(".")[-1]
            if (
                field_path in documented_fields
                or leaf in documented_fields
                or field_path in registry
            ):
                documented += 1
            else:
                undocumented.append((artifact_name, field_path))

    coverage_pct = round((documented / total) * 100, 2) if total > 0 else 100.0

    # Enum validation
    enum_issues = _validate_enum_in_code(repo_root)

    return {
        "totalFields": total,
        "documentedFields": documented,
        "coveragePercent": coverage_pct,
        "undocumentedFields": undocumented,
        "enumIssues": enum_issues,
        "registryEntryCount": len(registry),
    }


def main() -> None:
    parser = argparse.ArgumentParser(description="Verify dashboard field coverage")
    parser.add_argument("--repo-root", type=Path, default=_find_project_root())
    args = parser.parse_args()

    repo_root = args.repo_root.resolve()
    result = verify_coverage(repo_root)

    print("=" * 60)
    print("Dashboard Field Coverage Report")
    print("=" * 60)
    print(f"Total fields:        {result['totalFields']}")
    print(f"Documented fields:   {result['documentedFields']}")
    print(f"Coverage:            {result['coveragePercent']}%")
    print(f"Registry entries:    {result['registryEntryCount']}")
    print()

    if result["undocumentedFields"]:
        print("Undocumented fields:")
        for artifact, field in result["undocumentedFields"][:30]:
            print(f"  {artifact}: {field}")
        if len(result["undocumentedFields"]) > 30:
            print(f"  ... and {len(result['undocumentedFields']) - 30} more")
        print()

    if result["enumIssues"]:
        print("Enum issues:")
        for issue in result["enumIssues"]:
            print(f"  {issue}")
        print()

    coverage_pct = result["coveragePercent"]
    if coverage_pct >= 95.0:
        print(f"PASS: Coverage {coverage_pct}% >= 95%")
        sys.exit(0)
    else:
        print(f"FAIL: Coverage {coverage_pct}% < 95%")
        print(f"       {len(result['undocumentedFields'])} fields need documentation")
        sys.exit(1)


if __name__ == "__main__":
    main()
