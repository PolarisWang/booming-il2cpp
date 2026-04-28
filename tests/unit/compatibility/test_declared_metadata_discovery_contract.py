from __future__ import annotations

import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
COLLECTION_GEN_PROGRAM_PATH = REPO_ROOT / "src" / "tools" / "Chaos.IL2CPP.Tools.TestFramework.CollectionGen" / "Program.cs"
LEGACY_DISCOVERY_PROGRAM_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.DeclarationDiscovery" / "Program.cs"
FAMILY_VERIFICATION_KERNEL_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "foundation_dll" / "verification_kernel.py"
FOUNDATION_DLL_AUDIT_GENERATOR_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "foundation_dll_audit_generator.py"


class DeclaredMetadataDiscoveryContractTests(unittest.TestCase):
    def test_collection_generator_lives_in_test_framework_tools_boundary(self) -> None:
        self.assertTrue(COLLECTION_GEN_PROGRAM_PATH.is_file(), msg=f"missing collection generator source: {COLLECTION_GEN_PROGRAM_PATH}")
        self.assertFalse(
            LEGACY_DISCOVERY_PROGRAM_PATH.exists(),
            msg=f"legacy IL2CPP discovery tool should be removed: {LEGACY_DISCOVERY_PROGRAM_PATH}",
        )

    def test_declaration_discovery_centralizes_attribute_schema_mapping(self) -> None:
        discovery_source = COLLECTION_GEN_PROGRAM_PATH.read_text(encoding="utf-8")

        self.assertIn("DeclarationAttributeSchema", discovery_source)
        self.assertIn("TryResolveDeclaredAttributeSchema", discovery_source)
        self.assertIn("UnitNamedArgumentReaders", discovery_source)
        self.assertIn("BenchmarkNamedArgumentReaders", discovery_source)
        self.assertIn("CapabilityTestAttribute", discovery_source)
        self.assertIn("BenchmarkAttribute", discovery_source)

        for legacy_fragment in [
            'case "Alias":',
            'case "CapabilityFamily":',
            'case "Capability":',
            'case "Requires":',
            'case "Archetype":',
            'case "HotUpdateCapability":',
            'case "Evidence":',
            'case "Priority":',
            'case "Modes":',
            'case "WarmupCount":',
            'case "IterationCount":',
            'case "InvocationCount":',
        ]:
            self.assertNotIn(legacy_fragment, discovery_source)

    def test_family_verification_kernel_no_longer_embeds_family_specific_claim_mappings(self) -> None:
        kernel_source = FAMILY_VERIFICATION_KERNEL_PATH.read_text(encoding="utf-8")

        self.assertNotIn("native_proof_family_subject_ids", kernel_source)

    def test_audit_generator_no_longer_embeds_native_proof_family_claim_logic(self) -> None:
        generator_source = FOUNDATION_DLL_AUDIT_GENERATOR_PATH.read_text(encoding="utf-8")

        self.assertNotIn("def _derive_family_native_proof", generator_source)
        self.assertNotIn("def _native_proof_family_subject_ids", generator_source)
        self.assertIn("family_verification_claims_module", generator_source)
        self.assertIn("verification_kernel_module", generator_source)


if __name__ == "__main__":
    unittest.main()
