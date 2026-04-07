from __future__ import annotations

from pathlib import Path
from typing import Any
import re
import sys

try:
    from ..common import read_json
    from . import path_resolver as path_resolver_module
except ImportError:
    root = Path(__file__).resolve().parents[1]
    sys.path.insert(0, str(root))
    from common import read_json
    from testing import path_resolver as path_resolver_module


def read_json_file(path: Path) -> object:
    return read_json(path)


def test_json_schema_type(value: object, schema_type: str) -> bool:
    if schema_type == "object":
        return isinstance(value, dict)
    if schema_type == "array":
        return isinstance(value, list)
    if schema_type == "string":
        return isinstance(value, str)
    if schema_type == "integer":
        return isinstance(value, int) and not isinstance(value, bool)
    raise RuntimeError(f"unsupported schema type: {schema_type}")


def assert_json_matches_schema(value: object, schema: dict[str, Any], path: str) -> None:
    if "const" in schema and value != schema["const"]:
        raise RuntimeError(f"schema const mismatch at {path}: expected '{schema['const']}' actual '{value}'")

    schema_type = schema.get("type")
    if schema_type is not None and not test_json_schema_type(value, str(schema_type)):
        raise RuntimeError(f"schema type mismatch at {path}: expected '{schema_type}'")

    if "enum" in schema and value not in schema["enum"]:
        raise RuntimeError(f"schema enum mismatch at {path}: value '{value}' not allowed")

    if "minLength" in schema and (value is None or len(str(value)) < int(schema["minLength"])):
        raise RuntimeError(f"schema minLength mismatch at {path}: expected >= {schema['minLength']}")

    if "minimum" in schema and int(value) < int(schema["minimum"]):
        raise RuntimeError(f"schema minimum mismatch at {path}: expected >= {schema['minimum']}")

    if "pattern" in schema and str(schema["pattern"]) not in {"", None} and re.search(str(schema["pattern"]), str(value)) is None:
        raise RuntimeError(f"schema pattern mismatch at {path}: value '{value}' does not match '{schema['pattern']}'")

    if "required" in schema:
        if not isinstance(value, dict):
            raise RuntimeError(f"schema required property mismatch at {path}: expected object")
        for required_name in schema["required"]:
            if required_name not in value:
                raise RuntimeError(f"schema required property missing at {path}: '{required_name}'")

    if "properties" in schema:
        if not isinstance(value, dict):
            raise RuntimeError(f"schema properties mismatch at {path}: expected object")
        for property_name, property_schema in schema["properties"].items():
            if property_name in value:
                assert_json_matches_schema(value[property_name], property_schema, f"{path}.{property_name}")

    if "minItems" in schema and len(list(value if isinstance(value, list) else [])) < int(schema["minItems"]):
        raise RuntimeError(f"schema minItems mismatch at {path}: expected >= {schema['minItems']}")

    if "items" in schema:
        if not isinstance(value, list):
            raise RuntimeError(f"schema items mismatch at {path}: expected array")
        for index, item in enumerate(value):
            assert_json_matches_schema(item, schema["items"], f"{path}[{index}]")


def assert_json_files_parse(paths: list[Path]) -> None:
    for path in paths:
        read_json_file(path)


def analysis_contract_json_paths(repo_root: Path) -> list[Path]:
    contract_roots = path_resolver_module.contract_roots(repo_root)
    paths: list[Path] = []
    paths.extend(sorted(contract_roots["artifactSchemaRoot"].glob("*.json")))
    paths.extend(sorted(contract_roots["artifactSampleRoot"].glob("*.json")))
    paths.extend(sorted(contract_roots["artifactSnapshotRoot"].glob("*.json")))
    return paths


def trace_contract_json_paths(repo_root: Path) -> list[Path]:
    return sorted((repo_root / "tests" / "contracts" / "trace").rglob("*.json"))


def validate_analysis_contracts(repo_root: Path) -> None:
    contract_roots = path_resolver_module.contract_roots(repo_root)
    schema_dir = contract_roots["artifactSchemaRoot"]
    example_dir = contract_roots["artifactSampleRoot"]
    snapshot_dir = contract_roots["artifactSnapshotRoot"]

    for schema_file in sorted(schema_dir.glob("*.schema.json")):
        base_name = schema_file.name.removesuffix(".schema.json")
        schema = read_json_file(schema_file)
        example = read_json_file(example_dir / f"{base_name}.min.json")
        snapshot = read_json_file(snapshot_dir / f"{base_name}.snapshot.json")
        if not isinstance(schema, dict):
            raise RuntimeError(f"analysis schema must be an object: {schema_file}")
        assert_json_matches_schema(example, schema, f"{base_name}.example")
        assert_json_matches_schema(snapshot, schema, f"{base_name}.snapshot")


def validate_trace_schema_contracts(repo_root: Path) -> None:
    schema = read_json_file(repo_root / "tests" / "contracts" / "trace" / "schema" / "warmup-trace.schema.json")
    if not isinstance(schema, dict):
        raise RuntimeError("trace schema must be an object: tests/contracts/trace/schema/warmup-trace.schema.json")
    for snapshot_path in sorted((repo_root / "tests" / "contracts" / "trace" / "snapshots").glob("*.json")):
        snapshot = read_json_file(snapshot_path)
        assert_json_matches_schema(snapshot, schema, snapshot_path.name)
