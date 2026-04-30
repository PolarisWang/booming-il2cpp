#!/usr/bin/env python3
"""Validate all output JSON artifacts against their JSON Schema definitions.

Usage:
    python docs/dev/scripts/validate-output-schemas.py [--repo-root PATH]

Exits with code 0 if all validations pass, 1 if any fail.
"""

from __future__ import annotations

import argparse
import json
import sys
from pathlib import Path
from typing import Any


def _find_project_root() -> Path:
    marker = Path(__file__).resolve().parents[3]
    return marker


SCHEMA_DIR = "verification/projections/foundation-dll-audit/schema"
OUTPUT_DIR = "verification/projections/foundation-dll-audit"

ARTIFACT_SCHEMA_MAP: list[tuple[str, str]] = [
    ("program.json", "program.schema.json"),
    ("dll-matrix.json", "dll-matrix.schema.json"),
    ("family-verification.json", "family-verification.schema.json"),
    ("family-verification-claims.json", "family-verification-claims.schema.json"),
    ("truth-contracts.json", "truth-contracts.schema.json"),
]


def _array_paths(obj: dict[str, Any], prefix: str = "") -> list[str]:
    """Walk a dict and collect JSON paths for all array-type fields (leaf arrays)."""
    paths: list[str] = []
    for key, value in obj.items():
        current = f"{prefix}.{key}" if prefix else key
        if isinstance(value, dict):
            paths.extend(_array_paths(value, current))
        elif isinstance(value, list) and value and isinstance(value[0], (dict, list)):
            paths.extend(_array_paths(value[0], f"{current}[]"))
    return paths


def validate_with_jsonschema(instance: dict, schema: dict, artifact_name: str) -> list[str]:
    """Try to validate with the `jsonschema` library; fall back to basic checks."""
    try:
        import jsonschema as js

        validator = js.validate(instance, schema)
        return []
    except ImportError:
        # Fallback: basic structural checks
        return _basic_schema_check(instance, schema, artifact_name)
    except js.ValidationError as e:
        return [f"  {artifact_name}: {e.message}"]


def _basic_schema_check(instance: dict, schema: dict, artifact_name: str) -> list[str]:
    """Basic structural validation without jsonschema library."""
    errors: list[str] = []
    required = schema.get("required", [])
    for field in required:
        if field not in instance:
            errors.append(f"  {artifact_name}: missing required field '{field}'")
    properties = schema.get("properties", {})
    for field, definition in properties.items():
        if field not in instance:
            continue
        expected_type = definition.get("type")
        if expected_type:
            type_map = {
                "string": str,
                "integer": int,
                "number": (int, float),
                "boolean": bool,
                "object": dict,
                "array": list,
            }
            py_type = type_map.get(expected_type)
            if py_type and not isinstance(instance[field], py_type):
                errors.append(
                    f"  {artifact_name}.{field}: expected {expected_type}, got {type(instance[field]).__name__}"
                )
        enum_values = definition.get("enum")
        if enum_values and instance[field] not in enum_values:
            errors.append(
                f"  {artifact_name}.{field}: value '{instance[field]}' not in enum {enum_values}"
            )
    return errors


def validate_outputs(repo_root: Path) -> bool:
    schema_root = repo_root / SCHEMA_DIR
    output_root = repo_root / OUTPUT_DIR

    all_ok = True
    for artifact_file, schema_file in ARTIFACT_SCHEMA_MAP:
        artifact_path = output_root / artifact_file
        schema_path = schema_root / schema_file

        if not schema_path.is_file():
            print(f"  SKIP {artifact_file}: schema not found at {schema_path}")
            continue
        if not artifact_path.is_file():
            print(f"  SKIP {artifact_file}: artifact not found at {artifact_path}")
            continue

        with open(artifact_path, encoding="utf-8") as f:
            instance = json.load(f)
        with open(schema_path, encoding="utf-8") as f:
            schema = json.load(f)

        errors = validate_with_jsonschema(instance, schema, artifact_file)
        if errors:
            print(f"  FAIL {artifact_file}:")
            for err in errors:
                print(err)
            all_ok = False
        else:
            print(f"  OK   {artifact_file}")

    # Also validate per-DLL JSON files against a shared schema
    dll_dir = output_root / "dlls"
    dll_schema_path = schema_root / "dll-detail.schema.json"
    if dll_schema_path.is_file() and dll_dir.is_dir():
        with open(dll_schema_path, encoding="utf-8") as f:
            dll_schema = json.load(f)
        for dll_file in sorted(dll_dir.glob("*.json")):
            with open(dll_file, encoding="utf-8") as f:
                instance = json.load(f)
            errors = validate_with_jsonschema(instance, dll_schema, f"dlls/{dll_file.name}")
            if errors:
                for err in errors:
                    print(err)
                all_ok = False
            else:
                print(f"  OK   dlls/{dll_file.name}")

    return all_ok


def main() -> None:
    parser = argparse.ArgumentParser(description="Validate dashboard output JSON against schemas")
    parser.add_argument("--repo-root", type=Path, default=_find_project_root())
    args = parser.parse_args()

    repo_root = args.repo_root.resolve()
    print(f"Validating outputs in {repo_root / OUTPUT_DIR}")
    ok = validate_outputs(repo_root)
    if ok:
        print("\nAll validations passed.")
        sys.exit(0)
    else:
        print("\nSome validations FAILED.")
        sys.exit(1)


if __name__ == "__main__":
    main()
