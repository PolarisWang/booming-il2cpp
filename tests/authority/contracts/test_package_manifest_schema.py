from __future__ import annotations

import json
import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
SCHEMA_PATH = REPO_ROOT / "contracts" / "shared" / "v0" / "package-manifest.schema.json"


def _assert_type(value: object, expected_type: str, path: str) -> None:
    type_checks = {
        "object": lambda candidate: isinstance(candidate, dict),
        "array": lambda candidate: isinstance(candidate, list),
        "string": lambda candidate: isinstance(candidate, str),
        "integer": lambda candidate: isinstance(candidate, int) and not isinstance(candidate, bool),
    }

    check = type_checks.get(expected_type)
    if check is None:
        raise AssertionError(f"unsupported schema type in test validator: {expected_type}")
    if not check(value):
        raise ValueError(f"{path}: expected {expected_type}, got {type(value).__name__}")


def validate_against_schema(instance: object, schema: dict[str, object], path: str = "$") -> None:
    expected_type = schema.get("type")
    if isinstance(expected_type, str):
        _assert_type(instance, expected_type, path)

    if "const" in schema and instance != schema["const"]:
        raise ValueError(f"{path}: expected const {schema['const']!r}, got {instance!r}")

    if isinstance(instance, dict):
        required = list(schema.get("required") or [])
        for required_key in required:
            if required_key not in instance:
                raise ValueError(f"{path}: missing required property {required_key}")

        properties = dict(schema.get("properties") or {})
        if schema.get("additionalProperties") is False:
            extras = set(instance.keys()) - set(properties.keys())
            if extras:
                raise ValueError(f"{path}: unexpected properties {sorted(extras)}")

        for key, value in instance.items():
            if key in properties:
                validate_against_schema(value, dict(properties[key]), f"{path}.{key}")

    if isinstance(instance, list):
        min_items = schema.get("minItems")
        if isinstance(min_items, int) and len(instance) < min_items:
            raise ValueError(f"{path}: expected at least {min_items} items, got {len(instance)}")

        item_schema = schema.get("items")
        if isinstance(item_schema, dict):
            for index, item in enumerate(instance):
                validate_against_schema(item, item_schema, f"{path}[{index}]")


class PackageManifestSchemaTests(unittest.TestCase):
    def test_package_manifest_schema_exists_and_exposes_frozen_shape(self) -> None:
        self.assertTrue(SCHEMA_PATH.is_file(), msg=f"missing schema: {SCHEMA_PATH}")
        schema = json.loads(SCHEMA_PATH.read_text(encoding="utf-8"))

        self.assertEqual("object", schema["type"])
        self.assertEqual(False, schema["additionalProperties"])
        self.assertEqual(
            [
                "formatVersion",
                "packageId",
                "targetAotVersion",
                "assemblies",
                "supplementalMetadata",
                "signature",
            ],
            schema["required"],
        )
        self.assertEqual("v0", dict(schema["properties"])["formatVersion"]["const"])
        self.assertEqual(1, dict(schema["properties"])["assemblies"]["minItems"])

    def test_valid_manifest_passes_schema_validation(self) -> None:
        schema = json.loads(SCHEMA_PATH.read_text(encoding="utf-8"))
        manifest = {
            "formatVersion": "v0",
            "packageId": "com.example.hotupdate.20260411",
            "targetAotVersion": "1.0.0",
            "assemblies": [
                {
                    "name": "HotPatch.dll",
                    "hash": "sha256:0123456789abcdef",
                    "size": 12345,
                    "entryPoint": "HotPatch/Patch::Apply()",
                }
            ],
            "supplementalMetadata": "metadata-supplement.bin",
            "signature": "signed-proof",
        }

        validate_against_schema(manifest, schema)

    def test_invalid_manifest_is_rejected(self) -> None:
        schema = json.loads(SCHEMA_PATH.read_text(encoding="utf-8"))
        invalid_manifest = {
            "formatVersion": "v1",
            "packageId": "com.example.bad",
            "targetAotVersion": "1.0.0",
            "assemblies": [],
            "supplementalMetadata": "metadata-supplement.bin",
            "signature": "signed-proof",
            "unexpected": True,
        }

        with self.assertRaises(ValueError):
            validate_against_schema(invalid_manifest, schema)


if __name__ == "__main__":
    unittest.main()
