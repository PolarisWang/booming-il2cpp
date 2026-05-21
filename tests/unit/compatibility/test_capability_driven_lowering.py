from __future__ import annotations

import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
CONTRACTS_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Contracts" / "ManagedClosureContracts.cs"
CODEGEN_STAGE_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Generator" / "CodeGenStage.cs"
LOWERING_PLANNER_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Generator" / "NativeReferenceLoweringPlanner.cs"
EMITTER_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Generator" / "NativeReferenceProofEmitter.cs"
CATALOG_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "ReferenceProof"
    / "NativeReferenceProofCatalog.cs"
)


class CapabilityDrivenLoweringTests(unittest.TestCase):
    def test_codegen_owns_lowering_family_selection_and_managed_plan_artifact(self) -> None:
        contracts_source = CONTRACTS_PATH.read_text(encoding="utf-8")
        codegen_source = CODEGEN_STAGE_PATH.read_text(encoding="utf-8")
        emitter_source = EMITTER_PATH.read_text(encoding="utf-8")

        self.assertIn('NativeReferenceLoweringPlan = "native-reference.lowering-plan.json"', contracts_source)
        self.assertIn("NativeReferenceLoweringPlan", contracts_source)
        self.assertIn("NativeReferenceLoweringPlanner", codegen_source)
        self.assertIn("ManagedClosureArtifactNames.NativeReferenceLoweringPlan", emitter_source)

        for retired_marker in [
            "CreateLoweringPlan(",
            "TryCreatePInvokeDllImportMinimalLoweringPlan(",
            "TryCreateReflectionQueryMinimalLoweringPlan(",
            "IsStaticCallCtorGetterEntryPointShape(",
            "ValidateConstructorThenInstanceCallEntryPointShape(",
        ]:
            self.assertNotIn(retired_marker, emitter_source)

    def test_lowering_family_vocabulary_is_frozen_in_codegen_planner(self) -> None:
        planner_source = LOWERING_PLANNER_PATH.read_text(encoding="utf-8")
        catalog_source = CATALOG_PATH.read_text(encoding="utf-8")

        for constant_name, lowering_family in [
            ("ManagedObjectCapturedStateInstanceMessageMinimal", "managed-object.captured-state-instance-message.minimal"),
            ("ManagedGenericStaticForwarderCapturedGetterMinimal", "managed-generic.static-forwarder-captured-getter.minimal"),
            ("ReflectionClosedTypeQueryMinimal", "reflection.closed-type-query.minimal"),
            ("InteropPInvokeDirectCallMinimal", "interop.pinvoke-direct-call.minimal"),
        ]:
            self.assertIn(lowering_family, catalog_source)
            self.assertIn(f"NativeReferenceProofCatalog.{constant_name}", planner_source)


if __name__ == "__main__":
    unittest.main()
