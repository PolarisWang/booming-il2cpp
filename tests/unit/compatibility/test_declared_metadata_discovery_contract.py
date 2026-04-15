from __future__ import annotations

import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
DISCOVERY_PROGRAM_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.DeclarationDiscovery" / "Program.cs"


class DeclaredMetadataDiscoveryContractTests(unittest.TestCase):
    def test_declaration_discovery_centralizes_attribute_schema_mapping(self) -> None:
        self.assertTrue(DISCOVERY_PROGRAM_PATH.is_file(), msg=f"missing discovery source: {DISCOVERY_PROGRAM_PATH}")
        discovery_source = DISCOVERY_PROGRAM_PATH.read_text(encoding="utf-8")

        self.assertIn("DeclarationAttributeSchema", discovery_source)
        self.assertIn("TryResolveDeclaredAttributeSchema", discovery_source)
        self.assertIn("UnitNamedArgumentReaders", discovery_source)
        self.assertIn("BenchmarkNamedArgumentReaders", discovery_source)

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


if __name__ == "__main__":
    unittest.main()
