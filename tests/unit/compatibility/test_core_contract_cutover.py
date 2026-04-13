from __future__ import annotations

import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
CONTRACTS_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Contracts" / "ManagedClosureContracts.cs"
SEMANTIC_WORLD_STAGE_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.SemanticWorld" / "SemanticWorldStage.cs"
CODEGEN_STAGE_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.CodeGen" / "CodeGenStage.cs"
EMITTER_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.CodeGen" / "NativeReferenceProofEmitter.cs"

THREE_PART_CONCAT = "System.Private.CoreLib/System.String::Concat(System.String,System.String,System.String)"


class Phase2CoreContractCutoverTests(unittest.TestCase):
    def test_semantic_world_stage_owns_current_concat_canonicalization(self) -> None:
        semantic_world_source = SEMANTIC_WORLD_STAGE_PATH.read_text(encoding="utf-8")
        codegen_source = CODEGEN_STAGE_PATH.read_text(encoding="utf-8")

        self.assertIn(THREE_PART_CONCAT, semantic_world_source)
        self.assertNotIn("NormalizeInstructions(", codegen_source)
        self.assertNotIn(THREE_PART_CONCAT, codegen_source)

    def test_contracts_and_downstream_sources_expose_method_contract_surface(self) -> None:
        contracts_source = CONTRACTS_PATH.read_text(encoding="utf-8")
        codegen_source = CODEGEN_STAGE_PATH.read_text(encoding="utf-8")
        emitter_source = EMITTER_PATH.read_text(encoding="utf-8")

        for marker in [
            "CanonicalSubjects",
            "SemanticShapes",
            "CapabilityBundles",
            "MethodRole",
            "BodyAvailability",
            "Capabilities",
        ]:
            self.assertIn(marker, contracts_source)

        for marker in [
            "MethodRole",
            "BodyAvailability",
            "Capabilities",
        ]:
            self.assertIn(marker, codegen_source)
            self.assertIn(marker, emitter_source)


if __name__ == "__main__":
    unittest.main()
