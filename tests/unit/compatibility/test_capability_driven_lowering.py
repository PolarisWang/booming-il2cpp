from __future__ import annotations

import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
CONTRACTS_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Contracts" / "ManagedClosureContracts.cs"
CODEGEN_STAGE_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.CodeGen" / "CodeGenStage.cs"
LOWERING_PLANNER_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.CodeGen" / "NativeReferenceLoweringPlanner.cs"
EMITTER_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.CodeGen" / "NativeReferenceProofEmitter.cs"


class Phase3CapabilityDrivenLoweringTests(unittest.TestCase):
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

        for lowering_family in [
            "managed-object.captured-state-instance-message.minimal",
            "managed-generic.static-forwarder-captured-getter.minimal",
            "reflection.closed-type-query.minimal",
            "interop.pinvoke-direct-call.minimal",
        ]:
            self.assertIn(lowering_family, planner_source)


if __name__ == "__main__":
    unittest.main()
