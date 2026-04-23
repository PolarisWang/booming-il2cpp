from __future__ import annotations

import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
AOT_CORE_IR_LOWERING_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.CodeGen" / "AotCoreIrLowering.cs"


class LoaderInstantiationDemandGraphAotCoreIrAuthorityContractTests(unittest.TestCase):
    def test_aot_core_ir_lowering_prefers_loader_demand_graph_for_generic_authority(self) -> None:
        lowering_source = AOT_CORE_IR_LOWERING_PATH.read_text(encoding="utf-8")

        for required_fragment in [
            "var genericDemandLookup = BuildGenericDemandLookup(linkedWorld.GenericInstantiationDemandGraph);",
            "var runtimeGenericContext = ResolveRuntimeGenericContext(",
            "var genericDiagnostic = ResolveGenericDiagnostic(",
            "RuntimeGenericContext = runtimeGenericContext,",
            "GenericDiagnostic = genericDiagnostic,",
            "private static IReadOnlyDictionary<string, GenericInstantiationDemandModel> BuildGenericDemandLookup(",
            "private static RuntimeGenericContextArtifact? ResolveRuntimeGenericContext(",
            "private static GenericDiagnosticArtifact? ResolveGenericDiagnostic(",
            "genericDemandLookup.TryGetValue(subjectId, out var demand)",
        ]:
            self.assertIn(required_fragment, lowering_source)

        self.assertNotIn(
            "return ManagedNaming.TryCreateRuntimeGenericContext(subjectId, definitionSubjectId);",
            lowering_source,
        )


if __name__ == "__main__":
    unittest.main()
