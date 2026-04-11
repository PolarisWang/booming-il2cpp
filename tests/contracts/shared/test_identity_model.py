from __future__ import annotations

import json
import re
import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
SHARED_ROOT = REPO_ROOT / "contracts" / "shared" / "v0"
ENTRY_PATTERN = re.compile(
    r"^[A-Za-z0-9_.-]+/[A-Za-z0-9_.+`<>-]+::[A-Za-z0-9_.`<>-]+(?:\([A-Za-z0-9_., <>+\[\]`-]*\))?$"
)


class SharedIdentityModelTests(unittest.TestCase):
    def test_shared_contract_documents_exist(self) -> None:
        expected_files = [
            "README.md",
            "identity-model.md",
            "object-model.md",
            "handle-model.md",
            "abi-calling-convention.md",
            "exception-boundary.md",
            "delegate-abi.md",
            "metadata-token-mapping.md",
            "version-policy.md",
            "interpreter-ir-decision.md",
        ]

        for relative_path in expected_files:
            self.assertTrue((SHARED_ROOT / relative_path).is_file(), msg=f"missing shared contract file: {relative_path}")

    def test_subject_entries_follow_frozen_entry_pattern(self) -> None:
        manifest_paths = sorted((REPO_ROOT / "subjects").rglob("subject.manifest.json"))
        self.assertGreaterEqual(len(manifest_paths), 11)

        for manifest_path in manifest_paths:
            manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
            entry = str(dict(manifest["source"])["entry"])
            self.assertRegex(
                entry,
                ENTRY_PATTERN,
                msg=f"source.entry must follow frozen SubjectId-like format: {manifest_path.relative_to(REPO_ROOT)}",
            )

    def test_managed_naming_source_matches_frozen_patterns(self) -> None:
        contracts_source = (
            REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Contracts" / "ManagedClosureContracts.cs"
        ).read_text(encoding="utf-8")

        self.assertIn(
            'return $"{assemblyName}/{GetTypeIdentityPart(assemblyName, namespaceName, typeName)}";',
            contracts_source,
        )
        self.assertIn('return $"{declaringTypeSubjectId}::{fieldName}";', contracts_source)
        self.assertIn('return $"{declaringTypeSubjectId}::property:{propertyName}";', contracts_source)
        self.assertIn(
            'return $"{declaringTypeSubjectId}::{methodName}({string.Join(",", parameterTypes)})";',
            contracts_source,
        )
        self.assertIn(
            'return $"{methodSubjectId}::parameter[{parameterIndex}]:{parameterName}";',
            contracts_source,
        )
        self.assertIn(
            'return $"{StripGenericArity(genericTypeSubjectId)}<{string.Join(",", typeArguments)}>";',
            contracts_source,
        )
        self.assertIn("if (current != '`')", contracts_source)

    def test_runtime_sources_expose_frozen_object_and_handle_fact_sources(self) -> None:
        runtime_abi = (REPO_ROOT / "contracts" / "native" / "v0" / "runtime_abi.h").read_text(encoding="utf-8")
        runtime_core = (
            REPO_ROOT / "src" / "native" / "runtime-core" / "runtime_core.cpp"
        ).read_text(encoding="utf-8")
        bootstrap = (REPO_ROOT / "src" / "native" / "bootstrap" / "bootstrap.cpp").read_text(encoding="utf-8")

        handle_aliases = {
            "TypeInfoHandle": "TypeInfoOpaque",
            "MethodInfoHandle": "MethodInfoOpaque",
            "FieldInfoHandle": "FieldInfoOpaque",
            "PropertyInfoHandle": "PropertyInfoOpaque",
            "EventInfoHandle": "EventInfoOpaque",
            "ParameterInfoHandle": "ParameterInfoOpaque",
            "GenericContextHandle": "GenericContextOpaque",
            "AssemblyHandle": "AssemblyOpaque",
            "ImageHandle": "ImageOpaque",
            "ExceptionHandle": "ExceptionOpaque",
        }

        for handle_name, opaque_name in handle_aliases.items():
            self.assertIn(f"typedef struct {opaque_name}* {handle_name};", runtime_abi)

        self.assertIn("struct ObjectHeader {", runtime_core)
        self.assertIn("TypeInfoHandle type;", runtime_core)
        self.assertIn("struct StringObjectHeader {", runtime_core)
        self.assertIn("uintptr_t byte_count;", runtime_core)
        self.assertIn("struct ArrayHeader {", runtime_core)
        self.assertIn("uintptr_t length;", runtime_core)
        self.assertIn("struct BoxedValueHeader {", runtime_core)
        self.assertIn("struct DelegateInstance {", bootstrap)
        self.assertIn("uint32_t method_token;", bootstrap)
        self.assertIn("void* method_pointer;", bootstrap)
        self.assertIn("void* target_instance;", bootstrap)


if __name__ == "__main__":
    unittest.main()
