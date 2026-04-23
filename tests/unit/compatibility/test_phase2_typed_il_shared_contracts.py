from __future__ import annotations

import unittest
from pathlib import Path

from tests.support import read_contracts_source


REPO_ROOT = Path(__file__).resolve().parents[3]
SEMANTIC_WORLD_STAGE_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.SemanticWorld" / "SemanticWorldStage.cs"
CODEGEN_STAGE_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.CodeGen" / "CodeGenStage.cs"


class Phase2TypedIlSharedContractTests(unittest.TestCase):
    def test_contracts_define_shared_method_identity_body_availability_and_dispatch_enums(self) -> None:
        contracts_source = read_contracts_source(REPO_ROOT)

        for required_fragment in [
            "record ManagedMethodIdentityArtifact",
            "enum BodyAvailabilityCode",
            "InterpreterReady",
            "ExternalRuntime",
            "Unsupported",
            "enum HybridDispatchKind",
            "Direct",
            "Virtual",
            "Bridge",
            "class BodyAvailabilityResolver",
            "class HybridDispatchResolver",
        ]:
            self.assertIn(required_fragment, contracts_source)

    def test_semantic_world_and_codegen_use_shared_resolvers_and_emit_typed_fields(self) -> None:
        semantic_world_source = SEMANTIC_WORLD_STAGE_PATH.read_text(encoding="utf-8")
        codegen_source = CODEGEN_STAGE_PATH.read_text(encoding="utf-8")

        for required_fragment in [
            "BodyAvailabilityResolver.Resolve(",
            "BodyAvailabilityCode =",
        ]:
            self.assertIn(required_fragment, semantic_world_source)

        for required_fragment in [
            "Identity = ManagedMethodIdentityResolver.Create(method)",
            "BodyAvailabilityCode =",
            "DispatchKindCode =",
            "HybridDispatchResolver.ResolveInstruction(",
        ]:
            self.assertIn(required_fragment, codegen_source)


if __name__ == "__main__":
    unittest.main()
